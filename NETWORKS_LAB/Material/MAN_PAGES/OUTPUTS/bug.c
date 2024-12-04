#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8181
#define MAXLINE 1024

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE] = "Hello, Server!";
    int n;

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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Attempt to send message without connecting
    if ((n = send(sockfd, buffer, strlen(buffer), 0)) == -1)
    {
        if (errno == ENOTCONN)
        {
            perror("send failed - Socket is not connected");
        }
        else
        {
            perror("send failed");
        }
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Message sent.\n");

    // Close socket
    close(sockfd);

    return 0;
}