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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#define ACK_SIZE 10
#define SP_MSG_SIZE 2

// Define DLOG to enable the server logs
#define DLOG
// Define DSTAT to enable the statistics on exit
#define DSTAT

/*
Header Design:

Data Message:
    0 0 SeqNum(4) Message(1024)
    - This format is used to send data messages

Ack Message:
    1 0 SeqNum(4) WindowSize(4)
    - This format is used to send acks

Special Ack Message:
    1 1 seqNum(4) WindowSize(4)
    - This is a special ack message to indicate that earlier the window was full but now it is not and the sender can send more messages
    - The second bit is set to 1 to indicate that this is a special ack message
    - The sender is supposed to send more messages after receiving this ack if he has any
    - Otherwise if the sender does not have any more messages to send, he should reply with a special message to the receiver to indicate that he has successfully received the information of buffer space availability
    - Until the sender either sends a special message or a data message, the receiver will keep on sending special ack messages to indicate buffer space availability

Special Message:
    0 1
    - This is a special message to reply against special ack messages to indicate that the sender has successfully received the information of buffer space availability but currently does not have any more messages to send
    - On receiving ack indicating buffer space availability on the receiver side, the sender should either send this message or a data message to the receiver

    Note: In this header format we have actually used one total byte to indicate one bit of header information. This is not the most efficient way to do it but it is done to keep the implementation simple and easy to understand with better representation of the header information.
*/

// Functions for logging in color
#ifdef DLOG
void red()
{
    printf("\033[1;31m");
}

void green()
{
    printf("\033[1;32m");
}

void pink()
{
    printf("\033[1;35m");
}

void orange()
{
    printf("\033[1;33m");
}

void blue()
{
    printf("\033[1;34m");
}

void cyan()
{
    printf("\033[1;36m");
}

void grey()
{
    printf("\033[1;37m");
}

void reset()
{
    printf("\033[0m");
}
#endif

// Semaphore operations
#define Down(s) semop(s, &pop, 1)
#define Up(s) semop(s, &vop, 1)

int shmid, shmid_1;
struct shared_memory *SM;
struct SOCK_INFO *SI;
int table_lock, sem_1, sem_2, sock_info_lock;
int sem_row[N];
struct sembuf pop, vop;
int msg_count = 0, msg_send_count = 0, ack_count = 0;

/**
 * Signal handler function that handles SIGQUIT and SIGINT signals.
 * Clears shared memory and exits the program.
 *
 * @param signo The signal number.
 */
void sig_handler(int signo)
{
    if (signo == SIGQUIT || signo == SIGINT)
    {
#ifdef DLOG
        grey();
        printf("\n\nInit:\nSignal received %s\n", strsignal(signo));
        printf("Clearing Shared Memory\n");
        reset();
#endif
        shmdt(SM);
        shmdt(SI);
        shmctl(shmid, IPC_RMID, NULL);
        shmctl(shmid_1, IPC_RMID, NULL);
        semctl(table_lock, 0, IPC_RMID);
        semctl(sem_1, 0, IPC_RMID);
        semctl(sem_2, 0, IPC_RMID);
        semctl(sock_info_lock, 0, IPC_RMID);
        for (int i = 0; i < N; i++)
        {
            semctl(sem_row[i], 0, IPC_RMID);
        }
#ifdef DLOG
        grey();
        printf("Shared Memory Cleared\n");
        printf("Exiting\n");
        reset();
#endif

#ifdef DSTAT
        printf("\n\nStatistics:\n");
        printf("Value of P: %lf\n", P);
        if (msg_count == 0)
        {
            printf("No messages sent\n");
            exit(EXIT_SUCCESS);
        }
        double avg_msg = ((double)msg_send_count / (double)msg_count);
        double avg_ack = ((double)ack_count / (double)msg_count);
        for (int i = 0; i < 40; i++)
        {
            printf("-");
        }
        printf("\n");
        printf("Total Messages Sent: %d\n", msg_count);
        printf("Total Times Message Sent: %d\n", msg_send_count);
        printf("Total Times Ack Sent: %d\n", ack_count);
        printf("Average Messages Sent per Message: %lf\n", avg_msg);
        printf("Average Ack Sent per Message: %lf\n", avg_ack);
        double total = avg_msg + avg_ack;
        printf("Total Average Transmissions (Messages + Ack) per Message: %lf\n", total);
        for (int i = 0; i < 40; i++)
        {
            printf("-");
        }
        printf("\n");
#endif
        exit(EXIT_SUCCESS);
    }
}

