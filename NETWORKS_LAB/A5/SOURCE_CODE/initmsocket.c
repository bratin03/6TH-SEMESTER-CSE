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

#define VERBOSE

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

void reset()
{
    printf("\033[0m");
}

#define Down(s) semop(s, &pop, 1)
#define Up(s) semop(s, &vop, 1)

int shmid, shmid_1;
struct shared_memory *SM;
struct SOCK_INFO *SI;
int table_lock, sem_1, sem_2, sock_info_lock;
int sem_row[N];
struct sembuf pop, vop;

void sigchld_handler(int signo)
{
    printf("\n\nInit:\nSignal received %s\n", strsignal(signo));
    printf("Clearing Shared Memory\n");
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
    printf("Shared Memory Cleared\n");
    printf("Exiting\n");
    exit(0);
}

void decimal_to_binary(int decimal, char *buffer)
{
    // Mask to extract each bit
    int mask = 1 << 3;

    for (int i = 0; i < 4; i++)
    {
        // Extract the ith bit
        int bit = (decimal & mask) ? 1 : 0;

        // Convert the bit to character '0' or '1' and store it in the buffer
        buffer[i] = bit + '0';

        // Shift the mask to the right for the next bit
        mask >>= 1;
    }

    // Null-terminate the buffer
    buffer[4] = '\0';
}

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

void *Garbage_Collector()
{
    while (1)
    {
        Down(table_lock);
// for (int i = 0; i < N; i++)
// {
//     if (SM[i].is_available == 0)
//     {

//         if (kill(SM[i].pid, 0) == -1)
//         {
//             close(SM[i].src_sock);
//             SM[i].is_available = 1;
//         }
//     }
// }
#ifdef VERBOSE
        pink();
        for (int i = 0; i <= 0; i++)
        {

            printf("i = %d\n", i);
            printf("SM[%d].is_available = %d SM[%d].pid = %d\n SM[%d].src_sock = %d\n", i, SM[i].is_available, i, SM[i].pid, i, SM[i].src_sock);
            printf("\n");
            for (int j = 0; j < RECV_BUFF_SIZE; j++)
            {
                printf("SM[%d].recv_buff[%d] = %s SM[%d].recv_buff.buffer_is_valid[%d] = %d\n", i, j, SM[i].recv_buff[j], i, j, SM[i].receive_window.buffer_is_valid[j]);
                printf("\n");
            }
        }
        reset();
#endif
        Up(table_lock);
        sleep(T / 5);
    }
}

