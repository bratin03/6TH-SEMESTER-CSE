#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8181
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE] = "Hello, Server!";
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        printf("Error Number %d\n", errno); // Print the error number
        perror("send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}
