#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#define N 10
#define MAX_LEN 21
#define SERV_PORT 8181
#define MY_PORT 8185

int getinfo(int sockfd)
{
    int count = 0;
    printf("---------------------------------------------------\n");
    while (1)
    {
        int x;
        int ret = recv(sockfd, (void *)(&x), sizeof(int), 0);
        if (ret <= 0)
            return (count == 0) ? -2 : -1;
        int index = ntohl(x);
        char first_name[MAX_LEN];
        int idx = 0;
        while (1)
        {
            char c;
            ret = recv(sockfd, (void *)&c, 1, 0);
            if (ret <= 0)
            {
                return -1;
            }
            first_name[idx++] = c;
            if (c == '\0')
            {
                break;
            }
        }
        if (index == 0 && strcmp(first_name, "") == 0)
        {

            printf("---------------------------------------------------\n");
            return 0;
        }
        char last_name[MAX_LEN];
        idx = 0;
        while (1)
        {
            char c;
            ret = recv(sockfd, (void *)&c, 1, 0);
            if (ret <= 0)
            {
                return -1;
            }
            last_name[idx++] = c;
            if (c == '\0')
            {
                break;
            }
        }
        printf("ID: %d First Name: %s Last Name: %s\n", index, first_name, last_name);
    }
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in cli, serv;
    inet_aton("127.0.0.1", &serv.sin_addr);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERV_PORT);
    cli.sin_addr.s_addr = INADDR_ANY;
    cli.sin_family = AF_INET;
    cli.sin_port = htons(MY_PORT);
    bind(sockfd, (const struct sockaddr *)&cli, sizeof(cli));
    int ret = connect(sockfd, (const struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0)
    {
        perror("connect");
        exit(1);
    }
    ret = getinfo(sockfd);
    if (ret == -2)
    {
        printf("Could not vote: retry after some time\n");
        exit(1);
    }
    if (ret == -1)
    {
        printf("Error Occured!\n");
        exit(1);
    }
    printf("Enter your vote: ");
    int vote;
    scanf("%d", &vote);
    int x = htonl(vote);
    send(sockfd, (void *)&x, sizeof(x), 0);
    char buf[100];
    int idx = 0;
    while (1)
    {
        char c;
        ret = recv(sockfd, (void *)&c, 1, 0);
        if (ret <= 0)
        {
            printf("Error Occured!\n");
            exit(1);
        }
        buf[idx++] = c;
        if (c == '\0')
        {
            break;
        }
    }
    printf("%s\n", buf);
    close(sockfd);
    exit(0);
}