/**
 * Converts a decimal number to a binary representation.
 *
 * @param decimal The decimal number to convert.
 * @param buffer The buffer to store the binary representation.
 */
void decimal_to_binary(int decimal, char *buffer)
{
    int mask = 1 << 3;

    for (int i = 0; i < 4; i++)
    {
        int bit = (decimal & mask) ? 1 : 0;
        buffer[i] = bit + '0';
        mask >>= 1;
    }
}

/**
 * Sends an acknowledgment packet over a socket.
 *
 * @param sock The socket descriptor.
 * @param dest_ip The destination IP address.
 * @param dest_port The destination port number.
 * @param seq_num The sequence number of the acknowledgment.
 * @param window_size The window size of the acknowledgment.
 * @param ack_type The type of the acknowledgment.
 */
void send_ack(int sock, struct in_addr dest_ip, int dest_port, int seq_num, int window_size, int ack_type)
{

    ack_count++;

    char ack[ACK_SIZE];
    ack[0] = '1';
    ack[1] = '0' + ack_type;
    decimal_to_binary(seq_num, ack + 2);
    decimal_to_binary(window_size, ack + 6);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dest_port;
    dest_addr.sin_addr = dest_ip;

    int ret = sendto(sock, ack, ACK_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
#ifdef DLOG
    grey();
    printf("Ack sent by socket %d. Ack No: %d Window Size: %d\n", sock, seq_num, window_size);
    reset();
#endif
}

/**
 * Sends a message over a socket to a specified destination IP and port.
 *
 * @param sock The socket descriptor.
 * @param dest_ip The destination IP address.
 * @param dest_port The destination port number.
 * @param seq_num The sequence number of the message.
 * @param msg The message to be sent.
 */
void send_msg(int sock, struct in_addr dest_ip, int dest_port, int seq_num, char *msg)
{
    msg_send_count++;
    char send_msg[MSG_SIZE + 6];
    send_msg[0] = '0';
    send_msg[1] = '0';
    decimal_to_binary(seq_num, send_msg + 2);
    for (int i = 0; i < MSG_SIZE; i++)
    {
        send_msg[i + 6] = msg[i];
    }
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dest_port;
    dest_addr.sin_addr = dest_ip;
    int ret = sendto(sock, send_msg, MSG_SIZE + 6, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
#ifdef DLOG
    cyan();
    printf("Message sent by socket %d. Seq No: %d\n", sock, seq_num);
    reset();
#endif
}

/**
 * Sends an empty message (01) to the specified destination IP and port using the given socket.
 *
 * @param sock The socket to send the message from.
 * @param dest_ip The destination IP address.
 * @param dest_port The destination port number.
 */
void send_empty_msg(int sock, struct in_addr dest_ip, int dest_port)
{
    msg_send_count++;
    char send_msg[SP_MSG_SIZE];
    send_msg[0] = '0';
    send_msg[1] = '1';
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dest_port;
    dest_addr.sin_addr = dest_ip;
    int ret = sendto(sock, send_msg, SP_MSG_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
#ifdef DLOG
    cyan();
    printf("Empty Message sent by socket %d\n", sock);
    reset();
#endif
}

/**
 * Converts a binary string to its decimal equivalent.
 *
 * @param binary The binary string to be converted.
 * @param size The size of the binary string.
 * @return The decimal equivalent of the binary string.
 */
int binary_to_decimal(char *binary, int size)
{
    int decimal = 0;
    int base = 1;
    for (int i = size - 1; i >= 0; i--)
    {
        if (binary[i] == '1')
        {
            decimal += base;
        }
        base *= 2;
    }
    return decimal;
}

/**

 * This function continuously checks the status of each table entry in the SM array.
 * If a process is found to be dead, it marks the corresponding table entry as available,
 * closes the source socket, and resets the destination IP address.
 * If an error occurs while checking the process status or closing the socket, the function exits with failure.
 *
 * @return void
 */
void *Garbage_Collector()
{
    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            // If closed by m_close, close the socket and reset the destination IP address
            if (SM[i].src_sock != 0 && SM[i].is_available == 1)
            {
                Down(table_lock);
                close(SM[i].src_sock);
                SM[i].src_sock = 0;
                SM[i].pid = 0;
                inet_aton("0.0.0.0", &SM[i].dest_ip);
                Up(table_lock);
                continue;
            }

            // Check if the process is dead
            if (SM[i].is_available == 0)
            {
                Down(sem_row[i]);

                if (kill(SM[i].pid, 0) == -1)
                {
                    if (errno == ESRCH)
                    {
                        // Process is dead
#ifdef DLOG
                        red();
                        printf("Garbage Collector: Process %d for table entry %d is dead\n", SM[i].pid, i);
                        reset();
#endif
                        // Mark the table entry as available and reset the destination IP address
                        SM[i].is_available = 1;
                        SM[i].pid = 0;
                        close(SM[i].src_sock);
                        SM[i].src_sock = 0;
                        inet_aton("0.0.0.0", &SM[i].dest_ip);
                    }
                    else
                    {
                        Up(sem_row[i]);
                        perror("kill");
                        exit(EXIT_FAILURE);
                    }
                }
                Up(sem_row[i]);
            }
        }
        // Sleep for some time before checking again
        sleep(2 * T);
    }
}

/**
 * The R_Thread function is responsible for handling incoming packets from multiple sockets.
 * It uses the select system call to monitor the sockets for incoming data.
 * When a packet is received, it processes the packet and updates the receive window for the corresponding socket.
 * If the receive window becomes full, it sends an acknowledgment indicating that there is no space available.
 * If the receive window has space available, it sends an acknowledgment indicating the last in-order packet received and the current window size.
 * The R_Thread function runs in an infinite loop, continuously monitoring the sockets for incoming packets.
 * @return void
 */

void *R_Thread()
{
    // Declare the fd_set and timeval structures
    fd_set readfds;
    struct timeval tv;
    int max_fd = 0;

    int prev_empty[N] = {0}; // Array to keep track of whether the receive window was full in the previous iteration
    while (1)
    {
        max_fd = 0;
        FD_ZERO(&readfds);
        tv.tv_sec = T / 2 - (1 - T % 2); // TIme to wait for select
        tv.tv_usec = 0;
        Down(table_lock);
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                FD_SET(SM[i].src_sock, &readfds); // Add the socket to the fd_set if it is currently in use
                if (SM[i].src_sock > max_fd)
                {
                    max_fd = SM[i].src_sock;
                }
            }
        }
        Up(table_lock);
        int ret = select(max_fd + 1, &readfds, NULL, NULL, &tv); // Wait for incoming data on the sockets
        if (ret == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        if (ret == 0)
        {
            // Timeout
            // Check for each table entry if the receive window was full and now has space available
            // If the receive window has space available, send an acknowledgment indicating the last in-order packet received and the current window size
            for (int i = 0; i < N; i++)
            {
                if (prev_empty[i] == 1 && SM[i].is_available == 0 && SM[i].receive_window.nospace == 0)
                {

                    Down(sem_row[i]);
                    int current_window_size = 0;
                    int last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                    // Calculate the current window size by checking the number of packets that are in-order and have been received but not yet delivered to the application user
                    for (int j = 0; j < RECV_BUFF_SIZE; j++)
                    {
                        if (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j) % MAX_SEQ_NUM]] == 1)
                        {
                            break;
                        }
                        current_window_size++;
                    }
                    SM[i].receive_window.window_size = current_window_size;

                    for (int j = 0; j < RECV_BUFF_SIZE; j++)
                    {
                        SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + (j + 1)) % RECV_BUFF_SIZE;
                    }
                    // Send a special acknowledgment indicating that the window was full but now has space available
                    send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size, 1);
