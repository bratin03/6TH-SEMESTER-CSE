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

int main()
{
    int T;
    T = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servT;
    memset(&servT, 0, sizeof(servT));
    inet_aton("127.0.0.1", &servT.sin_addr);
    servT.sin_port = htons(TCP_PORT);
    servT.sin_family = AF_INET;

    connect(T, (const struct sockaddr *)&servT, sizeof(servT));

    int count = 0;
    while (1)
    {
        char buf[BUFF];
        int i = 0;
        while (1)
        {
            char c;
            recv(T, &c, 1, 0);
            if (i == 0 && c == '\0')
            {
                printf("Total Words: %d\n", count);
                exit(0);
            }
            buf[i++] = c;
            if (c == '\0')
            {
                printf("%d Word: %s\n", ++count, buf);
                break;
            }
        }
    }
}