void *R_Thread()
{
    fd_set readfds;
    struct timeval tv;
    int max_fd = 0;

    int prev_empty[N] = {0};
    while (1)
    {
        max_fd = 0;
        FD_ZERO(&readfds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        Down(table_lock);
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                FD_SET(SM[i].src_sock, &readfds);
                if (SM[i].src_sock > max_fd)
                {
                    max_fd = SM[i].src_sock;
                }
            }
        }
        Up(table_lock);
        int ret = select(max_fd + 1, &readfds, NULL, NULL, &tv);
        if (ret == -1)
        {
            perror("select");
            exit(1);
        }
        if (ret == 0)
        {
            for (int i = 0; i < N; i++)
            {
                if (prev_empty[i] == 1 && SM[i].is_available == 0 && SM[i].receive_window.nospace == 0)
                {
                    prev_empty[i] = 0;
                    char ack[MSG_SIZE + 9];
                    ack[0] = '1';

                    Down(sem_row[i]);
                    int current_window_size = 0;
                    int last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                    for (int j = 0; j < RECV_BUFF_SIZE; j++)
                    {
                        if (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j) % MAX_SEQ_NUM]] == 1)
                        {
                            break;
                        }
                        current_window_size++;
                    }
                    SM[i].receive_window.window_size = current_window_size;
                    orange();
                    for (int j = 0; j < current_window_size; j++)
                    {

                        SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + (j + 1)) % RECV_BUFF_SIZE;
                        printf("SM[i].receive_window.seq_buf_index_map[%d] = %d\n", (last_inorder_packet + j + 1) % MAX_SEQ_NUM, SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM]);
                    }
                    reset();

                    decimal_to_binary(SM[i].receive_window.last_inorder_packet, ack + 1);
                    decimal_to_binary(SM[i].receive_window.window_size, ack + 5);
                    struct sockaddr_in dest_addr;
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_port = SM[i].dest_port;
                    dest_addr.sin_addr = SM[i].dest_ip;
                    int ret = sendto(SM[i].src_sock, ack, MSG_SIZE + 9, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                    if (ret == -1)
                    {
                        perror("sendto");
                        Up(sem_row[i]);
                        exit(1);
                    }
                    green();
                    printf("Sending Ack indicatating space available %s\n", ack);
                    reset();
                    green();
                    printf("Sent Ack: %s\n", ack);
                    reset();
                    Up(sem_row[i]);
                }
            }
            continue;
        }
        for (int i = 0; i < N; i++)
        {

            if (SM[i].is_available == 0)
            {

                if (FD_ISSET(SM[i].src_sock, &readfds))
                {
                    green();
                    printf("Received data on %d\n", i);
                    struct sockaddr_in src_addr;
                    socklen_t src_addr_len = sizeof(src_addr);
                    char RECVMSG[MSG_SIZE + 5];
                    int ret = recvfrom(SM[i].src_sock, RECVMSG, MSG_SIZE + 5, 0, (struct sockaddr *)&src_addr, &src_addr_len);
                    printf("Received: %s\n", RECVMSG);
                    reset();
                    if (ret == -1)
                    {
                        perror("recvfrom");
                        exit(1);
                    }
                    char id_bit = RECVMSG[0];
                    if (id_bit == '0')
                    {
                        char seq_num[5];
                        strncpy(seq_num, RECVMSG + 1, 4);
                        seq_num[4] = '\0';
                        int seq_num_int = binary_to_decimal(seq_num, 4);
                        Down(sem_row[i]);
                        int last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                        int window_size = SM[i].receive_window.window_size;
                        int next_to_deliver = (last_inorder_packet + 1) % MAX_SEQ_NUM;
                        green();
                        printf("seq_num_int: %d last_inorder_packet: %d window_size: %d next_to_deliver: %d\n", seq_num_int, last_inorder_packet, window_size, next_to_deliver);
                        reset();
                        if (((seq_num_int - last_inorder_packet + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {

                            int buffer_index = SM[i].receive_window.seq_buf_index_map[seq_num_int];
                            green();
                            printf("Buffer Index: %d\n", buffer_index);
                            reset();
                            if (SM[i].receive_window.buffer_is_valid[buffer_index] == 0)
                            {
                                strcpy(SM[i].recv_buff[buffer_index], RECVMSG + 5);
                                SM[i].receive_window.buffer_is_valid[buffer_index] = 1;
                            }
                            if (seq_num_int == next_to_deliver)
                            {

                                int next_index = (last_inorder_packet + 1) % MAX_SEQ_NUM;
                                while (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[next_index]] == 1 && (((next_index - last_inorder_packet + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size))
                                {
                                    green();
                                    printf("Next Index: %d\n", next_index);
                                    printf("SM[i].receive_window.seq_buf_index_map[next_index]: %d\n", SM[i].receive_window.seq_buf_index_map[next_index]);
                                    printf("SM[i].recv_buff.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[next_index]]: %d\n", SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[next_index]]);
                                    reset();
                                    SM[i].receive_window.last_inorder_packet = next_index;
                                    next_index = (next_index + 1) % MAX_SEQ_NUM;
                                }
                                last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                                int new_window_size = 0;
                                for (int j = 0; j < window_size; j++)
                                {
                                    if (SM[i].receive_window.buffer_is_valid[(last_inorder_packet + j + 1) % MAX_SEQ_NUM] == 1)
                                    {
                                        break;
                                    }
                                    new_window_size++;
                                }
                                for (int j = 0; j < new_window_size; j++)
                                {
                                    SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + (j + 1)) % RECV_BUFF_SIZE;
                                    orange();
                                    printf("SM[i].receive_window.seq_buf_index_map[%d] = %d\n", (last_inorder_packet + j + 1) % MAX_SEQ_NUM, SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM]);
                                    reset();
                                }
                                SM[i].receive_window.window_size = new_window_size;
                                if (new_window_size == 0)
                                {
                                    SM[i].receive_window.nospace = 1;
                                    prev_empty[i] = 1;
                                }
                            }
                            green();
                            printf("\nLast Inorder Packet: %d\n\n", SM[i].receive_window.last_inorder_packet);
                            reset();
                            char ack[MSG_SIZE + 9];
                            ack[0] = '1';
                            decimal_to_binary(SM[i].receive_window.last_inorder_packet, ack + 1);
                            decimal_to_binary(SM[i].receive_window.window_size, ack + 5);
                            struct sockaddr_in dest_addr;
                            dest_addr.sin_family = AF_INET;
                            dest_addr.sin_port = SM[i].dest_port;
                            dest_addr.sin_addr = SM[i].dest_ip;
                            green();
                            printf("Sending Normal Ack: %s\n", ack);
                            reset();
                            int ret = sendto(SM[i].src_sock, ack, MSG_SIZE + 9, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                            if (ret == -1)
                            {
                                perror("sendto");
                                Up(sem_row[i]);
                                exit(1);
                            }
                            Up(sem_row[i]);
                        }
                        else if (SM[i].receive_window.nospace == 1)
                        {
                            char ack[MSG_SIZE + 9];
                            ack[0] = '1';
                            // Down(table_lock);
                            decimal_to_binary(SM[i].receive_window.last_inorder_packet, ack + 1);
                            decimal_to_binary(SM[i].receive_window.window_size, ack + 5);
                            struct sockaddr_in dest_addr;
                            dest_addr.sin_family = AF_INET;
                            dest_addr.sin_port = SM[i].dest_port;
                            dest_addr.sin_addr = SM[i].dest_ip;
                            int ret = sendto(SM[i].src_sock, ack, MSG_SIZE + 9, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                            if (ret == -1)
                            {
                                perror("sendto");
                                Up(sem_row[i]);
                                exit(1);
                            }
                            green();
                            printf("Sending Ack indicating no space: %s\n", ack);
                            reset();
                            Up(sem_row[i]);
                        }
                    }
                    else if (id_bit == '1')
                    {
                        // Ack
                        char seq_num[5];
                        strncpy(seq_num, RECVMSG + 1, 4);
                        seq_num[4] = '\0';
                        int ack_num = binary_to_decimal(seq_num, 4);
                        char window_size_update[5];
                        strncpy(window_size_update, RECVMSG + 5, 4);
                        window_size_update[4] = '\0';
                        int new_window_size = binary_to_decimal(window_size_update, 4);
                        Down(sem_row[i]);
                        int last_seq_ack = SM[i].send_window.last_seq_ack;
                        int window_size = SM[i].send_window.window_size;
                        int next_to_send = (last_seq_ack + 1) % MAX_SEQ_NUM;
                        green();
                        printf("Ack Num: %d Last Seq Ack: %d Window Size: %d Next to Send: %d New Window Size: %d\n", ack_num, last_seq_ack, window_size, next_to_send, new_window_size);
                        reset();
                        if (((ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {
                            if (ack_num == last_seq_ack)
                            {
                            }
                            for (int j = 0; j < (ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM; j++)
                            {
                                SM[i].send_window.buffer_is_valid[SM[i].send_window.seq_buf_index_map[(last_seq_ack + j + 1) % MAX_SEQ_NUM]] = 0;
                            }
                        }
                        SM[i].send_window.last_seq_ack = ack_num;
                        SM[i].send_window.window_size = new_window_size;
                        for (int j = 0; j < new_window_size; j++)
                        {
                            SM[i].send_window.seq_buf_index_map[(ack_num + j + 1) % MAX_SEQ_NUM] = (SM[i].send_window.seq_buf_index_map[ack_num] + (j + 1)) % SEND_BUFF_SIZE;
                        }

                        Up(sem_row[i]);
                    }
                }
            }
        }
    }
}

void *S_Thread()
{
    int sleep_time = T / 2 - (1 - T % 2);
    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                Down(table_lock);
                int next_to_send = (SM[i].send_window.last_seq_ack + 1) % MAX_SEQ_NUM;
                if (i == 1)
                {
                    red();
                    printf("Next to send: %d\n", next_to_send);
                    reset();
                }
                int next_to_send_index = SM[i].send_window.seq_buf_index_map[next_to_send];
                int window_size = SM[i].send_window.window_size;
                if (i == 1)
                {
                    red();
                    printf("Next to send index: %d Window Size: %d\n", next_to_send_index, window_size);
                    reset();
                }
                for (int j = 0; j < window_size; j++)
                {
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 0)
                    {
                        continue;
                    }
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 2)
                    {
                        if ((time(NULL) - SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM]) > T)
                        {
                            orange();
                            printf("Time Now: %ld, Time Last Sent: %ld\n", time(NULL), SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM]);
                            printf("Timeout difference: %ld\n", time(NULL) - SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM]);
                            reset();
                            SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM] = time(NULL);
                        }
                        else
                        {
                            continue;
                        }
                    }
                    if (SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] == 1)
                    {
                        SM[i].send_window.timeout[(next_to_send + j) % MAX_SEQ_NUM] = time(NULL);
                    }
                    SM[i].send_window.buffer_is_valid[(next_to_send_index + j) % SEND_BUFF_SIZE] = 2;
                    struct sockaddr_in dest_addr;
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_port = SM[i].dest_port;
                    dest_addr.sin_addr = SM[i].dest_ip;
                    char SENDMSG[MSG_SIZE + 5];
                    SENDMSG[0] = '0';
                    decimal_to_binary((next_to_send + j) % MAX_SEQ_NUM, SENDMSG + 1);
                    strcat(SENDMSG + 5, SM[i].send_buff[(next_to_send_index + j) % SEND_BUFF_SIZE]);
                    int ret = sendto(SM[i].src_sock, SENDMSG, MSG_SIZE + 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                    if (ret == -1)
                    {
                        perror("sendto");
                        Up(table_lock);
                        exit(1);
                    }
                    red();
                    printf("Sent: %s\n", SENDMSG);
                    reset();
                }
                Up(table_lock);
            }
        }
        sleep(sleep_time);
    }
}

