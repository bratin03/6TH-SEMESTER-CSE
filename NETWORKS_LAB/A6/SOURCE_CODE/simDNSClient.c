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
#include <signal.h>
#include <ctype.h>

#define INTERFACE "lo"
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEST_IP_ADDRESS "127.0.0.2"
#define SOURCE_MAC "ac:74:b1:ac:5c:1a"
#define DEST_MAC "ac:74:b1:ac:5c:1a"
#define WAIT_TIME 1
#define MAX_RETRY 3

// #define PARALLEL_IO_SUPPORT // Prevent Race Condition in I/O for terminal input and response output

// #define VERBOSE

// #define QUERY_CHECK
// #define PACKET_CHECK
// #define FIELD_CHECK
// #define RESPONSE_PACKET_CHECK
// #define DEBUG_RESPONSE
// #define DEBUG_INPUT
// #define DEBUG_TABLE
// #define DEBUG_RECV

#define MAX_QUERY_SIZE 1024
#define PROTOCOL_NO 254
#define MAX_QUERY_NO 8
#define MIN_DOMAIN_LEN 3
#define MAX_DOMAIN_LEN 31
#define TTL 64

#define LITTLE_ENDIAN_ORDER 0
#define BIG_ENDIAN_ORDER 1
#define PROTOCOL_ENDIANNESS LITTLE_ENDIAN_ORDER

#ifdef PARALLEL_IO_SUPPORT
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
void enable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON);
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}
#endif

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
 * Set the integer to the buffer in the specified order
 * @param buffer Buffer to store the integer
 * @param value Integer value
 * @param order Endianness of the integer
 * @return void
 */
void setIntToBuf(unsigned char *buffer, unsigned int value, int order)
{
    if (order == LITTLE_ENDIAN_ORDER)
    {
        buffer[0] = (value >> 24) & 0xFF;
        buffer[1] = (value >> 16) & 0xFF;
        buffer[2] = (value >> 8) & 0xFF;
        buffer[3] = value & 0xFF;
    }
    else
    {
        buffer[3] = (value >> 24) & 0xFF;
        buffer[2] = (value >> 16) & 0xFF;
        buffer[1] = (value >> 8) & 0xFF;
        buffer[0] = value & 0xFF;
    }
}

/**
 * Get the integer from the buffer in the specified order
 * @param buffer Buffer containing the integer
 * @param order Endianness of the integer
 * @return unsigned int Integer value
 */
void setShortToBuf(unsigned char *buffer, unsigned short value, int order)
{
    if (order == LITTLE_ENDIAN_ORDER)
    {
        buffer[0] = (value >> 8) & 0xFF;
        buffer[1] = value & 0xFF;
    }
    else
    {
        buffer[1] = (value >> 8) & 0xFF;
        buffer[0] = value & 0xFF;
    }
}

/**
 * Get the short integer from the buffer in the specified order
 * @param buffer Buffer containing the short integer
 * @param order Endianness of the short integer
 * @return unsigned short Short integer value
 */
unsigned short getShortFromBuf(unsigned char *buffer, int order)
{
    unsigned short value = 0;
    if (order == LITTLE_ENDIAN_ORDER)
    {
        value = buffer[0];
        value = (value << 8) | buffer[1];
    }
    else
    {
        value = buffer[1];
        value = (value << 8) | buffer[0];
    }
    return value;
}

/**
 * Structure to store the query table entry
 */
typedef struct query_table_entry
{
    unsigned short query_id;        /* Query ID */
    int retry_count;                /* Number of retries */
    char **domain_names;            /* Array of domain names */
    int num_queries;                /* Number of queries */
    struct query_table_entry *next; /* Pointer to the next query table entry */
} query_table_entry;

/**
 * Calculate the checksum of the buffer
 * @param ptr Pointer to the buffer
 * @param nbytes Number of bytes
 * @return unsigned short Checksum value
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
 * Check if the domain name is valid
 * @param buffer Buffer containing the domain name
 * @param domain_names Array of domain names
 * @param query_num Query number
 * @return int 1 if valid, 0 otherwise
 */
