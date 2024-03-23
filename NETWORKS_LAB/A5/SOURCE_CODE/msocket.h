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

    seq_buf_index_map: An array of size MAX_SEQ_NUM which is used to map the sequence number to the index of the buffer index in the send buffer.

    timeout: An array of size MAX_SEQ_NUM which is used to keep track of the last time at which the packet was sent.

    last_buf_index: The index of the last buffer that was last written by the application user. This is used to keep track on which position the next buffer will be written by the application user.
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
/*
The shared_memory data structure is used to keep track of the shared memory between for message sending and receiving.
It contains the following fields:
    is_available: A flag to indicate whether the shared memory is available or not currently for new entry

    pid: The process id of the process that is using the socket.

    src_sock: The socket id of the source socket.

    dest_ip: The destination IP address.

    dest_port: The destination port number.

    recv_buff: A 2D array of size RECV_BUFF_SIZE x MSG_SIZE which is used to store the received messages.

    send_buff: A 2D array of size SEND_BUFF_SIZE x MSG_SIZE which is used to store the messages that are to be sent.

    send_window: The send window data structure.

    receive_window: The receive window data structure.
*/
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
/*
The SOCK_INFO data structure is used to perform the m_socket, m_bind operations.
It contains the following fields:
    sock_id: The socket id of the socket.

    IP: The IP address of the socket.

    port: The port number of the socket.

    errorno: The error number of the operation.
*/
struct SOCK_INFO
{
    int sock_id;
    struct in_addr IP;
    int port;
    int errorno;
};

/**
 * Creates a new socket with the specified domain, type, and protocol.
 *
 * @param domain    The domain of the socket (must be AF_INET).
 * @param type      The type of the socket (must be SOCK_MTP).
 * @param protocol  The protocol of the socket (must be 0).
 * @return          The index of the new socket in the table, or -1 if an error occurred.
 *                  If an error occurred, the errno variable will be set to indicate the specific error.
 */
int m_socket(int domain, int type, int protocol);
/**
 * Binds a socket to a specific source and destination address.
 *
 * @param sockfd The socket file descriptor.
 * @param src_addr Pointer to the source address structure.
 * @param addrlen Length of the source address structure.
 * @param dest_addr Pointer to the destination address structure.
 * @param addrlen1 Length of the destination address structure.
 * @return 0 on success, -1 on failure.
 *       If an error occurred, the errno variable will be set to indicate the specific error.
 */
int m_bind(int sockfd, const struct sockaddr *src_addr, socklen_t addrlen, const struct sockaddr *dest_addr, socklen_t addrlen1);
/**
 * Sends a message to a destination address using a socket. More specifically, it writes the message to the send buffer of the socket.
 *
 * @param sockfd The socket file descriptor.
 * @param buf Pointer to the buffer containing the message to be sent.
 * @param len The length of the message in bytes.
 * @param flags Flags to modify the behavior of the send operation (currently not supported).
 * @param dest_addr Pointer to a sockaddr structure containing the destination address.
 * @param addrlen The length of the destination address structure.
 * @return On success, returns the number of bytes sent. On failure, returns -1 and sets errno accordingly.
 *         Possible errno values: EOPNOTSUPP (flags not supported), EINVAL (invalid address length),
 *         EMSGSIZE (message too long), ENOTCONN (socket not connected), ENOBUFS (no buffer space available).
 */
int m_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
/**
 * Receives a message from a socket. More specifically, it reads the message from the receive buffer of the socket.
 *
 * @param sockfd The socket file descriptor.
 * @param buf Pointer to the buffer where the received message will be stored.
 * @param len The maximum length of the message to be received.
 * @param flags Flags that modify the behavior of the receive operation (currently not supported).
 * @param src_addr Pointer to a sockaddr structure that will be filled with the source address information.
 * @param addrlen Pointer to the length of the src_addr structure.
 *
 * @return On success, returns the length of the received message. On failure, returns -1 and sets errno accordingly.
 *         Possible errno values: EOPNOTSUPP (flags not supported), EMSGSIZE (message too long), ENOMSG (no message available).
 */
int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/**
 * Closes the socket with the given file descriptor. More specifically, it marks the socket as available in the shared memory.
 *
 * @param fd The file descriptor of the socket to be closed.
 * @return 0 on success, -1 on failure.
 *       If an error occurred, the errno variable will be set to indicate the specific error.
 */
int m_close(int sockfd);
/**
 * This function is used to drop a message with a probability of p.
 *
 * @param p The probability of dropping the message.
 * @return 1 if the message is dropped, 0 otherwise.
 */
int dropMessage(float p);
#endif
