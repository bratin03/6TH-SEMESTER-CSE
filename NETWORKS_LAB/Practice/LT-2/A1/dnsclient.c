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

#define TCP_PORT 8181
#define UDP_PORT 9191
#define BUFF 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Host Name>\n", argv[0]);
        exit(1);
    }
    int U;
    U = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servU;
    memset(&servU, 0, sizeof(servU));
    inet_aton("127.0.0.1", &servU.sin_addr);
    servU.sin_port = htons(UDP_PORT);
    servU.sin_family = AF_INET;
    sendto(U, argv[1], strlen(argv[1]) + 1, 0, (const struct sockaddr *)&servU, sizeof(servU));
    char buf[BUFF];
    recv(U, buf, BUFF, 0);
    if (buf[0] == '\0')
    {
        printf("No IP Found for %s\n", argv[1]);
    }
    else
    {
        printf("IP for %s : %s\n", argv[1], buf);
    }
    exit(0);
}