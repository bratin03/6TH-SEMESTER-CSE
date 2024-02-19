#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
    int sockfd, newsockfd;
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;

    int i;
    char buf[100];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    // int flags = fcntl(sockfd, F_GETFL, 0);
    // if (flags == -1)
    // {
    //     perror("fcntl");
    //     exit(EXIT_FAILURE);
    // }

    // if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    // {
    //     perror("fcntl");
    //     exit(EXIT_FAILURE);
    // }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8180);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    // if (listen(sockfd, 5) == -1)
    // {
    //     int error = errno;
    //     printf("Error code: %d\n", error);
    //     perror("Error listening on socket");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }

    while (1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd == -1)
        {
            perror("Accept error\n");
            exit(0);
        }
        strcpy(buf, "Message from server");
        send(newsockfd, buf, strlen(buf) + 1, 0);
        recv(newsockfd, buf, 100, 0);
        printf("%s\n", buf);

        close(newsockfd);
    }
    return 0;
}