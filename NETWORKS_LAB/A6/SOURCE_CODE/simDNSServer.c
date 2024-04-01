#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>

#define LOCAL_IP_ADDRESS "192.168.43.203"

int main()
{
    int raw_socket;
    struct sockaddr_ll local_addr;

    // Create raw socket
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Prepare local address structure
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sll_family = AF_PACKET;
    local_addr.sll_protocol = htons(ETH_P_ALL);
    // Change the interface index to the desired IP address
    inet_pton(AF_INET, LOCAL_IP_ADDRESS, &local_addr.sll_addr[2]); // Assuming IPv4 address

    // Bind the raw socket to the local address
    if (bind(raw_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("Bind failed");
        close(raw_socket);
        exit(1);
    }

    // Receive packets
    while (1)
    {
        char buffer[2048];
        int data_size = recv(raw_socket, buffer, sizeof(buffer), 0);
        if (data_size < 0)
        {
            perror("Recv failed");
            close(raw_socket);
            exit(1);
        }
        // Get the protocol from the Ethernet header
        struct ethhdr *eth = (struct ethhdr *)buffer;
        unsigned short protocol = ntohs(eth->h_proto);
        printf("Protocol: %d\n", protocol);
        // Get the IP address from the IP header
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        char source_ip[INET_ADDRSTRLEN];
        char dest_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip->saddr, source_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &ip->daddr, dest_ip, INET_ADDRSTRLEN);
        printf("Source IP: %s\n", source_ip);
        printf("Destination IP: %s\n", dest_ip);
        // Get the MAC address from the Ethernet header
        unsigned char source_mac[6];
        unsigned char dest_mac[6];
        for (int i = 0; i < 6; i++)
        {
            source_mac[i] = eth->h_source[i];
            dest_mac[i] = eth->h_dest[i];
        }
        printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", source_mac[0], source_mac[1], source_mac[2], source_mac[3], source_mac[4], source_mac[5]);
        printf("Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", dest_mac[0], dest_mac[1], dest_mac[2], dest_mac[3], dest_mac[4], dest_mac[5]);
        printf("\n");
    }

    close(raw_socket);
    return 0;
}
