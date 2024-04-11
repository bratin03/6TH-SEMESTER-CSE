#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFF 1024

int receiveUntilNull(int fd, char *buf)
{
    int i = 0;
    while (1)
    {
        char c;
        int ret = recv(fd, &c, 1, 0);
        if (ret == 0)
            return 0;
        buf[i++] = c;
        if (c == '\0')
            return i;
    }
}

void writeInt(int sockfd, int n)
{
    int x = ntohl(n);
    send(sockfd, &x, sizeof(x), 0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <My Port No> <Load Balancer Port No>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);
    int lb_port = atoi(argv[2]);
    if (port == 0 || lb_port == 0)
    {
        fprintf(stderr, "Invalid Port Number\n");
        exit(1);
    }
    srand(getpid());
    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv, load_b;
    memset(&serv, 0, sizeof(serv));
    memset(&load_b, 0, sizeof(load_b));
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_family = AF_INET;
    bind(fd1, (const struct sockaddr *)&serv, sizeof(serv));
    load_b.sin_port = htons(lb_port);
    inet_aton("127.0.0.1", &load_b.sin_addr);
    load_b.sin_family = AF_INET;
    connect(fd1, (const struct sockaddr *)&load_b, sizeof(load_b));
    while (1)
    {
        char buff[BUFF];
        int ret = receiveUntilNull(fd1, buff);
        if (ret == 0)
        {
            fprintf(stderr, "Load Balancer Closed Connection\n");
            exit(1);
        }
        if (strcmp(buff, "Send Load") == 0)
        {
            int load = 1 + rand() % 100;
            writeInt(fd1, load);
            printf("Load sent: %d\n", load);
        }
        else if (strcmp(buff, "Send Time") == 0)
        {
            
        }
    }
}