#ifdef DLOG
                    green();
                    printf("R Thread: Sending Ack indicating space available for table entry %d. Ack No: %d Window Size: %d\n", i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                    reset();
#endif
                    Up(sem_row[i]);
                }
            }
            continue;
        }
        for (int i = 0; i < N; i++)
        {

            if (SM[i].is_available == 0)
            {
                // Check if the socket has incoming data
                if (FD_ISSET(SM[i].src_sock, &readfds))
                {
                    struct sockaddr_in src_addr;
                    socklen_t src_addr_len = sizeof(src_addr);
                    char msg[MSG_SIZE + 6];
                    int ret = recvfrom(SM[i].src_sock, msg, MSG_SIZE + 6, 0, (struct sockaddr *)&src_addr, &src_addr_len);
#ifdef DLOG
                    orange();
                    int seq_num = binary_to_decimal(msg + 2, 4);
                    printf("R Thread: Received packet for table entry %d with seq num %d\n", i, seq_num);
                    reset();
#endif
                    if (ret == -1)
                    {
                        perror("recvfrom");
                        exit(EXIT_FAILURE);
                    }
                    // Drop the packet with probability P
                    if (dropMessage(P))
                    {
#ifdef DLOG
                        red();
                        printf("R Thread: Dropped packet for table entry %d with seq num %d\n", i, seq_num);
                        reset();
#endif
                        continue;
                    }
#ifdef DLOG
                    blue();
                    printf("R Thread: Packet Not Dropped for table entry %d with seq num %d\n", i, seq_num);
                    reset();
#endif
                    char id_bit = msg[0];
                    // If the first bit is 0, it is a data message
                    if (id_bit == '0')
                    {
                        char type_bit = msg[1];
                        if (type_bit == '1')
                        {
                            // Special Message
                            // Mark that the sender has received the information of buffer space availability
                            // Stop sending special ack messages to the sender indicating buffer space availability
                            Down(sem_row[i]);
                            SM[i].receive_window.nospace = 0;
                            prev_empty[i] = 0;
#ifdef DLOG
                            green();
                            printf("R Thread: Received Empty Message for table entry %d\n", i);
                            reset();
#endif
                            Up(sem_row[i]);
                            continue;
                        }
                        char seq_num[5];
                        for (int i = 0; i < 4; i++)
                        {
                            seq_num[i] = msg[i + 2];
                        }
                        seq_num[4] = '\0';
                        int seq_num_int = binary_to_decimal(seq_num, 4); // Convert the sequence number to an integer
                        Down(sem_row[i]);
                        int last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                        int window_size = SM[i].receive_window.window_size;
                        int next_to_deliver = (last_inorder_packet + 1) % MAX_SEQ_NUM;

                        // If the packet is already received, send an acknowledgment indicating the last in-order packet received and the current window size
                        if (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[seq_num_int]] == 1)
                        {
                            send_ack(SM[i].src_sock, src_addr.sin_addr, src_addr.sin_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size, 0);
                            Up(sem_row[i]);
                            continue;
                        }

                        // If the packet is in the receive window, process the packet
                        if (((seq_num_int - last_inorder_packet + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {
                            // If the packet is in the receive window and not received yet, store the message in the receive buffer
                            int buffer_index = SM[i].receive_window.seq_buf_index_map[seq_num_int];
                            if (SM[i].receive_window.buffer_is_valid[buffer_index] == 0 && seq_num_int != last_inorder_packet)
                            {
                                for (int msglen = 0; msglen < MSG_SIZE; msglen++)
                                {
                                    SM[i].recv_buff[buffer_index][msglen] = msg[msglen + 6];
                                }
                                SM[i].receive_window.buffer_is_valid[buffer_index] = 1;
                            }
                            // If the sequence number is the expected sequence number, recalculate the window size and the last in-order packet
                            if (seq_num_int == next_to_deliver)
                            {
                                for (int j = 0; j <= RECV_BUFF_SIZE; j++)
                                {
                                    SM[i].receive_window.seq_buf_index_map[(next_to_deliver + j) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[next_to_deliver] + j) % RECV_BUFF_SIZE;
                                }
                                // First check if there is space available outside the window
                                int buffer_index_outside_window = SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + window_size + 1) % MAX_SEQ_NUM];
                                int outside_free_count = 0;
                                for (int j = 0; j < RECV_BUFF_SIZE; j++)
                                {

                                    int cur_buf_index = (buffer_index_outside_window + j) % RECV_BUFF_SIZE;
                                    if (cur_buf_index == SM[i].receive_window.seq_buf_index_map[last_inorder_packet])
                                    {
                                        break;
                                    }
                                    if (SM[i].receive_window.buffer_is_valid[cur_buf_index] == 0)
                                    {
                                        outside_free_count++;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }

                                // See in the current window how many packets have been received in-order
                                last_inorder_packet = seq_num_int;
                                while (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + 1) % MAX_SEQ_NUM]] == 1 && (((last_inorder_packet + 1 - next_to_deliver + MAX_SEQ_NUM) % MAX_SEQ_NUM) < window_size))
                                {
                                    last_inorder_packet = (last_inorder_packet + 1) % MAX_SEQ_NUM;
                                }
                                // Calculate the new window size based on what is the next expected packet
                                int new_window_size = (buffer_index_outside_window - (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + 1) + RECV_BUFF_SIZE + outside_free_count) % RECV_BUFF_SIZE;
                                SM[i].receive_window.last_inorder_packet = last_inorder_packet;
                                SM[i].receive_window.window_size = new_window_size;
                                // If the window size is 0, mark that there is no space available
                                if (new_window_size == 0)
                                {
                                    SM[i].receive_window.nospace = 1;
                                    prev_empty[i] = 1;
                                }
                                // If the window size is not 0, mark that there is space available
                                else
                                {
                                    prev_empty[i] = 0;
                                }
                            }
                            else if (SM[i].receive_window.window_size != 0)
                            {
                                prev_empty[i] = 0;
                            }
#ifdef DLOG
                            green();
                            printf("R Thread: Received packet %d for table entry %d\n", seq_num_int, i);
                            printf("Sending Ack for table entry %d. Ack No: %d Window Size: %d\n", i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            reset();
#endif
                            send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size, 0);
                            Up(sem_row[i]);
                        }
                        // If no space is available in the receive buffer, send an acknowledgment indicating that there is no space available
                        else if (SM[i].receive_window.nospace == 1)
                        {
#ifdef DLOG
                            green();
                            printf("R Thread: Received packet %d for table entry %d. No space available. Sending Ack indicating space available for table entry %d. Ack No: %d Window Size: %d\n", seq_num_int, i, i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            printf("Sending Ack indicating no space available for table entry %d. Ack No: %d Window Size: %d\n", i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            reset();
#endif
                            send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size, 0);

                            Up(sem_row[i]);
                        }
                        else
                        {
                            Up(sem_row[i]);
                        }
                    }
                    else if (id_bit == '1')
                    {
                        // Acknowledgment Message
                        char seq_num[5];
                        strncpy(seq_num, msg + 2, 4);
                        for (int i = 0; i < 4; i++)
                        {
                            seq_num[i] = msg[i + 2];
                        }
                        seq_num[4] = '\0';
                        int ack_num = binary_to_decimal(seq_num, 4); // Convert the sequence number to an integer
                        char window_size_update[5];
                        for (int i = 0; i < 4; i++)
                        {
                            window_size_update[i] = msg[i + 6];
                        }
                        window_size_update[4] = '\0';
                        int new_window_size = binary_to_decimal(window_size_update, 4); // Convert the window size to an integer
                        Down(sem_row[i]);
                        int last_seq_ack = SM[i].send_window.last_seq_ack;
                        int window_size = SM[i].send_window.window_size;

                        // If the acknowledgment is within the window, update the window size and the last acknowledged packet
                        if (((ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {
                            SM[i].send_window.last_seq_ack = ack_num;
                            if (ack_num == last_seq_ack)
                            {
                                // Do nothing
                            }
                            else
                            {
                                for (int j = 0; j < (ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM; j++)
                                {
                                    SM[i].send_window.buffer_is_valid[SM[i].send_window.seq_buf_index_map[(last_seq_ack + j + 1) % MAX_SEQ_NUM]] = 0;
                                    msg_count++;
                                }
                            }
                        }

                        SM[i].send_window.window_size = new_window_size;
                        for (int j = 0; j < new_window_size; j++)
                        {
                            SM[i].send_window.seq_buf_index_map[(ack_num + j + 1) % MAX_SEQ_NUM] = (SM[i].send_window.seq_buf_index_map[ack_num] + (j + 1)) % SEND_BUFF_SIZE;
                        }
#ifdef DLOG
                        pink();
                        printf("R Thread: Received Ack for table entry %d. Ack No: %d Window Size: %d\n", i, ack_num, new_window_size);
                        printf("Updated window size for table entry %d: %d and Next to send: %d\n", i, new_window_size, (ack_num + 1) % MAX_SEQ_NUM);
                        reset();
#endif
                        // If it is a special acknowledgment and sender has no data to send, transmit an empty message to indicate that the sender has received the information of buffer space availability
                        if (new_window_size > 0 && SM[i].send_window.buffer_is_valid[SM[i].send_window.seq_buf_index_map[(ack_num + 1) % MAX_SEQ_NUM]] == 0 && msg[1] == '1')
                        {
                            send_empty_msg(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port);
                        }

                        Up(sem_row[i]);
                    }
                }
            }
        }
    }
}

/**
 * The S_Thread continuously checks each table entry in the send window and sends packets if they are available.
 * It uses a sleep time to control the rate of sending packets.
 * If a packet needs to be resent due to a timeout, it updates the timeout value and resends the packet.
 *
 * @return void
 */
void *S_Thread()
{
    int sleep_time = (T / 2 - (1 - T % 2)); // Sleep time to control the rate of sending packets
    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                Down(sem_row[i]);
                int next_to_send = (SM[i].send_window.last_seq_ack + 1) % MAX_SEQ_NUM;
                int next_to_send_index = SM[i].send_window.seq_buf_index_map[next_to_send];
                int window_size = SM[i].send_window.window_size;

                // Send packets if they are available in the send window
                for (int j = 0; j < window_size; j++)
                {
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 0)
                    {
                        continue;
                    }
                    // If the packet is already sent and not acknowledged, check if it is time to resend the packet
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 2)
                    {
                        // If the time since the last transmission is greater than T, resend the packet
                        if (difftime(time(NULL), SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM]) > T)
                        {
#ifdef DLOG
                            pink();
                            printf("S Thread: Timeout! Resending packet %d for table entry %d\n", (next_to_send + j) % MAX_SEQ_NUM, i);
                            reset();
#endif
                            SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM] = time(NULL);
                        }
                        else
                        {
                            continue;
                        }
                    }
                    // If the packet is not sent yet, send the packet
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 1)
                    {
#ifdef DLOG
                        pink();
                        printf("S Thread: Sending packet %d for table entry %d\n", (next_to_send + j) % MAX_SEQ_NUM, i);
                        reset();
#endif
                        SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM] = time(NULL);
                    }
                    SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] = 2;
                    send_msg(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, (next_to_send + j) % MAX_SEQ_NUM, SM[i].send_buff[(next_to_send_index + j) % SEND_BUFF_SIZE]);
                }
                Up(sem_row[i]);
            }
        }
        sleep(sleep_time); // Sleep for some time before going to the next iteration
    }
}

