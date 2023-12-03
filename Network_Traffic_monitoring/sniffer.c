#include <stdio.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif

int total_network_flows = 0;
int tcp_network_flows = 0;
int udp_network_flows = 0;
int total_packets_received = 0;
int tcp_packets_received = 0;
int udp_packets_received = 0;
int total_bytes_tcp = 0;
int total_bytes_udp = 0;
int retransmitted_tcp_packets = 0;
int retransmitted_udp_packets = 0;

void process_packet(const u_char *packet, struct ip *ip_header) {
    // Process TCP packets
    if (ip_header->ip_p == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ip));
        // Check if TCP packet is a retransmission
        if (tcp_header->th_flags & TH_ACK) {
            // Mark as retransmitted
            printf("Retransmitted TCP packet\n");
            retransmitted_tcp_packets++;
        }
        // Process TCP packet here
        // ...
        tcp_packets_received++;
        total_packets_received++;
        total_bytes_tcp += ntohs(ip_header->ip_len) - sizeof(struct ip);
        tcp_network_flows++;
    }
    // Process UDP packets
    else if (ip_header->ip_p == IPPROTO_UDP) {
        struct udphdr *udp_header = (struct udphdr *)(packet + sizeof(struct ip));
        // Check if UDP packet is a retransmission
        // UDP packets are connectionless, so retransmissions are not applicable
        // ...
        udp_packets_received++;
        total_packets_received++;
        total_bytes_udp += ntohs(ip_header->ip_len) - sizeof(struct ip);
        udp_network_flows++;
    }
}

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ip *ip_header = (struct ip *)(packet + 14); // Skip Ethernet header
    process_packet(packet, ip_header);
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct pcap_pkthdr header;
    const u_char *packet;
    

    // Open network interface for live capture
    // @TODO: Replace "eth0" with the name of the network interface
    handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening interface: %s\n", errbuf);
        return 1;
    }
    
    DEBUG_PRINT("Starting packet capture...\n");

    // // Capture and process packets indefinitely
    // while (1) {
    //     packet = pcap_next(handle, &header);
    //     packet_handler(NULL, &header, packet);
    // }

    /* Attempt to capture one packet. If there is no network traffic
      and the timeout is reached, it will return NULL */
     packet = pcap_next(handle, &header);
     if (packet == NULL) {
        printf("No packet found.\n");
        return 2;
    }

    pcap_close(handle);

    // Print statistics
    printf("Total number of network flows captured: %d\n", total_network_flows);
    printf("Number of TCP network flows captured: %d\n", tcp_network_flows);
    printf("Number of UDP network flows captured: %d\n", udp_network_flows);
    printf("Total number of packets received: %d\n", total_packets_received);
    printf("Total number of TCP packets received: %d\n", tcp_packets_received);
    printf("Total number of UDP packets received: %d\n", udp_packets_received);
    printf("Total bytes of TCP packets received: %d\n", total_bytes_tcp);
    printf("Total bytes of UDP packets received: %d\n", total_bytes_udp);

    return 0;
}
