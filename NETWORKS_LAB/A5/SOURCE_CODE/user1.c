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
    src_addr.sin_port = htons(8000);
    src_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8001);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = m_bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("m_bind");
        exit(1);
    }

    char recv_buf[MSG_SIZE];
    while (1)
    {
        ret = m_recvfrom(fd, recv_buf, MSG_SIZE, 0, (struct sockaddr *)&dest_addr, (socklen_t *)&dest_addr);
        if (ret == -1)
        {
            sleep(5);
            continue;
            // break;
        }
        else
        {
            printf("Received: %s\n", recv_buf);
            if(strcmp(recv_buf, "TheEnd") == 0)
                exit(0);
        }
    }

    while (1)
        ;
}