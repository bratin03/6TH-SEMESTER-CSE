/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
                    Name: Somya Kumar
                    Student ID: 21CS30050
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

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
#include <sys/select.h>
#include <net/if.h>
#include <ctype.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>

#define INTERFACE "lo"
#define LOCAL_IP_ADDRESS "127.0.0.2"
#define SOURCE_MAC "ac:74:b1:ac:5c:1a"

#define VERBOSE
// #define QUERY_PACKET
// #define DEBUG_PROCESSING
// #define RESPONSE_CHECK
// #define PACKET_CHECK
// #define DEBUG_RECV
// #define DEBUG_MAC

#define PROTOCOL_NO 254
#define PROB 0.5
#define BUFFER_SIZE 1024
#define TTL 64

#define LITTLE_ENDIAN_ORDER 0
#define BIG_ENDIAN_ORDER 1
#define PROTOCOL_ENDIANNESS LITTLE_ENDIAN_ORDER

/**
 * Signal handler for SIGINT and SIGQUIT
 * @param signum Signal number
 * @return void
 */
void sigint_handler(int signum)
{
    if (signum == SIGINT || signum == SIGQUIT)
    {
        printf("\nReceived signal %d\n", signum);
        printf("Exiting...\n");
        exit(EXIT_SUCCESS);
    }
}

/**
 * Get the least significant byte of a 32-bit integer
 * @param x 32-bit integer
 * @return unsigned char Least significant byte
 */
unsigned char getLSB32(unsigned int x)
{
    return (unsigned char)(x & 0xFF);
}

/**
 * Get the integer from the least significant byte of a 32-bit integer
 * @param c Least significant byte
 * @return unsigned int Integer value
 */
unsigned int getIntFromLSB32(unsigned char c)
{
    return (unsigned int)c;
}

/**
 * Get the integer from the buffer
 * @param buffer Buffer
 * @param order Endianness
 * @return unsigned int Integer value
 */
unsigned int getIntFromBuf(unsigned char *buffer, int order)
{
    unsigned int value = 0;
    if (order == LITTLE_ENDIAN_ORDER)
    {
        value = value | (buffer[0] << 24);
        value = value | (buffer[1] << 16);
        value = value | (buffer[2] << 8);
        value = value | (buffer[3]);
    }
    else
    {
        value = value | (buffer[3] << 24);
        value = value | (buffer[2] << 16);
        value = value | (buffer[1] << 8);
        value = value | (buffer[0]);
    }
    return value;
}

/**
 * Get the new ID for the packet
 * @return int New ID
 */
int get_new_id()
{
    static int id = 0;
    id++;
    id %= 65536;
    return id;
}

/**
 * Calculate the checksum of the buffer
 * @param ptr Buffer
 * @param nbytes Number of bytes
 * @return unsigned short Checksum
 */
