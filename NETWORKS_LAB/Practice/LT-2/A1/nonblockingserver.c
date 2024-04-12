#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

// #define DEBUG

#define PORT_T 8180
#define PORT_U 9190

#define BUFF 1024

#define INPUT "word.txt"

void child(int fd)
{
    char buffer[BUFF];
    FILE *fp = fopen(INPUT, "r");
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
    int flags = fcntl(T, F_GETFL);
    fcntl(T, F_SETFL, flags | O_NONBLOCK);
    struct sockaddr_in servT, servU, cliT, cliU;
    memset(&servT, 0, sizeof(servT));
    memset(&servU, 0, sizeof(servU));
    memset(&cliT, 0, sizeof(cliT));
    memset(&cliU, 0, sizeof(cliU));

    servT.sin_addr.s_addr = INADDR_ANY;
    servT.sin_port = htons(PORT_T);
    servT.sin_family = AF_INET;

    servU.sin_addr.s_addr = INADDR_ANY;
    servU.sin_port = htons(PORT_U);
    servU.sin_family = AF_INET;

    bind(T, (const struct sockaddr *)(&servT), sizeof(servT));
    bind(U, (const struct sockaddr *)(&servU), sizeof(servU));

    listen(T, 5);

    while (1)
    {
        while (1)
        {
            int cli_len_T = sizeof(cliT);
            int newfd = accept(T, (struct sockaddr *)&cliT, &cli_len_T);
            if (newfd == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else
                {
                    perror("accept");
                    exit(1);
                }
            }
            else
            {
                if (fork() == 0)
                {
                    close(T);
                    close(U);
                    child(newfd);
                }
                else
                {
                    close(newfd);
                }
            }
        }
        while (1)
        {
            int cli_len_U = sizeof(cliU);
            char buffer[BUFF];
            int recv_ret = recvfrom(U, buffer, BUFF, MSG_DONTWAIT, (struct sockaddr *)&cliU, &cli_len_U);
            if (recv_ret == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else
                {
                    perror("recvfrom");
                    exit(1);
                }
            }
#ifdef DEBUG
            printf("Buffer: %s\n", buffer);
#endif
            struct hostent *entry;
            entry = gethostbyname(buffer);
            if (entry == NULL)
            {
                char c = '\0';
                sendto(U, &c, 1, 0, (const struct sockaddr *)&cliU, sizeof(cliU));
            }
            else
            {
                struct in_addr **temp = (struct in_addr **)entry->h_addr_list;
                char *ip = inet_ntoa(*temp[0]);
                sendto(U, ip, strlen(ip) + 1, 0, (const struct sockaddr *)&cliU, sizeof(cliU));
            }
        }
    }
}