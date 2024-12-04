// Bratin Mondal
// 21CS10016

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 30000
#define BUF 1024

// #define DEBUG

#define MAX_LEN 1000

int readint(int sockfd)
{
    int n;
    recv(sockfd, &n, sizeof(n), 0);
    int num = ntohl(n);
    return num;
}

void writeint(int sockfd, int n)
{
    int num = htonl(n);
    send(sockfd, &num, sizeof(num), 0);
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv.sin_addr);

    connect(fd, (const struct sockaddr *)&serv, sizeof(serv));

    int n = readint(fd);
    int count = 0;
    printf("Entity List:\n");
    while (1)
    {
        if (count == n)
        {
            break;
        }
        char buf[BUF];
        int pos = 0;
        while (1)
        {
            char c;
            recv(fd, &c, 1, 0);
            buf[pos++] = c;
            if (c == '\0')
            {
                count++;
                printf("Entity %d: %s\n", count, buf);
                break;
            }
        }
    }
    int is_willing;
    printf("\nDo you want to vote for any of the entity?\nEnter 0 to exit, 1 to vote:\n");
    scanf("%d", &is_willing);
    getchar();
    if (is_willing == 0)
    {
        close(fd);
        exit(0);
    }
    char name[BUF];
    printf("Enter the name of the entity you want to vote for: \n");
    fgets(name, BUF, stdin);
    if (name[strlen(name) - 1] == '\n')
    {
        name[strlen(name) - 1] = '\0';
    }
#ifdef DEBUG
    printf("Debug: Name:%s\n", name);
#endif
    send(fd, name, strlen(name) + 1, 0);
#ifdef DEBUG
    printf("Send: Name:%s\n", name);
#endif
    char buf[BUF];
    int pos = 0;
    while (1)
    {
        char c;
        recv(fd, &c, 1, 0);
        buf[pos++] = c;
        if (c == '\0')
        {
            printf("Server Response: %s\n", buf);
            close(fd);
            exit(0);
        }
    }
    close(fd);
    exit(0);
}