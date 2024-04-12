#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <limits.h>

#define PORT 8181
// #define DEBUG
#define BUFF 1024

void writeInt(int fd, int n)
{
    n = htonl(n);
    send(fd, &n, sizeof(n), 0);
}

int readint(int fd)
{
    int n;
    int ret = recv(fd, &n, sizeof(n), 0);
    if (ret == 0)
    {
        return -1;
    }
    return ntohl(n);
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    inet_aton("127.0.0.1", &serv.sin_addr);
    serv.sin_port = htons(PORT);
    serv.sin_family = AF_INET;

    connect(fd, (const struct sockaddr *)&serv, sizeof(serv));

    int n;
    char buff[BUFF];

    while (1)
    {
        n = readint(fd);
        if (n == -1)
        {
            fprintf(stderr, "Could Not vote: retry after some time\n");
            exit(1);
        }
        if (n == 0)
        {
            char c;
            int ret = recv(fd, &c, 1, 0);
            if (ret == 0)
            {
                fprintf(stderr, "Could Not vote: retry after some time\n");
                exit(1);
            }
            if (c == '\0')
            {
                break;
            }
        }
        printf("%d ", n);
        int i = 0;
        while (1)
        {
            char c;
            int ret = recv(fd, &c, 1, 0);
            if (ret == 0)
            {
                fprintf(stderr, "Could Not vote: retry after some time\n");
                exit(1);
            }
            buff[i++] = c;
            if (c == '\0')
            {
                printf("%s\n", buff);
                break;
            }
        }
    }
    int vote;
    printf("Enter Your Vote: ");
    scanf("%d", &vote);
    writeInt(fd, vote);
    int i = 0;
    while (1)
    {
        char c;
        int ret = recv(fd, &c, 1, 0);
        if (ret == 0)
        {
            fprintf(stderr, "Could Not vote: retry after some time\n");
            exit(1);
        }
#ifdef DEBUG
        printf("%d %d %c\n", ret, i, c);
#endif
        buff[i++] = c;
        if (c == '\0')
        {
            printf("%s\n", buff);
            close(fd);
            break;
        }
    }
}