unsigned short calculate_checksum(unsigned short *ptr, int nbytes)
{
    unsigned long sum = 0;
    for (; nbytes > 1; nbytes -= 2)
        sum += *ptr++;
    if (nbytes == 1)
        sum += *(unsigned char *)ptr;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

/**
 * Drop the message with a given probability
 * @param prob Probability
 * @return int 1 if message is dropped, 0 otherwise
 */
int dropmessage(float prob)
{
    float random = (float)rand() / (float)(RAND_MAX);
    if (random < prob)
    {
        return 1;
    }
    return 0;
}

/**
 * Validate the MAC address format
 * @param mac MAC address
 * @return int 1 if valid, 0 otherwise
 */
int validateMacAddrFormat(char *mac)
{
    int len = strlen(mac);
    if (len != 17)
    {
        return 0;
    }
    for (int i = 0; i < len; i++)
    {
        if (i % 3 == 2)
        {
            if (mac[i] != ':')
            {
                return 0;
            }
        }
        else
        {
            if (!isxdigit(mac[i]))
            {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * Main function
 * @param argc Number of arguments
 * @param argv Arguments
 * @return int Exit status
 */
int main(int argc, char *argv[])
{
    srand(time(0));
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigint_handler);
    int raw_socket;
    struct sockaddr_ll local_addr;
    char *source_mac_addr = (char *)malloc(20 * sizeof(char));
    if (argc == 1)
    {
#ifdef DEBUG_MAC
        printf("Using default source MAC address\n");
        printf("Source MAC: %s\n", SOURCE_MAC);
#endif
        strcpy(source_mac_addr, SOURCE_MAC);
    }
    else if (argc == 2)
    {
        strcpy(source_mac_addr, argv[1]);
        if (!validateMacAddrFormat(source_mac_addr))
        {
            fprintf(stderr, "Error: Invalid MAC address format\n");
            fprintf(stderr, "Please provide the MAC address in the format XX:XX:XX:XX:XX:XX\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        fprintf(stderr, "For Command Line Arguments:\n");
        fprintf(stderr, "Usage: %s <Source MAC Address>\n", argv[0]);
        fprintf(stderr, "For Default Source MAC Address:\n");
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sll_family = AF_PACKET;
    local_addr.sll_protocol = htons(ETH_P_ALL);
    local_addr.sll_ifindex = if_nametoindex(INTERFACE);
    sscanf(source_mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &local_addr.sll_addr[0], &local_addr.sll_addr[1], &local_addr.sll_addr[2], &local_addr.sll_addr[3], &local_addr.sll_addr[4], &local_addr.sll_addr[5]);

    // Bind the socket
    if (bind(raw_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("Bind failed");
        close(raw_socket);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        unsigned char buffer[BUFFER_SIZE];
        // Receive the packet
        int data_size = recv(raw_socket, buffer, sizeof(buffer), 0);
        if (data_size < 0)
        {
            perror("Recv failed");
            close(raw_socket);
            exit(EXIT_FAILURE);
        }
#ifdef DEBUG_RECV
        printf("Data Size: %d\n", data_size);
        for (int i = 0; i < data_size; i++)
        {
            printf("%d ", buffer[i] & 0xff);
        }
        printf("\n");
#endif
        struct ethhdr *eth = (struct ethhdr *)buffer;
        unsigned short eth_protocol = ntohs(eth->h_proto);
        // Check if the protocol is IP
        if (eth_protocol != ETH_P_IP)
        {
#ifdef DEBUG_RECV
            fprintf(stderr, "Ethernet Protocol: %d\n", eth_protocol);
#endif
            continue;
        }
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        char source_ip[INET_ADDRSTRLEN];
        char dest_ip[INET_ADDRSTRLEN];
        unsigned int protocol = getIntFromLSB32(ip->protocol);
        inet_ntop(AF_INET, &ip->saddr, source_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &ip->daddr, dest_ip, INET_ADDRSTRLEN);
        // Check if the destination IP is the local IP and the protocol is the custom protocol
        if (strcmp(dest_ip, LOCAL_IP_ADDRESS) == 0 && protocol == PROTOCOL_NO)
        {
            unsigned char source_mac[6];
            unsigned char dest_mac[6];
            for (int i = 0; i < 6; i++)
            {
                source_mac[i] = eth->h_source[i];
                dest_mac[i] = eth->h_dest[i];
            }
            // Check if the checksum is valid
            if (calculate_checksum((unsigned short *)(buffer + sizeof(struct ethhdr)), 4 * (ip->ihl)) == 0)
            {
#ifdef DEBUG_RECV
                printf("Checksum verified\n");
#endif
                unsigned char dns_response[BUFFER_SIZE];

                int curr_pos = sizeof(struct ethhdr) + sizeof(struct iphdr);

                unsigned char *data = buffer + sizeof(struct ethhdr) + sizeof(struct iphdr);

                // Copy the DNS header
                dns_response[curr_pos++] = data[0];
                dns_response[curr_pos++] = data[1];
                // Set the DNS type bit to response
                dns_response[curr_pos++] = getLSB32(1);
                data += 2;
                unsigned char message_type = *data;
                data += 1;
                if (message_type != 0)
                {
#ifdef DEBUG_RECV
                    fprintf(stderr, "Message Type is not a query\n");
#endif
                    continue;
                }

#ifdef VERBOSE
                for (int ii = 0; ii < 40; ii++)
                    printf("-");
                printf("\n");
                printf("Packet received\n");
                printf("Source IP: %s\n", source_ip);
                printf("Destination IP: %s\n", dest_ip);
                printf("Protocol: %d\n", protocol);
                printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", source_mac[0], source_mac[1], source_mac[2], source_mac[3], source_mac[4], source_mac[5]);
                printf("Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", dest_mac[0], dest_mac[1], dest_mac[2], dest_mac[3], dest_mac[4], dest_mac[5]);
#endif
#ifdef QUERY_PACKET
                for (int i = 0; i < (int)(data_size - sizeof(struct ethhdr) - sizeof(struct iphdr)); i++)
                {
                    printf("%d ", buffer[i + sizeof(struct ethhdr) + sizeof(struct iphdr)] & 0xff);
                }
#endif
                // Drop the message with a given probability
                if (dropmessage(PROB))
                {
#ifdef VERBOSE
                    printf("\nDropping the Query\n");
                    for (int ii = 0; ii < 40; ii++)
                        printf("-");
                    printf("\n");
#endif

                    continue;
                }

#ifdef VERBOSE
                printf("\nProcessing the Query\n");
#endif

                unsigned char num_queries = *data;
                unsigned int num_queries_int = getIntFromLSB32(num_queries);
                dns_response[curr_pos++] = num_queries;
                data += 1;
#ifdef DEBUG_PROCESSING
                printf("Number of queries: %d\n", num_queries_int);
#endif
                // Process each query
                for (unsigned int i = 0; i < num_queries_int; i++)
                {
                    unsigned int len = getIntFromBuf(data, PROTOCOL_ENDIANNESS);
                    data += 4;
#ifdef DEBUG_PROCESSING
                    printf("Length of domain name: %d\n", len);
#endif
                    unsigned char *domain_name = (unsigned char *)malloc((len + 1) * sizeof(unsigned char));
                    for (unsigned int j = 0; j < len; j++)
                    {
                        domain_name[j] = *data;
                        data += 1;
                    }
                    domain_name[len] = '\0';
#ifdef DEBUG_PROCESSING
                    printf("Domain Name: %s\n", domain_name);
#endif
                    struct hostent *host_entry;
                    char *hostname = (char *)domain_name;
                    char ip_buffer[INET_ADDRSTRLEN];
                    host_entry = gethostbyname(hostname);
                    if (host_entry == NULL)
                    {
#ifdef VERBOSE
                        printf("%s: %s\n", domain_name, "Not found");
#endif
                        dns_response[curr_pos++] = getLSB32(0);
                        curr_pos += 4;
                    }
                    else
                    {
                        if (inet_ntop(AF_INET, host_entry->h_addr_list[0], ip_buffer, sizeof(ip_buffer)) == NULL)
                        {
                            fprintf(stderr, "inet_ntop failed\n");
                            close(raw_socket);
                            exit(EXIT_FAILURE);
                        }
                        else
                        {
#ifdef VERBOSE
                            printf("%s: %s\n", domain_name, ip_buffer);
#endif
                            dns_response[curr_pos++] = getLSB32(1);
                            char *token;
                            unsigned char ip[4];
                            memset(ip, 0, sizeof(ip));
                            token = strtok(ip_buffer, ".");
                            ip[0] = getLSB32(atoi(token));
                            token = strtok(NULL, ".");
                            ip[1] = getLSB32(atoi(token));
                            token = strtok(NULL, ".");
                            ip[2] = getLSB32(atoi(token));
                            token = strtok(NULL, ".");
                            ip[3] = getLSB32(atoi(token));
                            dns_response[curr_pos++] = ip[0];
                            dns_response[curr_pos++] = ip[1];
                            dns_response[curr_pos++] = ip[2];
                            dns_response[curr_pos++] = ip[3];
#ifdef DEBUG_PROCESSING
                            printf("IP address for %s : %d.%d.%d.%d\n", hostname, ip[0], ip[1], ip[2], ip[3]);
#endif
                        }
                    }
                }
                // Construct the ip header
                struct iphdr *iph = (struct iphdr *)(dns_response + sizeof(struct ethhdr));
                iph->ihl = getLSB32(5) & 0xF;
                iph->version = getLSB32(4) & 0xF;
                iph->tos = 0 & 0xFF;
                iph->id = htons(get_new_id());
                iph->ttl = getLSB32(TTL);
                iph->protocol = getLSB32(PROTOCOL_NO);
                iph->saddr = inet_addr(LOCAL_IP_ADDRESS);
                iph->daddr = inet_addr(source_ip);
                iph->tot_len = htons(curr_pos - sizeof(struct ethhdr));
                iph->frag_off = 0;
                iph->check = 0;
                iph->check = calculate_checksum((unsigned short *)iph, iph->ihl * 4);

                // Construct the ethernet header
                struct ethhdr *eth = (struct ethhdr *)dns_response;
                eth->h_proto = htons(ETH_P_IP);
                for (int i = 0; i < 6; i++)
                {
                    eth->h_dest[i] = source_mac[i];
                }
                sscanf(source_mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &eth->h_source[0], &eth->h_source[1], &eth->h_source[2], &eth->h_source[3], &eth->h_source[4], &eth->h_source[5]);
                // Send the response
                if (sendto(raw_socket, dns_response, curr_pos, 0, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
                {
                    perror("Send failed");
                    close(raw_socket);
                    exit(EXIT_FAILURE);
                }
                else
                {

#ifdef RESPONSE_CHECK
                    printf("\nResponse Packet: ");
                    for (int i = sizeof(struct ethhdr) + sizeof(struct iphdr); i < curr_pos; i++)
                    {
                        printf("%d ", dns_response[i]);
                    }
                    printf("\n");
#endif
#ifdef PACKET_CHECK
                    printf("\nPacket: ");
                    for (int i = 0; i < curr_pos; i++)
                    {
                        printf("%d ", dns_response[i]);
                    }
                    printf("\n");
#endif
#ifdef VERBOSE
                    printf("Response sent\n");
                    for (int ii = 0; ii < 40; ii++)
                        printf("-");
                    printf("\n");
#endif
                }
            }
        }
    }

    close(raw_socket);
    exit(EXIT_SUCCESS);
}
