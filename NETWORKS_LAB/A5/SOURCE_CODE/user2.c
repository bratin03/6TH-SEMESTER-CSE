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
User 2 sends messages to User 1. It sends 25 messages and then sends "TheEnd" message to indicate the end of the communication. 
*/

#include "msocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define VERBOSE
#define WAIT_TIME 3

int generate_message_of_length(char *buf, int len, int seq_num)
{
    sprintf(buf, "%2d", seq_num);
    for (int i = 2; i < len - 2; i++)
    {
        buf[i] = '-';
    }
    buf[len - 2] = '\n';
    buf[len - 1] = '\0';
    return 0;
}

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
    src_addr.sin_port = htons(8001);
    src_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8000);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = m_bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("m_bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful from IP addr %s and port %d to IP addr %s and port %d\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));
    char buf[MSG_SIZE];
    int count = 1;
    while (1)
    {
        generate_message_of_length(buf, MSG_SIZE, count);
        ret = m_sendto(fd, buf, MSG_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret == -1)
        {
            if (errno == ENOBUFS)
            {
#ifdef VERBOSE
                printf("Buffer is currently Full. Sleeping for %d seconds\n", WAIT_TIME);
#endif
                sleep(WAIT_TIME);
                continue;
            }
            else
            {
                perror("m_sendto");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("Sent: %s\n", buf);
            count++;
        }
        if (count > 25)
        {
            break;
        }
    }
    char end[] = "TheEnd";
    while (1)
    {
        ret = m_sendto(fd, end, MSG_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret == -1)
        {
            if (errno == ENOBUFS)
            {
#ifdef VERBOSE
                printf("Buffer is currently Full. Sleeping for %d seconds\n", WAIT_TIME);
#endif
                sleep(WAIT_TIME);
                continue;
            }
            else
            {
                perror("m_sendto");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("Sent: %s\n", end);
            break;
        }
    }
    if (m_close(fd) < 0)
    {
        perror("m_close");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}