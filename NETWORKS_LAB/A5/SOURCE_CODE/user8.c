/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
                    Name: Somya Kumar
                    Student ID: 21CS30050
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/
/*
User 2 receives messages from user1 and writes to a file
*/

#include "msocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define VERBOSE
#define WAIT_TIME 1
#define OUTPUT_FILE "output4.txt"
int main()
{

    int domain = AF_INET;
    int type = SOCK_MTP;

    int protocol = 0;
    int fd = m_socket(domain, type, protocol);
    if (fd == -1)
    {
        perror("m_socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");
    struct sockaddr_in src_addr, dest_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(11080);
    src_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(11081);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = m_bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("m_bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful from IP addr %s and port %d to IP addr %s and port %d\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));

    char recv_buf[MSG_SIZE];
    FILE *fp = fopen(OUTPUT_FILE, "w");
    while (1)
    {
        ret = m_recvfrom(fd, recv_buf, MSG_SIZE, 0, (struct sockaddr *)&dest_addr, (socklen_t *)&dest_addr);
        if (ret == -1)
        {
            if (errno == ENOMSG)
            {
#ifdef VERBOSE
                printf("No data received. Sleeping for %d seconds\n", WAIT_TIME);
#endif
                sleep(WAIT_TIME);
                continue;
            }
            else
            {
                perror("m_recvfrom");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            char print_buf[MSG_SIZE + 1];
            for (int i = 0; i < MSG_SIZE; i++)
            {
                print_buf[i] = recv_buf[i];
            }
            print_buf[MSG_SIZE] = '\0';
            printf("Received: %s\n", print_buf);
            int isend = 1;
            for (int i = 0; i < MSG_SIZE; i++)
            {
                if (recv_buf[i] != '$')
                {
                    isend = 0;
                    break;
                }
            }
            if (isend)
            {
#ifdef VERBOSE
                printf("End of file\n");
#endif
                fclose(fp);
                break;
            }
            if (fprintf(fp, "%s", print_buf) < 0)
            {
                perror("fprintf");
                exit(EXIT_FAILURE);
            }
        }
    }
    while (1)
    {
    }
    exit(EXIT_FAILURE);
}