/**
 * The main function initializes the shared memory and semaphores.
 * It creates the Garbage Collector, R_Thread, and S_Thread threads.
 * It also handles the socket initialization and binding requests.
 */
int main()
{
    srand(time(NULL) % getpid());
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    // Initialize the shared memory and semaphores
    key_t key_;
    key_ = ftok("/usr/bin", 1);
    shmid = shmget(key_, N * sizeof(struct shared_memory), 0666 | IPC_CREAT);
    SM = (struct shared_memory *)shmat(shmid, (void *)0, 0);
    key_t key_1 = ftok("/usr/bin", 2);
    shmid_1 = shmget(key_1, sizeof(struct SOCK_INFO), 0666 | IPC_CREAT);
    SI = (struct SOCK_INFO *)shmat(shmid_1, (void *)0, 0);
    SI->errorno = 0;
    inet_aton("0.0.0.0", &SI->IP);
    SI->port = 0;
    SI->sock_id = 0;
    key_t key_table_lock = ftok("/usr/bin", 3);
    table_lock = semget(key_table_lock, 1, 0666 | IPC_CREAT);
    semctl(table_lock, 0, SETVAL, 1);
    key_t key_sem_1 = ftok("/usr/bin", 4);
    sem_1 = semget(key_sem_1, 1, 0666 | IPC_CREAT);
    semctl(sem_1, 0, SETVAL, 0);
    key_t key_sem_2 = ftok("/usr/bin", 5);
    sem_2 = semget(key_sem_2, 1, 0666 | IPC_CREAT);
    semctl(sem_2, 0, SETVAL, 0);
    key_t key_sock_info_lock = ftok("/usr/bin", 6);
    sock_info_lock = semget(key_sock_info_lock, 1, 0666 | IPC_CREAT);
    semctl(sock_info_lock, 0, SETVAL, 1);
    for (int i = 0; i < N; i++)
    {
        key_t key_sem_row = ftok("/usr/bin", 7 + i);
        sem_row[i] = semget(key_sem_row, 1, 0666 | IPC_CREAT);
        semctl(sem_row[i], 0, SETVAL, 1);
    }

    Down(table_lock);
    for (int i = 0; i < N; i++)
    {
        SM[i].is_available = 1;
    }
    Up(table_lock);
    pthread_t G, R, S;

    // Create the Garbage Collector, R_Thread, and S_Thread threads
    pthread_create(&G, NULL, Garbage_Collector, NULL);
    pthread_create(&R, NULL, R_Thread, NULL);
    pthread_create(&S, NULL, S_Thread, NULL);

#ifdef DLOG
    grey();
    printf("R, S and Garbage Collector Threads created\n");
    reset();
#endif

    // Signal handlers for SIGQUIT and SIGINT
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);
    while (1)
    {
        Down(sem_1);
        Down(sock_info_lock);
        // m_socket request
        if (SI->port == 0 && SI->sock_id == 0)
        {
#ifdef DLOG
            grey();
            printf("Init: Socket request received\n");
            reset();
#endif
            int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (udp_fd < 0)
            {
                SI->errorno = errno;
                SI->sock_id = -1;
#ifdef DLOG
                red();
                printf("Init: Socket creation failed\n");
                reset();
#endif
                Up(sem_2);
                Up(sock_info_lock);
                continue;
            }
            SI->sock_id = udp_fd;
#ifdef DLOG
            green();
            printf("Init: Socket created successfully\n");
            reset();
#endif
            Up(sem_2);
            Up(sock_info_lock);
        }
        // m_bind request
        else
        {
#ifdef DLOG
            grey();
            printf("Init: Bind request received for table entry %d\n", SI->sock_id);
            reset();
#endif
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = SI->port;
            addr.sin_addr = SI->IP;
            int bind_status = bind(SI->sock_id, (struct sockaddr *)&addr, sizeof(addr));
            if (bind_status < 0)
            {
#ifdef DLOG
                red();
                printf("Init: Bind failed for socket %d\n", SI->sock_id);
                reset();
#endif
                SI->errorno = errno;
                SI->sock_id = -1;

                Up(sem_2);
                Up(sock_info_lock);
                continue;
            }
            else
            {
#ifdef DLOG
                green();
                printf("Init: Bind successful for socket %d\n", SI->sock_id);
                reset();
#endif
                Up(sem_2);
                Up(sock_info_lock);
            }
        }
    }
    pthread_join(G, NULL);
    pthread_join(R, NULL);
    pthread_join(S, NULL);
    exit(EXIT_SUCCESS);
}