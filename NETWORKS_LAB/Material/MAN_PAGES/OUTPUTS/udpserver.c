#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define MAXLINE 1024

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int n;
    socklen_t len;
    char buffer[MAXLINE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }



    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(81801;

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("\nServer Running....\n");
    if (listen(sockfd, 5) == -1)
    {
        int error = errno;
        printf("Error code: %d\n", error);
        perror("Error listening on socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);
    n = recv(sockfd, (char *)buffer, MAXLINE, 0);
    buffer[n] = '\0';
    printf("%s\n", buffer);
    strcpy(buffer, "Message from server");
    send(sockfd, buffer, strlen(buffer) + 1, 0);

    close(sockfd);
    return 0;
}