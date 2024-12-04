#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#define TCP_PORT 8180
#define UDP_PORT 9190

#define BUFF 1024
#define INPUT "word.txt"

#define DEBUG

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void handle_request(int fd)
{
    FILE *fp;
    fp = fopen(INPUT, "r");
    char buffer[BUFF];
    while (fgets(buffer, BUFF, fp) != NULL)
    {
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = '\0';
        }
        send(fd, buffer, strlen(buffer) + 1, 0);
    }
    char c = '\0';
    send(fd, &c, 1, 0);
    close(fd);
    exit(0);
}

int main()
{
    int T, U;
    T = socket(AF_INET, SOCK_STREAM, 0);
    U = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servU, servT, cliU, cliT;
    memset(&servU, 0, sizeof(servU));
    memset(&servT, 0, sizeof(servT));
    memset(&cliU, 0, sizeof(cliU));
    memset(&cliT, 0, sizeof(cliT));
    servU.sin_addr.s_addr = INADDR_ANY;
    servU.sin_port = htons(UDP_PORT);
    servU.sin_family = AF_INET;
    servT.sin_addr.s_addr = INADDR_ANY;
    servT.sin_port = htons(TCP_PORT);
    servT.sin_family = AF_INET;

    bind(T, (const struct sockaddr *)&servT, sizeof(servT));
    bind(U, (const struct sockaddr *)&servU, sizeof(servU));

    listen(T, 5);

    while (1)
    {
        fd_set reafds;
        FD_ZERO(&reafds);
        FD_SET(T, &reafds);
        FD_SET(U, &reafds);
        int ret = select(max(T, U) + 1, &reafds, 0, 0, 0);
        if (ret < 0)
        {
            perror("select\n");
            exit(1);
        }
        if (FD_ISSET(U, &reafds))
        {
            char buffer[BUFF];
            int cli_len_U = sizeof(cliU);
            int data = recvfrom(U, buffer, BUFF, 0, (struct sockaddr *)&cliU, &cli_len_U);
            struct hostent *host_entry;
#ifdef DEBUG
            printf("Buffer: %s\n", buffer);
#endif
            host_entry = gethostbyname(buffer);
            if (host_entry == NULL)
            {
                buffer[0] = '\0';
                sendto(U, buffer, 1, 0, (const struct sockaddr *)&cliU, sizeof(cliU));
            }
            else
            {
                struct in_addr *tmp = (struct in_addr *)*host_entry->h_addr_list;
                char *ip = inet_ntoa(*tmp);
#ifdef DEBUG
                printf("IP: %s\n", ip);
#endif
                sendto(U, ip, strlen(ip) + 1, 0, (const struct sockaddr *)&cliU, sizeof(cliU));
            }
        }
        if (FD_ISSET(T, &reafds))
        {
            int cli_len_T = sizeof(cliT);
            int newfd = accept(T, (struct sockaddr *)&cliT, &cli_len_T);
            if (fork() == 0)
            {
                close(T);
                close(U);
                handle_request(newfd);
            }
            else
            {
                close(newfd);
            }
        }
    }
}
