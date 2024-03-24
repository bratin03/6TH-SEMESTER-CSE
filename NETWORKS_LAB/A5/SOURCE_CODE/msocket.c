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

#include "msocket.h"
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>

struct sembuf pop, vop;

#define Down(s) semop(s, &pop, 1) // Macro to perform down operation on semaphore
#define Up(s) semop(s, &vop, 1)   // Macro to perform up operation on semaphore

/**
 * This function resets the values of the given SOCK_INFO structure.
 * It sets the socket ID, error number, IP address, and port to their default values.
 *
 * @param SI The pointer to the SOCK_INFO structure to be reset.
 * @return void
 */
void reset_sock_info(struct SOCK_INFO *SI)
{
    SI->sock_id = 0;
    SI->errorno = 0;
    inet_aton("0.0.0.0", &SI->IP);
    SI->port = 0;
}

/**
 * Creates a new socket with the specified domain, type, and protocol.
 *
 * @param domain    The domain of the socket (must be AF_INET).
 * @param type      The type of the socket (must be SOCK_MTP).
 * @param protocol  The protocol of the socket (must be 0).
 * @return          The index of the new socket in the table, or -1 if an error occurred.
 *                  If an error occurred, the errno variable will be set to indicate the specific error.
 */
int m_socket(int domain, int type, int protocol)
{

    if (domain != AF_INET)
    {
        errno = EINVAL;
        return -1;
    }

    // Only SOCK_MTP is supported
    if (type != SOCK_MTP)
    {
        errno = EINVAL;
        return -1;
    }

    // No other protocol is supported
    if (protocol != 0)
    {
        errno = EINVAL;
        return -1;
    }

    key_t key_ = ftok("/usr/bin", 1);
    int shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    struct shared_memory *SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);
    key_t key_1 = ftok("/usr/bin", 2);
    int shmid_1 = shmget(key_1, sizeof(struct SOCK_INFO), 0666 | IPC_CREAT);
    struct SOCK_INFO *SI = (struct SOCK_INFO *)shmat(shmid_1, (void *)0, 0);

    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    key_t key_table_lock = ftok("/usr/bin", 3);
    int table_lock = semget(key_table_lock, 1, 0666 | IPC_CREAT);
    key_t key_sem_1 = ftok("/usr/bin", 4);
    int sem_1 = semget(key_sem_1, 1, 0666 | IPC_CREAT);
    key_t key_sem_2 = ftok("/usr/bin", 5);
    int sem_2 = semget(key_sem_2, 1, 0666 | IPC_CREAT);
    key_t key_sock_info_lock = ftok("/usr/bin", 6);
    int sock_info_lock = semget(key_sock_info_lock, 1, 0666 | IPC_CREAT);

    int free_space = -1;
    Down(table_lock);
    for (int i = 0; i < N; i++)
    {
        if (SM[i].is_available == 1)
        {
            free_space = i;
            SM[i].is_available = 0;
            break;
        }
    }
    Up(table_lock);

    if (free_space == -1)
    {
        // No free space available
        errno = ENOBUFS;
        return -1;
    }

    key_t key_sem_row = ftok("/usr/bin", 7 + free_space);
    int sem_row = semget(key_sem_row, 1, 0666 | IPC_CREAT);

    Down(sock_info_lock);
    SI->sock_id = 0;
    SI->errorno = 0;
    inet_aton("0.0.0.0", &SI->IP);
    SI->port = 0;
    Up(sem_1);
    Up(sock_info_lock);
    Down(sem_2);
    Down(sock_info_lock);
    int udp_fd = SI->sock_id;
    if (udp_fd < 0)
    {
        errno = SI->errorno;
        reset_sock_info(SI);
        Up(sock_info_lock);
        Down(table_lock);
        SM[free_space].is_available = 1;
        Up(table_lock);
        return -1;
    }
    reset_sock_info(SI);
    Up(sock_info_lock);

    Down(sem_row);
    SM[free_space].src_sock = udp_fd;
    SM[free_space].pid = getpid();

    SM[free_space].receive_window.window_size = RECV_BUFF_SIZE;
    SM[free_space].receive_window.to_deliver = 1;
    SM[free_space].receive_window.last_inorder_packet = 0;
    for (int i = 0; i < SM[free_space].receive_window.window_size; i++)
    {
        SM[free_space].receive_window.seq_buf_index_map[i] = i;
    }
    for (int i = 0; i < RECV_BUFF_SIZE; i++)
    {
        SM[free_space].receive_window.buffer_is_valid[i] = 0;
    }

    SM[free_space].send_window.last_seq_ack = 0;
    SM[free_space].send_window.window_size = RECV_BUFF_SIZE;
    SM[free_space].send_window.seq_buf_index_map[0] = 0;
    int last_ack = SM[free_space].send_window.last_seq_ack;
    for (int i = 0; i < SM[free_space].send_window.window_size; i++)
    {
        SM[free_space].send_window.seq_buf_index_map[(i + last_ack + 1) % (MAX_SEQ_NUM)] = (SM[free_space].send_window.seq_buf_index_map[last_ack] + i + 1) % (SEND_BUFF_SIZE);
    }
    for (int i = 0; i < SEND_BUFF_SIZE; i++)
    {
        SM[free_space].send_window.buffer_is_valid[i] = 0;
    }
    SM[free_space].send_window.last_buf_index = 0;
    Up(sem_row);
    shmdt(SM);
    shmdt(SI);
    return free_space;
}

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
int m_bind(int sockfd, const struct sockaddr *src_addr, socklen_t addrlen, const struct sockaddr *dest_addr, socklen_t addrlen1)
{
    if (addrlen != sizeof(struct sockaddr_in) || addrlen1 != sizeof(struct sockaddr_in))
    {
        errno = EINVAL;
        return -1;
    }
    if (((struct sockaddr_in *)src_addr)->sin_family != AF_INET || ((struct sockaddr_in *)dest_addr)->sin_family != AF_INET)
    {
        errno = EINVAL;
        return -1;
    }
    key_t key_ = ftok("/usr/bin", 1);
    int shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    struct shared_memory *SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);

    key_t key_1 = ftok("/usr/bin", 2);
    int shmid_1 = shmget(key_1, sizeof(struct SOCK_INFO), 0666 | IPC_CREAT);
    struct SOCK_INFO *SI = (struct SOCK_INFO *)shmat(shmid_1, (void *)0, 0);

    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    key_t key_sem_1 = ftok("/usr/bin", 4);
    int sem_1 = semget(key_sem_1, 1, 0666 | IPC_CREAT);
    key_t key_sem_2 = ftok("/usr/bin", 5);
    int sem_2 = semget(key_sem_2, 1, 0666 | IPC_CREAT);
    key_t key_sock_info_lock = ftok("/usr/bin", 6);
    int sock_info_lock = semget(key_sock_info_lock, 1, 0666 | IPC_CREAT);
    key_t key_sem_row = ftok("/usr/bin", 7 + sockfd);
    int sem_row = semget(key_sem_row, 1, 0666 | IPC_CREAT);

    Down(sock_info_lock);
    int actual_sockfd = SM[sockfd].src_sock;
    SI->sock_id = actual_sockfd;
    SI->errorno = 0;
    SI->IP = ((struct sockaddr_in *)src_addr)->sin_addr;
    SI->port = ((struct sockaddr_in *)src_addr)->sin_port;
    Up(sock_info_lock);
    Up(sem_1);
    Down(sem_2);
    Down(sock_info_lock);
    int udp_fd = SI->sock_id;
    if (udp_fd < 0)
    {
        errno = SI->errorno;
        reset_sock_info(SI);
        Up(sock_info_lock);
        return -1;
    }
    reset_sock_info(SI);
    Up(sock_info_lock);
    Down(sem_row);
    SM[sockfd].dest_ip = ((struct sockaddr_in *)dest_addr)->sin_addr;
    SM[sockfd].dest_port = ((struct sockaddr_in *)dest_addr)->sin_port;
    Up(sem_row);
    shmdt(SM);
    shmdt(SI);
    return 0;
}

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
int m_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    if (flags != 0)
    {
        errno = EOPNOTSUPP;
        return -1;
    }
    if (sizeof(struct sockaddr_in) != addrlen)
    {
        errno = EINVAL;
        return -1;
    }
    key_t key_ = ftok("/usr/bin", 1);
    int shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    struct shared_memory *SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);

    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    key_t key_sem_row = ftok("/usr/bin", 7 + sockfd);
    int sem_row = semget(key_sem_row, 1, 0666 | IPC_CREAT);

    if (len > MSG_SIZE)
    {
        errno = EMSGSIZE;
        return -1;
    }

    Down(sem_row);
    int dest_port_table = SM[sockfd].dest_port;
    struct in_addr dest_ip_table = SM[sockfd].dest_ip;
    int port_param = ((struct sockaddr_in *)dest_addr)->sin_port;
    struct in_addr ip_param = ((struct sockaddr_in *)dest_addr)->sin_addr;

    if (port_param != dest_port_table || ip_param.s_addr != dest_ip_table.s_addr)
    {
        // Destination address does not match
        errno = ENOTCONN;
        Up(sem_row);
        return -1;
    }

    int next_buf_index = (SM[sockfd].send_window.last_buf_index + 1) % SEND_BUFF_SIZE;
    if (SM[sockfd].send_window.buffer_is_valid[next_buf_index] != 0)
    {
        // Buffer space not available
        errno = ENOBUFS;
        Up(sem_row);
        return -1;
    }
    SM[sockfd].send_window.buffer_is_valid[next_buf_index] = 1;
    for (int i = 0; i < MSG_SIZE; i++)
    {
        SM[sockfd].send_buff[next_buf_index][i] = ((char *)buf)[i];
    }
    SM[sockfd].send_window.last_buf_index = next_buf_index;
    Up(sem_row);
    shmdt(SM);
    return len;
}

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
int m_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    if (flags != 0)
    {
        errno = EOPNOTSUPP;
        return -1;
    }
    if (len > MSG_SIZE)
    {
        errno = EMSGSIZE;
        return -1;
    }

    key_t key_ = ftok("/usr/bin", 1);
    int shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    struct shared_memory *SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);

    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    key_t key_sem_row = ftok("/usr/bin", 7 + sockfd);
    int sem_row = semget(key_sem_row, 1, 0666 | IPC_CREAT);

    Down(sem_row);
    int next_buf_index = (SM[sockfd].receive_window.to_deliver) % RECV_BUFF_SIZE;
    if (SM[sockfd].receive_window.buffer_is_valid[next_buf_index] == 0)
    {
        // No message available
        Up(sem_row);
        errno = ENOMSG;
        return -1;
    }
    for (int i = 0; i < MSG_SIZE; i++)
    {
        ((char *)buf)[i] = SM[sockfd].recv_buff[next_buf_index][i];
    }
    SM[sockfd].receive_window.buffer_is_valid[next_buf_index] = 0;
    SM[sockfd].receive_window.to_deliver = (SM[sockfd].receive_window.to_deliver + 1) % RECV_BUFF_SIZE;
    if (SM[sockfd].receive_window.window_size < 5)
    {
        SM[sockfd].receive_window.window_size++;
    }
    for (int j = 0; j < RECV_BUFF_SIZE; j++)
    {
        SM[sockfd].receive_window.seq_buf_index_map[(SM[sockfd].receive_window.last_inorder_packet + j + 1) % MAX_SEQ_NUM] = (SM[sockfd].receive_window.seq_buf_index_map[SM[sockfd].receive_window.last_inorder_packet] + j + 1) % RECV_BUFF_SIZE;
    }
    if (SM[sockfd].receive_window.nospace == 1)
    {
        SM[sockfd].receive_window.nospace = 0;
    }
    Up(sem_row);
    struct sockaddr_in *src_addr_in = (struct sockaddr_in *)src_addr;
    src_addr_in->sin_family = AF_INET;
    src_addr_in->sin_port = SM[sockfd].dest_port;
    src_addr_in->sin_addr = SM[sockfd].dest_ip;
    *addrlen = sizeof(struct sockaddr_in);
    shmdt(SM);
    return len;
}
/**
 * Closes the socket with the given file descriptor. More specifically, it marks the socket as available in the shared memory.
 *
 * @param fd The file descriptor of the socket to be closed.
 * @return 0 on success, -1 on failure.
 *       If an error occurred, the errno variable will be set to indicate the specific error.
 */

int m_close(int fd)
{
    if (fd < 0)
    {
        errno = EBADF;
        return -1;
    }
    key_t key_ = ftok("/usr/bin", 1);
    int shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    struct shared_memory *SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);
    key_t key_sem_row = ftok("/usr/bin", 7 + fd);
    int sem_row = semget(key_sem_row, 1, 0666 | IPC_CREAT);
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;
    Down(sem_row);
    SM[fd].is_available = 1;
    Up(sem_row);
    shmdt(SM);
    return 0;
}

/**
 * This function is used to drop a message with a probability of p.
 *
 * @param p The probability of dropping the message.
 * @return 1 if the message is dropped, 0 otherwise.
 */
int dropMessage(float p)
{

    float r = ((float)rand() / (float)RAND_MAX);
    if (r <= p)
    {
        return 1;
    }
    return 0;
}