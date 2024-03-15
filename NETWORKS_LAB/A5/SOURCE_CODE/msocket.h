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

#ifndef MSOCKET_H
#define MSOCKET_H

#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define T 5
#define RECV_BUFF_SIZE 5
#define SEND_BUFF_SIZE 10
#define MAX_SEQ_NUM 16
#define MSG_SIZE 1024
#define N 25
#define SOCK_MTP 689
#define P 0.2
#define MAX_TRIES_AFTER_CLOSE 8

extern struct sembuf pop, vop;

struct swnd
{
    int window_size;
    int seq_buf_index_map[MAX_SEQ_NUM];
    int last_seq_ack;
    int buffer_is_valid[SEND_BUFF_SIZE];
    time_t timeout[MAX_SEQ_NUM];
    int last_buf_index;
};

struct rwnd
{
    int to_deliver;
    int window_size;
    int last_inorder_packet;
    int buffer_is_valid[RECV_BUFF_SIZE];
    int seq_buf_index_map[MAX_SEQ_NUM];
    int nospace;
};

struct shared_memory
{
    int is_available;

    pid_t pid;
    int src_sock;
    struct in_addr dest_ip;
    int dest_port;
    char recv_buff[RECV_BUFF_SIZE][MSG_SIZE];
    char send_buff[SEND_BUFF_SIZE][MSG_SIZE];
    struct swnd send_window;
    struct rwnd receive_window;
};
struct SOCK_INFO
{
    int sock_id;
    struct in_addr IP;
    int port;
    int errorno;
    int to_close;
};

int m_socket(int domain, int type, int protocol);
int m_bind(int sockfd, const struct sockaddr *src_addr, socklen_t addrlen, const struct sockaddr *dest_addr, socklen_t addrlen1);
int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
int m_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
int m_close(int sockfd);
int dropMessage(float p);
#endif