int isValidDomain(const unsigned char *buffer, unsigned char **domain_names, int query_num)
{
    int length = strlen((char *)buffer);
    if (length < MIN_DOMAIN_LEN || length > MAX_DOMAIN_LEN)
    {
        fprintf(stderr, ">> Error: Domain %s length must be between %d and %d\n", buffer, MIN_DOMAIN_LEN, MAX_DOMAIN_LEN);
        return 0;
    }

    int hyphenFlag = 0;
    for (int i = 0; i < length; i++)
    {
        if (!isalnum(buffer[i]) && buffer[i] != '-' && buffer[i] != '.')
        {
            fprintf(stderr, ">> Error: Domain %s contains invalid character %c\n", buffer, buffer[i]);
            return 0;
        }
        if (buffer[i] == '-')
        {
            if (i == 0 || i == length - 1 || hyphenFlag)
            {
                fprintf(stderr, ">> Error: Domain %s contains invalid hyphen\n", buffer);
                return 0;
            }
            hyphenFlag = 1;
        }
        else
        {
            hyphenFlag = 0;
        }
    }

    domain_names[query_num] = (unsigned char *)malloc((length + 1) * sizeof(unsigned char));
    strcpy((char *)domain_names[query_num], (char *)buffer);
    domain_names[query_num][length] = '\0';
    return 1;
}

/**
 * Process the query and create a query table entry
 * @param query Query string
 * @param query_id Query ID
 * @return query_table_entry* Pointer to the query table entry
 */
query_table_entry *process_query(unsigned char *query, unsigned short query_id)
{
    char *token = strtok((char *)query, " ");
    if (token == NULL)
    {
        return NULL;
    }
    if (strcmp(token, "getIP") != 0)
    {
        fprintf(stderr, ">> Error: Invalid query syntax\n");
        return NULL;
    }
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        fprintf(stderr, ">> Error: No query number provided\n");
        return NULL;
    }
    int query_num = atoi(token);

#ifdef DEBUG_INPUT
    printf("Number of domain names: %d\n", query_num);
#endif

    if (query_num <= 0)
    {
        fprintf(stderr, ">> Error! Invalid query number\n");
        return NULL;
    }

    if (query_num > MAX_QUERY_NO)
    {
        fprintf(stderr, ">> Error! Query number exceeds the maximum limit %d\n", MAX_QUERY_NO);
        return NULL;
    }

    unsigned char **domain_names = (unsigned char **)malloc(query_num * sizeof(unsigned char *));

    for (int i = 0; i < query_num; i++)
    {
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(domain_names[j]);
            }
            free(domain_names);
            fprintf(stderr, ">> Error! Insufficient number of domain names\n");
            return NULL;
        }
        if (!isValidDomain((unsigned char *)token, domain_names, i))
        {
            for (int j = 0; j < i; j++)
            {
                free(domain_names[j]);
            }
            free(domain_names);
            return NULL;
        }
    }
    if (strtok(NULL, " ") != NULL)
    {
        for (int i = 0; i < query_num; i++)
        {
            free(domain_names[i]);
        }
        free(domain_names);
        fprintf(stderr, ">> Error! Extra arguments provided\n");
        return NULL;
    }
#ifdef DEBUG_INPUT
    for (int i = 0; i < query_num; i++)
    {
        printf("Domain name %d: %s\n", i + 1, domain_names[i]);
    }
#endif
    query_table_entry *new_entry = (query_table_entry *)malloc(sizeof(query_table_entry));
    new_entry->query_id = query_id;
    new_entry->retry_count = 0;
    new_entry->domain_names = (char **)domain_names;
    new_entry->next = NULL;
    new_entry->num_queries = query_num;
    return new_entry;
}

/**
 * Get the new query ID
 * @return unsigned short New query ID
 */
unsigned short get_new_id()
{
    static unsigned short id = 0;
    id++;
    id = id % 65536;
    return id;
}

/**
 * Get the new query ID
 * @return unsigned short New query ID
 */
unsigned short get_query_id()
{
    static unsigned short query_id = 0;
    query_id++;
    query_id = query_id % 65536;
    return query_id;
}

/**
 * Fill the query packet
 * @param buff Buffer to store the query packet
 * @param query Query table entry
 * @return int Size of the query packet
 */
