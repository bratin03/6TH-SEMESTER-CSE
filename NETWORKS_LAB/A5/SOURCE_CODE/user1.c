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

The end of the file is marked by sending a message with all characters as '$'
*/

/*
Run with DRANDOM defined to send random messages with sequence numbers
Run without DRANDOM to send messages from a file

Run with DVERBOSE defined to see sleep messages

Also reduce the MSG_SIZE to smaller value (15) fr better I/O during testing with DRANDOM flag
*/

#include "msocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// #define DVERBOSE
// #define DRANDOM
#define WAIT_TIME 1
// Change here for any other file or IP address or port
#define INPUT_FILE "input.txt"
#define IP_1 "127.0.0.1"
#define IP_2 "127.0.0.1"
#define PORT_1 15081
#define PORT_2 15080

void signal_handler(int signum)
{
    printf("\nSignal %d received\n", signum);
    printf("Exiting\n");
    exit(EXIT_SUCCESS);
}

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

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

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
    src_addr.sin_port = htons(PORT_1);
    src_addr.sin_addr.s_addr = inet_addr(IP_1);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT_2);
    dest_addr.sin_addr.s_addr = inet_addr(IP_2);
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
#ifdef DVERBOSE
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
        if (count >= 100)
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
#ifdef DVERBOSE
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
            printf("End Sent\n");
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
    int count = 1;
    char buf[MSG_SIZE];
    while (fgets(buf, MSG_SIZE, fp) != NULL)
    {
        while (1)
        {
            ret = m_sendto(fd, buf, MSG_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (ret == -1)
            {
                if (errno == ENOBUFS)
                {
#ifdef DVERBOSE
                    printf("Buffer is currently Full. Sleeping for %d seconds\n", WAIT_TIME);
#endif
                }
                else
                {
                    perror("m_sendto");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printf("Sent Message No: %d\n", count);
                count++;
                break;
            }
            sleep(WAIT_TIME);
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
#ifdef DVERBOSE
                printf("Buffer is currently Full. Sleeping for %d seconds\n", WAIT_TIME);
#endif
                sleep(WAIT_TIME);
            }
            else
            {
                perror("m_sendto");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("End Sent\n");
            break;
        }
    }
    fclose(fp);
    while (1)
    {
    }
    exit(EXIT_SUCCESS);
#endif
}