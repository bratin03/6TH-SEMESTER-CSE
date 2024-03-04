#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // Include this header for fcntl()
#include <errno.h>

#define PORT 8180
#define MAXLINE 1024

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE];

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect the client socket to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, "Hello, Server!");
    send(sockfd, buffer, strlen(buffer), 0);
    // Do something else, or sleep for some time to allow non-blocking connect to complete

    return 0;
}
