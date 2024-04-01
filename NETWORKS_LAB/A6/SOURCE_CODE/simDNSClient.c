/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
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
#include <ctype.h>

#define LOCAL_IP_ADDRESS "192.168.43.203"
#define MAX_QUERY_SIZE 1024
#define PROTOCOL_NO 254
#define MAX_QUERY_NO 8
#define MIN_DOMAIN_LEN 3
#define MAX_DOMAIN_LEN 31

#define DEBUG
#define VERBOSE

typedef struct query_table_entry
{
    int query_id;
    char **domain_names;
    struct query_table_entry *next;
} query_table_entry;

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

int isValidDomain(const char *buffer, char **domain_names, int query_num)
{
    int length = strlen(buffer);
    if (length < MIN_DOMAIN_LEN || length > MAX_DOMAIN_LEN)
    {
        fprintf(stderr, "Error: Domain length must be between %d and %d characters.\n", MIN_DOMAIN_LEN, MAX_DOMAIN_LEN);
        return 0;
    }

    int hyphenFlag = 0;
    for (int i = 0; i < length; i++)
    {
        if (!isalnum(buffer[i]) && buffer[i] != '-' && buffer[i] != '.')
        {
            fprintf(stderr, "Error: Domain contains invalid character '%c'.\n", buffer[i]);
            return 0;
        }
        if (buffer[i] == '-')
        {
            if (i == 0 || i == length - 1 || hyphenFlag || buffer[i - 1] == '.')
            {
                fprintf(stderr, "Error: Invalid hyphen usage in domain.\n");
                return 0;
            }
            hyphenFlag = 1;
        }
        else
        {
            hyphenFlag = 0;
        }
    }
    if (buffer[length - 1] == '-')
    {
        fprintf(stderr, "Error: Domain cannot end with a hyphen.\n");
        return 0;
    }

    domain_names[query_num] = (char *)malloc((length + 1) * sizeof(char));
    strcpy(domain_names[query_num], buffer);
    domain_names[query_num][length] = '\0';
    return 1;
}

void process_query(char *query, query_table_entry *QT, int *query_id)
{
    char *token = strtok(query, " ");
    if (strcmp(token, "getIP") != 0)
    {
        fprintf(stderr, "Error: Invalid query syntax\n");
        return;
    }

    token = strtok(NULL, " ");
    int query_num = atoi(token);

#ifdef DEBUG
    printf("Number of domain names: %d\n", query_num);
#endif

    if (query_num <= 0)
    {
        fprintf(stderr, "Error! Invalid query number\n");
        return;
    }

    if (query_num > MAX_QUERY_NO)
    {
        fprintf(stderr, "Error! Query number exceeds the maximum limit\n");
        return;
    }

    char **domain_names = (char **)malloc(query_num * sizeof(char *));

    for (int i = 0; i < query_num; i++)
    {
        // getIP N <domain-1> <domain-2> <domain-3> … <domain-N>
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            fprintf(stderr, "Error! Insufficient number of domain names\n");
            return;
        }
        if (!isValidDomain(token, domain_names, i))
        {
            return;
        }
    }

#ifdef DEBUG
    for (int i = 0; i < query_num; i++)
    {
        printf("Domain name %d: %s\n", i + 1, domain_names[i]);
    }
#endif
}

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

    int query_id = 0;
    query_table_entry *QT = NULL;

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(raw_socket, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int ret = select(raw_socket + 1, &readfds, NULL, NULL, &timeout);
        if (ret < 0)
        {
            perror("Select failed");
            close(raw_socket);
            exit(1);
        }
        else if (ret == 0)
        {

#ifdef VERBOSE
            printf("Timeout occurred\n");
#endif
            continue;
        }
        else
        {
            if (FD_ISSET(raw_socket, &readfds))
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
                if (protocol != PROTOCOL_NO)
                {
                    continue;
                }
            }
            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                char query[MAX_QUERY_SIZE];
                fgets(query, MAX_QUERY_SIZE, stdin);
                query[strlen(query) - 1] = '\0';
                process_query(query, QT, &query_id);
            }
        }
    }
}
