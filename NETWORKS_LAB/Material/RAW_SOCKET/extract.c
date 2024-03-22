#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netinet/tcp.h>

#define MAX_PACKET_SIZE 65536

void process_packet(unsigned char *buffer, int size);

int main()
{
    int raw_socket;
    struct sockaddr_in saddr;
    int saddr_len;
    unsigned char buffer[MAX_PACKET_SIZE];

    // Create a raw socket
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (raw_socket == -1)
    {
        perror("Failed to create raw socket");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Receive packets
        saddr_len = sizeof(saddr);
        int packet_size = recvfrom(raw_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&saddr, (socklen_t *)&saddr_len);
        if (packet_size < 0)
        {
            perror("Failed to receive packet");
            exit(EXIT_FAILURE);
        }

        // Process received packet
        process_packet(buffer, packet_size);
    }

    return 0;
}

void process_packet(unsigned char *buffer, int size)
{
    struct iphdr *ip_header = (struct iphdr *)(buffer);
    struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl * 4));

    // Check if TCP packet
    if (ip_header->protocol == IPPROTO_TCP)
    {
        // Extract and print relevant information
        printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
        printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));

        printf("Source Port: %d\n", ntohs(tcp_header->source));
        printf("Destination Port: %d\n", ntohs(tcp_header->dest));
        printf("Sequence Number: %u\n", ntohl(tcp_header->seq));
        printf("Acknowledgement Number: %u\n", ntohl(tcp_header->ack_seq));
        printf("Flags: ");
        if (tcp_header->syn)
            printf("SYN ");
        if (tcp_header->ack)
            printf("ACK ");
        if (tcp_header->fin)
            printf("FIN ");
        if (tcp_header->rst)
            printf("RST ");
        printf("\n");
        printf("Data Length: %d\n", size - (ip_header->ihl * 4 + tcp_header->doff * 4));
        printf("Data: %s\n", buffer + (ip_header->ihl * 4 + tcp_header->doff * 4));
        printf("\n");
    }
}
