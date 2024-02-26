#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8181
#define MAXLINE 1024

int main()
{
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    socklen_t clilen;
    char buffer[MAXLINE];

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize socket structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) == -1)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    clilen = sizeof(cli_addr);

    // Accept connections
    if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive message from client
    n = recv(newsockfd, buffer, MAXLINE, MSG_WAITALL);
    if (n == -1)
    {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    printf("Client: %s\n", buffer);

    // Close sockets
    close(newsockfd);
    close(sockfd);

    return 0;
}
