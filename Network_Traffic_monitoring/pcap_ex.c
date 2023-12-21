#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define LOG_PATH "log/log.txt"

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif

#define _MAX_LOGS_ 10000

// Statistics
int total_network_flows = 0;
int tcp_network_flows = 0;
int udp_network_flows = 0;
int total_packets_received = 0;
int tcp_packets_received = 0;
int udp_packets_received = 0;
int tcp_bytes_received = 0;
int udp_bytes_received = 0;
int retransmitted_tcp_packets = 0;
int retransmitted_udp_packets = 0;

int link_header_length;

typedef struct network_flow {
    unsigned int src_ip;
    unsigned int dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned char protocol;
}nflow_t;

nflow_t *network_flows;
int network_flows_count = 0;

int is_new_flow(const struct network_flow *flow) {
    for (int i = 0; i < network_flows_count; i++) {
        if (memcmp(flow, &network_flows[i], sizeof(nflow_t)) == 0) {
            return 0;                                                                                                   // Flow already exists
        }
    }
    return 1;                                                                                                           // New flow
}

typedef enum OPERATION {
    __ONLINE,
    __OFFLINE,
    __NULL
} op_t;

pcap_t * handle = NULL;

void printToLog(const nflow_t *flow) {
    FILE *fp = fopen(LOG_PATH, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(fp, "%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d\n",
        (flow->src_ip >> 24) & 0xFF,
        (flow->src_ip >> 16) & 0xFF,
        (flow->src_ip >> 8) & 0xFF,
        flow->src_ip & 0xFF,
        flow->src_port,
        (flow->dst_ip >> 24) & 0xFF,
        (flow->dst_ip >> 16) & 0xFF,
        (flow->dst_ip >> 8) & 0xFF,
        flow->dst_ip & 0xFF,
        flow->dst_port
    );

    flow->protocol == IPPROTO_TCP ? fprintf(fp, "Protocol: TCP\n") : fprintf(fp, "Protocol: UDP\n");

    fclose(fp);
}

void get_link_header_len(pcap_t* handle){
    // Determine the datalink layer type.
    int linktype;
    if ((linktype = pcap_datalink(handle)) == PCAP_ERROR) {
        fprintf(stderr, "pcap_datalink(): %s\n", pcap_geterr(handle));
        return;
    }

    // Set the datalink layer header size.
    switch (linktype){
        case DLT_NULL:
            link_header_length = 4;
            break;
        case DLT_EN10MB:
            link_header_length = 14;
            break;
        case DLT_SLIP:
        case DLT_PPP:
            link_header_length = 24;
            break;
        default:
            printf("Unsupported datalink (%d)\n", linktype);
            link_header_length = 0;
            break;
    }

}

void menu(){
    printf( "Options:\n-i Select the network interface name (e.g., eth0)\n-r Packet capture file name (e.g., test.pcap)\n-f Filter expression in string format (e.g., port 8080)\n-h This help message");
}

void packet_processor(u_char *args,const struct pcap_pkthdr *packet_header,const u_char *packetptr){
    struct ip* Header_IP;
    struct tcphdr* Header_TCP;
    struct udphdr* Header_UDP;
    char IP_Header_Info[256];
    char IP_source[256];
    char IP_dest[256];

    int packet_len, pak_hdr_len;

     // Skip the datalink layer header and get the IP header fields.
    packetptr += link_header_length;
    Header_IP = (struct ip*)packetptr;

    const int ip_header_length = Header_IP->ip_hl << 2;                                                                 //total number of bytes in the IP header
    const unsigned short src_port = ntohs(*(unsigned short *)(packetptr + 14 + ip_header_length));
    const unsigned short dst_port = ntohs(*(unsigned short *)(packetptr + 14 + ip_header_length + 2));

    const nflow_t flow = {
        .src_ip = Header_IP->ip_src.s_addr,
        .dst_ip = Header_IP->ip_dst.s_addr,
        .src_port = src_port,
        .dst_port = dst_port,
        .protocol = Header_IP->ip_p
    };

    int newFlow = is_new_flow(&flow);

    if(newFlow) {
        network_flows = realloc(network_flows, sizeof(nflow_t) * (network_flows_count + 1));
        network_flows[network_flows_count++] = flow;
        total_network_flows++;
    }

    strcpy(IP_source, inet_ntoa(Header_IP->ip_src));
    strcpy(IP_dest, inet_ntoa(Header_IP->ip_dst));
    sprintf(IP_Header_Info, "ID:%d TOS:0x%x, TTL:%d IpLen:%d DgLen:%d",
            ntohs(Header_IP->ip_id), Header_IP->ip_tos, Header_IP->ip_ttl,
            4*Header_IP->ip_hl, ntohs(Header_IP->ip_len));

    packet_len = ntohs(Header_IP->ip_len) - 4*Header_IP->ip_hl;                                                         //total number of bytes after the IP header
    packetptr += 4*Header_IP->ip_hl;                                                                                    // Advance to the transport layer header then parse and display
                                                                                                                        // the fields based on the type of hearder: tcp or udp.
    switch (Header_IP->ip_p){
    case IPPROTO_TCP:
        Header_TCP = (struct tcphdr*)packetptr;
        pak_hdr_len = 4*Header_TCP->th_off;
        printf("TCP  %s:%d -> %s:%d\n", IP_source, ntohs(Header_TCP->th_sport),
               IP_dest, ntohs(Header_TCP->th_dport));

        if (Header_TCP->th_flags & TH_ACK) {                                                                            //Check if the packet is a retransmission
            printf("TCP Packet is a retransmittion\n");
            retransmitted_tcp_packets++;
        }

        printf("Total TCP packet lenght: %d\n", packet_len);
        tcp_bytes_received+=packet_len;
        printf("TCP header length: %d\n", pak_hdr_len);
        printf("Payload length: %d\n", packet_len - pak_hdr_len);
        printf("Payload memory pointer: %p\n",Header_TCP+pak_hdr_len);
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        tcp_packets_received += 1;

        if (newFlow)
            tcp_network_flows++;

        break;

    case IPPROTO_UDP:
        Header_UDP = (struct udphdr*)packetptr;
        pak_hdr_len = 8; //UDP has a fixed header length
        printf("UDP  %s:%d -> %s:%d\n", IP_source, ntohs(Header_UDP->uh_sport),
               IP_dest, ntohs(Header_UDP->uh_dport));
        printf("Total UDP packet lenght: %d\n", packet_len);// according to IP
        //printf("Total UDP packet lenght according to UDP: %d\n", ntohs(udphdr->uh_ulen));
        udp_bytes_received+=packet_len;
        printf("UDP header length: %d\n", pak_hdr_len);
        printf("Payload length: %d\n", packet_len - pak_hdr_len);
        printf("Payload memory pointer: %p\n",packetptr+pak_hdr_len);
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        udp_packets_received += 1;
        udp_bytes_received += ntohs(Header_IP->ip_len) - sizeof(struct ip);

        if (newFlow)
            udp_network_flows++;

        break;

    default: //ignore non UDP or TCP packets
        break;
    }
    total_packets_received++;
}

void terminate_process(int signum) {
    pcap_breakloop(handle);
    pcap_close(handle);
}

void capture(pcap_t *handle, const char *filter_exp){
    // @TODO : Fix filters
    // struct bpf_program filter;
    //
    // if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
    //     fprintf(stderr,"Bad filter - %s\n", pcap_geterr(handle));
    //     return;
    // }
    // if (pcap_setfilter(handle, &filter) == -1) {
    //     fprintf(stderr,"Error setting filter - %s\n", pcap_geterr(handle));
    //     return;
    // }
    get_link_header_len(handle);

    signal(SIGINT, terminate_process);                                                                              //Catch the sigint signal and terminate pcap_loop
    pcap_loop(handle, 0, packet_processor, NULL);
}

void dropTheSlang(){
    printf("\n");
    printf("***************************************************\n");
    printf("*                                                 *\n");
    printf("*      SIMPLE PACKET CAPTURE TOOLARA(TM)          *\n");
    printf("*        Created by: The Coolest HMMYtzhdes       *\n");
    printf("*     (Krhtikakhs Marios, Georgakopoulos Hlias)   *\n");
    printf("*                                                 *\n");
    printf("***************************************************\n");

}

// int validDev(const char* name, char* error_buffer) {
//     // Get all devs
//     pcap_if_t *alldevsp;
//
//     const int retval = pcap_findalldevs(&alldevsp, error_buffer);
//     if (retval == 0) {
//         int i = 0;
//         for (pcap_if_t* temp = alldevsp; temp; temp=temp->next ) {
//             DEBUG_PRINT("%d : %s\n", i++, temp->name);
//             // if (strcmp(temp->name, name) == 0)
//             //     return 1;
//         }
//     }
//     pcap_freealldevs((alldevsp));
//     return 1;
//     return 0;
// }

int main(const int argc, char *argv[]){
    dropTheSlang();
    char error_buffer[PCAP_ERRBUF_SIZE];
    const int timeout_limit = 10000;

    op_t op = __NULL;
    char* dev = NULL;
    char* file_name = NULL;
    char* filter = NULL;

    for(int i=0;i<argc;i++){
        //It prints the help message(Named it menu because this ece school, not art school and I am not creative at all...sorry)
        if(strcmp(argv[i],"-h")==0) {
            menu();
            return 0;
        }

        if(strcmp(argv[i],"-i")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide an adapter name!\n");
                return 1;
            }
            op = __ONLINE;
            dev = argv[i+1];
            i+=1;
        } else if(strcmp(argv[i],"-r")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide a capture file!");
                return 1;
            }
            op = __OFFLINE;
            file_name = argv[i+1];
            i+=1;
        } else if(strcmp(argv[i],"-f")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide an expression!");
                return 1;
            }
            filter = argv[i+1];
            i+=1;
       }
    }

    if (op == __OFFLINE ) {
        handle = pcap_open_offline(file_name, error_buffer);
        if (handle == NULL) {
            fprintf(stderr, "Could not open device %s: %s\n", file_name, error_buffer);
            return 2;
        }
    }else if (op == __ONLINE){
        handle = pcap_open_live(dev,BUFSIZ,0,timeout_limit,error_buffer);
        if (handle == NULL) {
            fprintf(stderr, "Could not open device %s: %s\n", dev, error_buffer);
            return 2;
        }
    }

    capture(handle,filter);

    printf("Total packets captured: %d\n",total_packets_received);
    printf("TCP packets captured: %d\n",tcp_packets_received);
    printf("UDP packets captured: %d\n",udp_packets_received);
    printf("TCP packet bytes captured: %d\n",tcp_bytes_received);
    printf("UDP packet bytes captured: %d\n",udp_bytes_received);

    printf("TCP network flows: %d\n",tcp_network_flows);
    printf("UDP network flows: %d\n",udp_network_flows);
    printf("Total network flows: %d\n",total_network_flows);

    printf("Retransmited TCP packets: %d\n", retransmitted_tcp_packets);

    return 0;
}
