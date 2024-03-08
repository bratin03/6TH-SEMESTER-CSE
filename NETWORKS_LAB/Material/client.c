#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
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
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    socklen_t len;

    strcpy(buffer, "Hello from client");
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    if (send(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM) < 0)
    {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    printf("Hello message sent.\n");
    for (int i = 0; i < MAXLINE; i++)
    {
        buffer[i] = '\0';
    }
    servaddr.sin_port = htons(8081);
    if (recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len) < 0)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}
