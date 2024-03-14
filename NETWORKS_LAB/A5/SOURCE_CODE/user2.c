#include "msocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERBOSE

int main()
{
#ifdef VERBOSE
    printf("My pid: %d\n", getpid());
#endif
    int domain = AF_INET;
    int type = SOCK_MTP;

    int protocol = 0;
    int fd = m_socket(domain, type, protocol);
    if (fd == -1)
    {
        perror("m_socket");
        exit(1);
    }
    printf("Socket created successfully\n");
    struct sockaddr_in src_addr, dest_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(8001);
    src_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8000);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = m_bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("m_bind");
        exit(1);
    }
    char buf[MSG_SIZE];
    int count = 0;
    while (1)
    {

        sprintf(buf, "Line%02d", count);
        // printf("Sending: %s\n", buf);
        ret = m_sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret == -1)
        {
            sleep(1);
            continue;
        }
        printf("%d accepted\n", count);
        count++;
        if (count > 100)
            break;
    }
    while (1)
    {
        sprintf(buf, "TheEnd");
        // printf("Sending: %s\n", buf);
        ret = m_sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret == -1)
        {
            sleep(1);
            continue;
        }
        break;
    }
    while (1)
        ;
}