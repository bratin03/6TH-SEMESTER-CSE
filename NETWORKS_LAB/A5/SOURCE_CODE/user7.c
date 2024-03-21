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
User 1 sends messages to user2 from a file
*/

/*
Run with DRANDOM defined to send random messages with sequence numbers
Also reduce the MSG_SIZE to smaller value (15) fr better I/O during testing with DRANDOM flag
*/

#include "msocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// #define VERBOSE
#define DRANDOM
#define WAIT_TIME 1
#define INPUT_FILE "input.txt"

#ifdef DRANDOM
int generate_message_of_length(char *buf, int len, int seq_num)
{
    sprintf(buf, "%4d", seq_num);
    for (int i = 4; i < len - 1; i++)
    {
        buf[i] = '-';
    }
    buf[len - 2] = '\n';
    buf[len - 1] = '\0';
    return 0;
}
#endif

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
    src_addr.sin_port = htons(11081);
    src_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(11080);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = m_bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr), (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("m_bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful from IP addr %s and port %d to IP addr %s and port %d\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));

#ifdef DRANDOM
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
            printf("Sent: %d\n", count);
            count++;
        }
        if (count > 8192)
        {
            break;
        }
    }
    char end[MSG_SIZE];
    for (int i = 0; i < MSG_SIZE; i++)
    {
        end[i] = '$';
    }
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
            // printf("Sent: %s\n", end);
            break;
        }
    }

    while (1)
    {
    }
    exit(EXIT_SUCCESS);
#else
    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char buf[MSG_SIZE];
    while (fgets(buf, MSG_SIZE, fp) != NULL)
    {
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
        }
    };
    char end[] = "$";
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
#endif
}