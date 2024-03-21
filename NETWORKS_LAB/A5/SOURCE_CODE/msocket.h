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
#define MSG_SIZE 15
#define N 25
#define SOCK_MTP 689
#define P 0.2
#define MAX_TRIES_AFTER_CLOSE 8

extern struct sembuf pop, vop;

/*
The swnd data structure is used to keep track of the send window.
It contains the following fields:
    window_size: The size of the send window.

    last_seq_ack: The last sequence number that was acknowledged.

    buffer_is_valid: An array of size SEND_BUFF_SIZE which is used to keep track of whether the buffer at a particular index is valid or not.
        If the message at a particular index is garbage, then buffer_is_valid[i] = 0 ( Either the application user has not written anything in the buffer or the message has been sent and acknowledged).
        If the message at a particular index has been written by the application user and has not been sent even once then buffer_is_valid[i] = 1.
        If the message at a particular index has been sent but not acknowledged then buffer_is_valid[i] = 2.

    seq_buf_index_map: An array of size MAX_SEQ_NUM which is used to map the sequence number to the index of the buffer in the send buffer.

    timeout: An array of size MAX_SEQ_NUM which is used to keep track of the last time at which the packet was sent.

    last_buf_index: The index of the last buffer that was sent. This is used to keep track on which position the next buffer will be written by the application user.
*/
struct swnd
{
    int window_size;
    int last_seq_ack;
    int buffer_is_valid[SEND_BUFF_SIZE];
    int seq_buf_index_map[MAX_SEQ_NUM];
    time_t timeout[MAX_SEQ_NUM];
    int last_buf_index;
};

/*
The rwnd data structure is used to keep track of the receive window.
It contains the following fields:
    to_deliver: The buffer index of the next message that has to be delivered to the application user.

    window_size: The size of the receive window.

    last_inorder_packet: The sequence number of the last packet that was delivered.

    buffer_is_valid: An array of size RECV_BUFF_SIZE which is used to keep track of whether the buffer at a particular index is valid or not.
        If the message at a particular index is garbage, then buffer_is_valid[i] = 0 ( Either the message has not been received or the message has been delivered to the application user).
        If the message at a particular index has been received but not delivered to the application user then buffer_is_valid[i] = 1.

    seq_buf_index_map: An array of size MAX_SEQ_NUM which is used to map the sequence number to the index of the buffer in the receive buffer.

    nospace: A flag to indicate that there is no space in the receive buffer to store the received message.
*/
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
};

int m_socket(int domain, int type, int protocol);
int m_bind(int sockfd, const struct sockaddr *src_addr, socklen_t addrlen, const struct sockaddr *dest_addr, socklen_t addrlen1);
int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
int m_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
int m_close(int sockfd);
int dropMessage(float p);
#endif