int main()
{
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

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
    // pthread_mutex_unlock(&table_lock);
    pthread_t G, R, S;
    pthread_create(&G, NULL, Garbage_Collector, NULL);
    pthread_create(&R, NULL, R_Thread, NULL);
    pthread_create(&S, NULL, S_Thread, NULL);
    signal(SIGTERM, sigchld_handler);
    signal(SIGINT, sigchld_handler);
    signal(SIGQUIT, sigchld_handler);
    while (1)
    {
        Down(sem_1);
        // sem_wait(&sem1);

        Down(sock_info_lock);
        // pthread_mutex_lock(&sock_info_lock);
        if (SI->port == 0 && SI->sock_id == 0)
        {
            int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
            // printf("UDP FD: %d\n", udp_fd);
            if (udp_fd < 0)
            {
                SI->errorno = errno;
                SI->sock_id = -1;
                // #ifdef VERBOSE
                //                 printf("Signal sent\n");
                // #endif
                Up(sem_2);
                // sem_post(&sem2);
                Up(sock_info_lock);
                // pthread_mutex_unlock(&sock_info_lock);
                continue;
            }
            SI->sock_id = udp_fd;
            // #ifdef VERBOSE
            //             printf("Signal sent\n");
            // #endif
            Up(sem_2);
            // sem_post(&sem2);
            Up(sock_info_lock);
            // pthread_mutex_unlock(&sock_info_lock);
        }
        else
        {

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = SI->port;
            addr.sin_addr = SI->IP;
            int bind_status = bind(SI->sock_id, (struct sockaddr *)&addr, sizeof(addr));
            if (bind_status < 0)
            {
                SI->errorno = errno;
                SI->sock_id = -1;
                Up(sem_2);
                Up(sock_info_lock);
                continue;
            }
            else
            {
                Up(sem_2);
                Up(sock_info_lock);
            }
        }
    }
    pthread_join(G, NULL);
}