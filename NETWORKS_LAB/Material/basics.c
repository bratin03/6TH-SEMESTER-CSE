// Headers needed
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Remember to include this header
#include <sys/types.h>  // Remember to include this header
#include <sys/socket.h> // Remember to include this header
#include <netinet/in.h> // Remember to include this header
#include <arpa/inet.h>  // Remember to include this header

// UDP Client

int main()
{
    int sockfd;
    
    //********************************
    struct sockaddr_in servaddr;
    socklen_t len;
    //********************************

    // Creating a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //********************************
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    int err;
    err = inet_aton("127.0.0.1", &servaddr.sin_addr);
    if (err == 0)
    {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }
    //********************************

    char *hello = "Hello from client";

    //****************************************************************************************************************
    sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    //****************************************************************************************************************

    //****************************************************************************************************************
    char buffer[1024];
    int n;
    n = recvfrom(sockfd, (char *)buffer, 1024, 0, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    //****************************************************************************************************************
    
    getchar();
    close(sockfd);
    exit(EXIT_SUCCESS);
}

// UDP Server

int main()
{
    int sockfd;
    
    //********************************
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    //********************************

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //********************************
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);
    //********************************

    //********************************
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    //********************************

    //****************************************************************************************************************
    char buffer[1024];
    int n;
    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *)buffer, 1024, 0, (struct sockaddr *)&cliaddr, &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    //****************************************************************************************************************

    char *hello = "Hello from server";

    //****************************************************************************************************************
    sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *)&cliaddr, len);
    //****************************************************************************************************************

    getchar();
    close(sockfd);
    exit(EXIT_SUCCESS);
}