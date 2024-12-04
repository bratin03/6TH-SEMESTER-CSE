#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    // if (listen(sockfd, 5) == -1)
    // {
    //     perror("listen failed");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }

    printf("Server listening on port %d\n", PORT);
    int new_socket;
    struct sockaddr_in cli_addr;
    int cli_len = sizeof(cli_addr);
    if ((new_socket = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len)) == -1)
    {
        printf("error no: %d\n", errno);
        perror("accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return 0;
}
