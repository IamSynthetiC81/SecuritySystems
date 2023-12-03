#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

int linkhdrlen,TCPpackets,UDPpackets,totalPackets,TCPbytes,UDPbytes;


void get_link_header_len(pcap_t* handle){
    int linktype;
 
    // Determine the datalink layer type.
    if ((linktype = pcap_datalink(handle)) == PCAP_ERROR) {
        fprintf(stderr, "pcap_datalink(): %s\n", pcap_geterr(handle));
        return;
    }
 
    // Set the datalink layer header size.
    switch (linktype){
    case DLT_NULL:
        linkhdrlen = 4;
        break;
 
    case DLT_EN10MB:
        linkhdrlen = 14;
        break;
 
    case DLT_SLIP:
    case DLT_PPP:
        linkhdrlen = 24;
        break;
 
    default:
        printf("Unsupported datalink (%d)\n", linktype);
        linkhdrlen = 0;
    }

}

void menu(){
    printf( "Options:\n-i Select the network interface name (e.g., eth0)\n-r Packet capture file name (e.g., test.pcap)\n-f Filter expression in string format (e.g., port 8080)\n-h This help message");
}

void pak_handler(u_char *args,const struct pcap_pkthdr *packet_header,const u_char *packetptr){
    struct ip* iphdr;
    //struct icmp* icmphdr;
    struct tcphdr* tcphdr;
    struct udphdr* udphdr;
    char iphdrInfo[256];
    char srcip[256];
    char dstip[256];

    int packet_len,pak_hdr_len;

    // Skip the datalink layer header and get the IP header fields.
    packetptr += linkhdrlen;
    iphdr = (struct ip*)packetptr;
    strcpy(srcip, inet_ntoa(iphdr->ip_src));
    strcpy(dstip, inet_ntoa(iphdr->ip_dst));
    sprintf(iphdrInfo, "ID:%d TOS:0x%x, TTL:%d IpLen:%d DgLen:%d",
            ntohs(iphdr->ip_id), iphdr->ip_tos, iphdr->ip_ttl,
            4*iphdr->ip_hl, ntohs(iphdr->ip_len));
    
    packet_len = ntohs(iphdr->ip_len) - 4*iphdr->ip_hl;//total number of bytes after the IP header 

    // Advance to the transport layer header then parse and display
    // the fields based on the type of hearder: tcp or udp.
    packetptr += 4*iphdr->ip_hl;

    switch (iphdr->ip_p){
        
    case IPPROTO_TCP:   
        tcphdr = (struct tcphdr*)packetptr;
        pak_hdr_len = 4*tcphdr->th_off;
        printf("TCP  %s:%d -> %s:%d\n", srcip, ntohs(tcphdr->th_sport),
               dstip, ntohs(tcphdr->th_dport));

        printf("Total TCP packet lenght: %d\n", packet_len);
        TCPbytes+=packet_len;
        printf("TCP header length: %d\n", pak_hdr_len);
        printf("Payload length: %d\n", packet_len - pak_hdr_len);
        printf("Payload memory pointer: %hhn\n",tcphdr+pak_hdr_len);
        /*printf("%c%c%c%c%c%c Seq: 0x%x Ack: 0x%x Win: 0x%x HeaderLen: %d\n",
               (tcphdr->th_flags & TH_URG ? 'U' : '*'),
               (tcphdr->th_flags & TH_ACK ? 'A' : '*'),
               (tcphdr->th_flags & TH_PUSH ? 'P' : '*'),
               (tcphdr->th_flags & TH_RST ? 'R' : '*'),
               (tcphdr->th_flags & TH_SYN ? 'S' : '*'),
               (tcphdr->th_flags & TH_SYN ? 'F' : '*'),
               ntohl(tcphdr->th_seq), ntohl(tcphdr->th_ack),
               ntohs(tcphdr->th_win), 4*tcphdr->th_off);                       Unecessary???? If not just uncomment please!*/
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        TCPpackets += 1;
        break;
 
    case IPPROTO_UDP:
        udphdr = (struct udphdr*)packetptr;
        pak_hdr_len = 8; //UDP has a fixed header length
        printf("UDP  %s:%d -> %s:%d\n", srcip, ntohs(udphdr->uh_sport),
               dstip, ntohs(udphdr->uh_dport));
        printf("Total UDP packet lenght: %d\n", packet_len);// according to IP
        //printf("Total UDP packet lenght according to UDP: %d\n", ntohs(udphdr->uh_ulen));
        UDPbytes+=packet_len;
        printf("UDP header length: %d\n", pak_hdr_len);
        printf("Payload length: %d\n", packet_len - pak_hdr_len);
        printf("Payload memory pointer: %hhn\n",packetptr+pak_hdr_len);
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        UDPpackets += 1;
        break;
 
    default:
        // @TODO : ingore non UDP or TCP packets
        break;
    }
    totalPackets++;
    return;
}

void capture(pcap_t *handle, char *filter_exp){
    struct bpf_program filter;

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr,"Bad filter - %s\n", pcap_geterr(handle));
        return;
    }


    if (pcap_setfilter(handle, &filter) == -1) {
        fprintf(stderr,"Error setting filter - %s\n", pcap_geterr(handle));
        return;
    }

    get_link_header_len(handle);

    // @TODO crashes here
    pcap_loop(handle, 0, pak_handler, NULL);
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

int main( int argc, char *argv[]){
    pcap_t *handle;
    char error_buffer[PCAP_ERRBUF_SIZE];
    int timeout_limit = 10000;

    int op = 0;
    char* dev = NULL;
    char* file_name = NULL;
    char* filter = NULL;

    dropTheSlang();

    //printf("#Num of arguments: %d\n",argc);
    for(int i=0;i<argc;i++){
        //printf("Parcing argument: %d\n",i);

       //It prints the help message(Named it menu because this ece school, not art school and I am not creative at all...sorry)
       if(strcmp(argv[i],"-h")==0)
            menu();    
       if(strcmp(argv[i],"-i")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide an adapter name!\n");
                return 1;
            }
            dev = argv[i+1];
            op = 0;
            i+=1;
       }
        if(strcmp(argv[i],"-r")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide a capture file!");
                return 1;
            }
            file_name = argv[i+1];
            op = 1;
            i+=1;
        }
        if(strcmp(argv[i],"-f")==0){
            if(i+1>=argc || argv[i+1][0] == '-'){
                printf("Please provide an expression!");
                return 1;
            }
            filter = argv[i+1];
            i+=1;
       }
    }

    handle = op ? pcap_open_offline(file_name, error_buffer) : pcap_open_live(dev,BUFSIZ,0,timeout_limit,error_buffer);
    if (handle == NULL) {
        fprintf(stderr, "Could not open device %s: %s\n", dev, error_buffer);
        return 2;
    }

    capture(handle,filter);

    printf("Total packets captured: %d\n",totalPackets);
    printf("TCP packets captured: %d\n",TCPpackets);
    printf("UDP packets captured: %d\n",UDPpackets);
    printf("TCP packet bytes captured: %d\n",TCPbytes);
    printf("UDP packet bytes captured: %d\n",UDPbytes);

    return 0;
}