int fill_query(unsigned char *buff, query_table_entry *query)
{
    unsigned short query_id = query->query_id;
    // Set the query ID
    setShortToBuf(buff, query_id, PROTOCOL_ENDIANNESS);
    // Set the message type to Query
    buff[2] = getLSB32(0);
    buff[3] = getLSB32(query->num_queries);
    int cur_pos = 4;
    // Set the domain names
    for (int i = 0; i < query->num_queries; i++)
    {
        int domain_len = strlen(query->domain_names[i]);
        setIntToBuf((unsigned char *)(buff + cur_pos), domain_len, PROTOCOL_ENDIANNESS);
        cur_pos += 4;
        for (int j = 0; j < domain_len; j++)
        {
            buff[cur_pos++] = query->domain_names[i][j];
        }
    }
#ifdef QUERY_CHECK
    printf("Query Packet: ");
    for (int i = 0; i < cur_pos; i++)
    {
        printf("%d ", buff[i]);
    }
    printf("\n");
#endif
    return cur_pos;
}

void send_dns_query(query_table_entry *query, unsigned char *dest_mac, unsigned char *source_mac, int raw_socket, struct sockaddr_ll local_addr)
{
    unsigned char dns_query_packet[MAX_QUERY_SIZE];

    int query_size = fill_query((unsigned char *)dns_query_packet + sizeof(struct ethhdr) + sizeof(struct iphdr), query);

    // Fill the IP header
    struct iphdr *iph = (struct iphdr *)(dns_query_packet + sizeof(struct ethhdr));
    iph->ihl = getLSB32(5) & 0xF;
    iph->version = getLSB32(4) & 0xF;
    iph->tos = 0 & 0xFF;
    iph->id = htons(get_new_id());
    iph->ttl = getLSB32(TTL);
    iph->protocol = getLSB32(PROTOCOL_NO);
    iph->saddr = inet_addr(LOCAL_IP_ADDRESS);
    iph->daddr = inet_addr(DEST_IP_ADDRESS);
    iph->tot_len = htons(query_size + sizeof(struct iphdr));
    iph->frag_off = 0;
    iph->check = 0;
    iph->check = calculate_checksum((unsigned short *)iph, sizeof(struct iphdr));

    // Fill the Ethernet header
    struct ethhdr *eth = (struct ethhdr *)dns_query_packet;
    eth->h_proto = htons(ETH_P_IP);
    sscanf((char *)dest_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &eth->h_dest[0], &eth->h_dest[1], &eth->h_dest[2], &eth->h_dest[3], &eth->h_dest[4], &eth->h_dest[5]);
    sscanf((char *)source_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &eth->h_source[0], &eth->h_source[1], &eth->h_source[2], &eth->h_source[3], &eth->h_source[4], &eth->h_source[5]);

#ifdef FIELD_CHECK
    printf("\nIP Header:\n");
    printf("Version: %d\n", iph->version & 0x0F);
    printf("Header Length: %d\n", iph->ihl & 0x0F);
    printf("TOS: %d\n", iph->tos & 0xFF);
    printf("Total Length: %d\n", ntohs(iph->tot_len));
    printf("Identification: %d\n", ntohs(iph->id));
    printf("TTL: %d\n", iph->ttl & 0xFF);
    printf("Protocol: %d\n", iph->protocol & 0xFF);
    printf("Source IP: %s\n", LOCAL_IP_ADDRESS);
    printf("Destination IP: %s\n", DEST_IP_ADDRESS);
    printf("Checksum: %d\n", iph->check);

    printf("\nEthernet Header:\n");
    printf("Source MAC: %s\n", source_mac);
    printf("Destination MAC: %s\n", dest_mac);
    printf("Protocol: %d\n", ntohs(eth->h_proto));
#endif

    // Send the packet
    if (sendto(raw_socket, dns_query_packet, query_size + sizeof(struct ethhdr) + sizeof(struct iphdr), 0, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("Sendto failed");
        exit(EXIT_FAILURE);
    }

#ifdef PACKET_CHECK
    printf("\nPacket: ");
    for (int i = 0; i < (int)(query_size + sizeof(struct ethhdr) + sizeof(struct iphdr)); i++)
    {
        printf("%d ", dns_query_packet[i]);
    }
    printf("\n");
#endif
    query->retry_count++;
}

/**
 * Find the pending query in the query table
 * @param query_id Query ID
 * @param QT Query Table
 * @return query_table_entry* Pointer to the query table entry
 */
query_table_entry *find_pending_query(unsigned short query_id, query_table_entry *QT)
{
    query_table_entry *temp = QT;

    if (temp == NULL)
    {
#ifdef DEBUG_TABLE
        printf("Query Table is empty\n");
#endif
        return NULL;
    }

#ifdef DEBUG_TABLE
    printf("Finding Query in Query Table\n");
#endif

    while (temp != NULL)
    {
#ifdef DEBUG_TABLE
        printf("Query ID: %d\n", temp->query_id);
#endif
        if (temp->query_id == query_id)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * Validate the MAC address format
 * @param mac MAC address
 * @return int 1 if valid, 0 otherwise
 */
int validateMacAddrFormat(unsigned char *mac)
{
    int len = strlen((char *)mac);
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
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return int Exit status
 */
int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigint_handler);
    int raw_socket;
    struct sockaddr_ll local_addr;
    unsigned char *dest_mac = (unsigned char *)malloc(20 * sizeof(unsigned char));
    unsigned char *source_mac = (unsigned char *)malloc(20 * sizeof(unsigned char));
    if (argc == 3)
    {
        strcpy((char *)source_mac, argv[1]);
        strcpy((char *)dest_mac, argv[2]);

        if (!validateMacAddrFormat(dest_mac) || !validateMacAddrFormat(source_mac))
        {
            fprintf(stderr, "Error: Invalid MAC Address Format\n");
            fprintf(stderr, "Please provide the MAC address in the format XX:XX:XX:XX:XX:XX\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 1)
    {
#ifdef VERBOSE
        printf("Using default MAC Address for source and destination\n");
        printf("Source MAC: %s\n", SOURCE_MAC);
        printf("Destination MAC: %s\n", DEST_MAC);
#endif
        strcpy((char *)dest_mac, DEST_MAC);
        strcpy((char *)source_mac, SOURCE_MAC);
    }
    else
    {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        fprintf(stderr, "For Command Line Arguments:\n");
        fprintf(stderr, "Usage: %s <Source MAC Address> <Destination MAC Address>\n", argv[0]);
        fprintf(stderr, "For Default MAC Address:\n");
        fprintf(stderr, "Usage: %s\n", argv[0]);
        fprintf(stderr, "Please provide the MAC address in the format XX:XX:XX:XX:XX:XX\n");
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
    sscanf((char *)source_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &local_addr.sll_addr[0], &local_addr.sll_addr[1], &local_addr.sll_addr[2], &local_addr.sll_addr[3], &local_addr.sll_addr[4], &local_addr.sll_addr[5]);

    query_table_entry *QT = NULL;
    fd_set mainfds;
    FD_ZERO(&mainfds);
    FD_SET(raw_socket, &mainfds);
    FD_SET(STDIN_FILENO, &mainfds);
    int timeout_reset_flag = 1;
    struct timeval timeout;

#ifdef PARALLEL_IO_SUPPORT
    int is_Response_Output_Pending = 0;
#endif

    // Give a prompt
    printf("$ ");
    fflush(stdout);

    while (1)
    {
        fd_set readfds;
        readfds = mainfds;
        // Set the timeout if the flag is set
        if (timeout_reset_flag)
        {
            timeout.tv_sec = 0;
            timeout.tv_usec = WAIT_TIME * 1000000;
        }
        int ret = select(raw_socket + 1, &readfds, NULL, NULL, &timeout);
        if (ret < 0)
        {
            perror("Select failed");
            close(raw_socket);
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            // If timeout occurs, resend the queries
            query_table_entry *temp = QT;
            while (temp != NULL)
            {
                // Check if the query has been retried more than the maximum number of times
                if (temp->retry_count <= MAX_RETRY)
                {
                    // Resend the query
                    send_dns_query(temp, dest_mac, source_mac, raw_socket, local_addr);
                }
                else
                {
#ifdef PARALLEL_IO_SUPPORT
                    enable_raw_mode();
                    int is_Terminal_Input_Pending = kbhit();
                    if (is_Terminal_Input_Pending)
                    {
                        disable_raw_mode();
                        is_Response_Output_Pending = 1;
                        goto terminal_input;
                    }
                    else
                    {
                        disable_raw_mode();
                    }
                Query_Error:
#endif

                    // Print the error message and remove the query from the query table
                    printf("\n");
                    for (int ii = 0; ii < 40; ii++)
                    {
                        printf("-");
                    }
                    printf("\nError processing query with ID: %d even after %d retries\n", temp->query_id, MAX_RETRY);
                    printf("Query Strings: ");
                    for (int i = 0; i < temp->num_queries; i++)
                    {
                        printf("%s ", temp->domain_names[i]);
                    }
                    printf("\n");
                    for (int ii = 0; ii < 40; ii++)
                    {
                        printf("-");
                    }
                    printf("\n");
                    printf("$ ");
                    fflush(stdout);
                    query_table_entry *temp1 = QT;
                    query_table_entry *prev = NULL;
                    while (temp1 != NULL)
                    {
                        if (temp1->query_id == temp->query_id)
                        {
                            if (prev == NULL)
                            {
                                QT = temp1->next;
                            }
                            else
                            {
                                prev->next = temp1->next;
                            }
                            free(temp1);
                            break;
                        }
                        prev = temp1;
                        temp1 = temp1->next;
                    }
                }
                temp = temp->next;
            }
            timeout_reset_flag = 1;
            continue;
        }
        else
        {
            // Check if the raw socket has data
            if (FD_ISSET(raw_socket, &readfds))
            {
                unsigned char buffer[MAX_QUERY_SIZE];
                int data_size = recv(raw_socket, buffer, sizeof(buffer), 0);
                buffer[data_size] = '\0';
                if (data_size < 0)
                {
                    perror("Recv failed");
                    close(raw_socket);
                    free(QT);
                    exit(EXIT_FAILURE);
                }

                struct ethhdr *eth = (struct ethhdr *)buffer;
                unsigned short eth_protocol = ntohs(eth->h_proto);

                // Check if the packet is an IP packet
                if (eth_protocol != ETH_P_IP)
                {
#ifdef DEBUG_RECV
                    fprintf(stderr, "Ethernet Protocol: %d\n", eth_protocol);
#endif
                    timeout_reset_flag = 0;
                    continue;
                }

                struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

                char source_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(ip->saddr), source_ip, INET_ADDRSTRLEN);
                char dest_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(ip->daddr), dest_ip, INET_ADDRSTRLEN);
                unsigned int ip_protocol = getIntFromLSB32(ip->protocol);
                // Check if the packet is for this client
                if (ip_protocol != PROTOCOL_NO || strcmp(dest_ip, LOCAL_IP_ADDRESS) != 0)
                {
#ifdef DEBUG_RECV
                    printf("Packet not for this client. Protocol: %d\n", ip_protocol);
#endif
                    timeout_reset_flag = 0;
                    continue;
                }
                else
                {
#ifdef DEBUG_RESPONSE
                    printf("Packet received\n");
#endif
                    // Check the checksum of the packet
                    if (calculate_checksum((unsigned short *)(buffer + sizeof(struct ethhdr)), 4 * (ip->ihl)) == 0)
                    {
#ifdef DEBUG_RESPONSE
                        printf("Checksum verified\n");
#endif
                        // Check if the packet is a response
                        unsigned char *data = buffer + sizeof(struct ethhdr) + sizeof(struct iphdr);
                        unsigned short query_id = getShortFromBuf(data, PROTOCOL_ENDIANNESS);
                        data += 2;
                        char message_type = *data;
                        data += 1;
                        if (message_type != 1)
                        {
#ifdef DEBUG_RESPONSE
                            fprintf(stderr, "Message is not response\n");
#endif
                            continue;
                        }
                        char num_response = *data;
                        unsigned int num_response_int = getIntFromLSB32(num_response);
                        data += 1;
                        // Find the query in the query table
                        query_table_entry *query_response = find_pending_query(query_id, QT);
                        // Check if the query is found
                        if (query_response == NULL)
                        {
#ifdef DEBUG_RESPONSE
                            fprintf(stderr, "Query not found in the query table\n");
#endif
                            continue;
                        }
#ifdef RESPONSE_PACKET_CHECK
                        printf("\nResponse Packet: ");
                        for (int i = 0; i < data_size; i++)
                        {
                            printf("%d ", buffer[i]);
                        }
                        printf("\n");
#endif
#ifdef PARALLEL_IO_SUPPORT
                        enable_raw_mode();
                        int is_Terminal_Input_Pending = kbhit();
                        if (is_Terminal_Input_Pending)
                        {
                            disable_raw_mode();
                            is_Response_Output_Pending = 2;
                            goto terminal_input;
                        }
                        else
                        {
                            disable_raw_mode();
                        }
                    Query_Response:
#endif

                        // Print the response
                        printf("\n");
                        for (int ii = 0; ii < 40; ii++)
                        {
                            printf("-");
                        }
                        printf("\n");
                        printf("Query ID: %d\n", query_id);
                        printf("Total Query Strings: %d\n", query_response->num_queries);
                        for (unsigned int j = 0; j < num_response_int; j++)
                        {
                            if (data[0] == 0)
                            {
                                printf("%s NO IP ADDRESS FOUND\n", query_response->domain_names[j]);
                                data += 5;
                            }
                            else
                            {
                                data += 1;
                                int ip1;
                                ip1 = getIntFromLSB32(*data);
                                data += 1;
                                int ip2;
                                ip2 = getIntFromLSB32(*data);
                                data += 1;
                                int ip3;
                                ip3 = getIntFromLSB32(*data);
                                data += 1;
                                int ip4;
                                ip4 = getIntFromLSB32(*data);
                                data += 1;
                                printf("%s %d.%d.%d.%d\n", query_response->domain_names[j], ip1, ip2, ip3, ip4);
                            }
                        }
                        for (int ii = 0; ii < 40; ii++)
                        {
                            printf("-");
                        }
                        printf("\n");
                        printf("$ ");
                        fflush(stdout);
                        query_table_entry *temp1 = QT;
                        query_table_entry *prev = NULL;
                        while (temp1 != NULL)
                        {
                            if (temp1->query_id == query_id)
                            {
                                if (prev == NULL)
                                {
                                    QT = temp1->next;
                                }
                                else
                                {
                                    prev->next = temp1->next;
                                }
                                free(temp1);
                                break;
                            }
                            prev = temp1;
                            temp1 = temp1->next;
                        }
                    }
                }
                timeout_reset_flag = 1;
            }
            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
#ifdef PARALLEL_IO_SUPPORT
            terminal_input:;
#endif
                // Read the query from the user
                char query[MAX_QUERY_SIZE];
                fgets(query, MAX_QUERY_SIZE, stdin);
                if (strcmp(query, "EXIT\n") == 0)
                {
                    break;
                }
                query[strlen(query) - 1] = '\0';
#ifdef DEBUG_INPUT
                printf("Query: %s\n", query);
#endif
                // Process the query
                query_table_entry *new_query = process_query((unsigned char *)query, get_query_id());
                if (new_query == NULL)
                {
                    // Print the prompt
                    printf("$ ");
                    fflush(stdout);
                    continue;
                }
                // Add the query to the query table
                if (QT == NULL)
                {
                    QT = new_query;
                }
                else
                {
                    query_table_entry *temp = QT;
                    while (temp->next != NULL)
                    {
                        temp = temp->next;
                    }
                    temp->next = new_query;
                }
                send_dns_query(new_query, dest_mac, source_mac, raw_socket, local_addr);
                timeout_reset_flag = 0;
                printf("$ ");
                fflush(stdout);
#ifdef PARALLEL_IO_SUPPORT
                if (is_Response_Output_Pending == 1)
                {
                    is_Response_Output_Pending = 0;
                    goto Query_Error;
                }
                else if (is_Response_Output_Pending == 2)
                {
                    is_Response_Output_Pending = 0;
                    goto Query_Response;
                }
#endif
            }
        }
    }
    // Free the memory and close the socket
    free(QT);
    close(raw_socket);
    exit(EXIT_SUCCESS);
}
