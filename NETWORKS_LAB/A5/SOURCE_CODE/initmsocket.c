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

// Define DLOG to enable the server logs
#define DLOG

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

#define Down(s) semop(s, &pop, 1)
#define Up(s) semop(s, &vop, 1)

int shmid, shmid_1;
struct shared_memory *SM;
struct SOCK_INFO *SI;
int table_lock, sem_1, sem_2, sock_info_lock;
int sem_row[N];
struct sembuf pop, vop;
int m_close_requested[N] = {0};
int m_close_retry_count[N] = {0};

void sigchld_handler(int signo)
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
    exit(EXIT_SUCCESS);
}

void decimal_to_binary(int decimal, char *buffer)
{
    int mask = 1 << 3;

    for (int i = 0; i < 4; i++)
    {

        int bit = (decimal & mask) ? 1 : 0;

        buffer[i] = bit + '0';

        mask >>= 1;
    }

    buffer[4] = '\0';
}

void send_ack(int sock, struct in_addr dest_ip, int dest_port, int seq_num, int window_size)
{
    char ack[MSG_SIZE + 9];
    ack[0] = '1';
    decimal_to_binary(seq_num, ack + 1);
    decimal_to_binary(window_size, ack + 5);
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dest_port;
    dest_addr.sin_addr = dest_ip;
    int ret = sendto(sock, ack, MSG_SIZE + 9, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
#ifdef DLOG
    grey();
    printf("Ack: %s\n", ack);
    reset();
#endif
}

void send_msg(int sock, struct in_addr dest_ip, int dest_port, int seq_num, char *msg)
{
    char send_msg[MSG_SIZE + 5];
    send_msg[0] = '0';
    decimal_to_binary(seq_num, send_msg + 1);
    strcat(send_msg + 5, msg);
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dest_port;
    dest_addr.sin_addr = dest_ip;
    int ret = sendto(sock, send_msg, MSG_SIZE + 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
#ifdef DLOG
    cyan();
    printf("Msg: %s\n", send_msg);
    reset();
#endif
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
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                Down(sem_row[i]);
                if (m_close_requested[i] == 1)
                {
                    int to_send = 0;
                    for (int k = 0; k < SEND_BUFF_SIZE; k++)
                    {
                        if (SM[i].send_window.buffer_is_valid[k] != 0)
                        {
                            to_send = 1;
                            m_close_retry_count[i]++;
                            break;
                        }
                    }
                    if (to_send == 0 || m_close_retry_count[i] > MAX_TRIES_AFTER_CLOSE)
                    {
                        SM[i].is_available = 1;
                        m_close_requested[i] = 0;
                        if (close(SM[i].src_sock) == -1)
                        {
                            perror("close");
                            exit(EXIT_FAILURE);
                        }
#ifdef DLOG
                        red();
                        printf("Garbage Collector: Closed socket %d\n", i);
                        if (m_close_retry_count[i] > MAX_TRIES_AFTER_CLOSE)
                        {
                            printf("Garbage Collector: Max tries after close reached for table entry %d\n", i);
                        }
                        reset();
#endif
                        m_close_retry_count[i] = 0;
                    }
                    Up(sem_row[i]);
                    continue;
                }
                if (kill(SM[i].pid, 0) == -1)
                {
                    if (errno == ESRCH)
                    {
                        int to_send = 0;
                        for (int k = 0; k < SEND_BUFF_SIZE; k++)
                        {
                            if (SM[i].send_window.buffer_is_valid[k] != 0)
                            {
                                to_send = 1;
                                m_close_retry_count[i]++;
                                break;
                            }
                        }
                        if (to_send == 0 || m_close_retry_count[i] > MAX_TRIES_AFTER_CLOSE)
                        {
                            SM[i].is_available = 1;
                            if (m_close_requested[i] == 1)
                            {
                                m_close_requested[i] = 0;
                            }
                            if (close(SM[i].src_sock) == -1)
                            {
                                perror("close");
                                exit(EXIT_FAILURE);
                            }
#ifdef DLOG
                            red();
                            printf("Garbage Collector: Closed socket %d\n", i);
                            if (m_close_retry_count[i] > MAX_TRIES_AFTER_CLOSE)
                            {
                                printf("Garbage Collector: Max tries after close reached for table entry %d\n", i);
                            }
                            reset();
#endif
                            m_close_retry_count[i] = 0;
                        }
                    }
                    else
                    {
                        perror("kill");
                        exit(EXIT_FAILURE);
                    }
                }
                Up(sem_row[i]);
            }
        }
        sleep(2 * T);
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
            exit(EXIT_FAILURE);
        }
        if (ret == 0)
        {
            for (int i = 0; i < N; i++)
            {
                if (prev_empty[i] == 1 && SM[i].is_available == 0 && SM[i].receive_window.nospace == 0)
                {

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

                    for (int j = 0; j < RECV_BUFF_SIZE; j++)
                    {
                        SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + j + 1) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + (j + 1)) % RECV_BUFF_SIZE;
                    }
                    send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
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

                if (FD_ISSET(SM[i].src_sock, &readfds))
                {
                    struct sockaddr_in src_addr;
                    socklen_t src_addr_len = sizeof(src_addr);
                    char msg[MSG_SIZE + 5];
                    int ret = recvfrom(SM[i].src_sock, msg, MSG_SIZE + 5, 0, (struct sockaddr *)&src_addr, &src_addr_len);
#ifdef DLOG
                    orange();
                    printf("R Thread: Received %s for table entry %d\n", msg, i);
                    reset();
#endif
                    if (ret == -1)
                    {
                        perror("recvfrom");
                        exit(EXIT_FAILURE);
                    }
                    if (dropMessage(P))
                    {
#ifdef DLOG
                        red();
                        printf("R Thread: Dropped packet for table entry %d. Content: %s\n", i, msg);
                        reset();
#endif
                        continue;
                    }
#ifdef DLOG
                    cyan();
                    printf("R Thread: Packet Not Dropped for table entry %d. Content: %s\n", i, msg);
                    reset();
#endif
                    char id_bit = msg[0];
                    if (id_bit == '0')
                    {
                        char seq_num[5];
                        strncpy(seq_num, msg + 1, 4);
                        seq_num[4] = '\0';
                        int seq_num_int = binary_to_decimal(seq_num, 4);
                        Down(sem_row[i]);
                        int last_inorder_packet = SM[i].receive_window.last_inorder_packet;
                        int window_size = SM[i].receive_window.window_size;
                        int next_to_deliver = (last_inorder_packet + 1) % MAX_SEQ_NUM;

                        if (seq_num_int == last_inorder_packet)
                        {
                            send_ack(SM[i].src_sock, src_addr.sin_addr, src_addr.sin_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            Up(sem_row[i]);
                            continue;
                        }

                        if (((seq_num_int - last_inorder_packet + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {

                            int buffer_index = SM[i].receive_window.seq_buf_index_map[seq_num_int];
                            if (SM[i].receive_window.buffer_is_valid[buffer_index] == 0)
                            {
                                strcpy(SM[i].recv_buff[buffer_index], msg + 5);
                                SM[i].receive_window.buffer_is_valid[buffer_index] = 1;
                            }
                            if (seq_num_int == next_to_deliver)
                            {
                                for (int j = 0; j <= RECV_BUFF_SIZE; j++)
                                {
                                    SM[i].receive_window.seq_buf_index_map[(next_to_deliver + j) % MAX_SEQ_NUM] = (SM[i].receive_window.seq_buf_index_map[next_to_deliver] + j) % RECV_BUFF_SIZE;
                                }

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

                                last_inorder_packet = seq_num_int;
                                while (SM[i].receive_window.buffer_is_valid[SM[i].receive_window.seq_buf_index_map[(last_inorder_packet + 1) % MAX_SEQ_NUM]] == 1 && (((last_inorder_packet + 1 - next_to_deliver + MAX_SEQ_NUM) % MAX_SEQ_NUM) < window_size))
                                {
                                    last_inorder_packet = (last_inorder_packet + 1) % MAX_SEQ_NUM;
                                }
                                int new_window_size = (buffer_index_outside_window - (SM[i].receive_window.seq_buf_index_map[last_inorder_packet] + 1) + RECV_BUFF_SIZE + outside_free_count) % RECV_BUFF_SIZE;
                                SM[i].receive_window.last_inorder_packet = last_inorder_packet;
                                SM[i].receive_window.window_size = new_window_size;
                                if (new_window_size == 0)
                                {
                                    SM[i].receive_window.nospace = 1;
                                    prev_empty[i] = 1;
                                }
                            }
#ifdef DLOG
                            green();
                            printf("R Thread: Received and stored packet %d for table entry %d\n", seq_num_int, i);
                            printf("Sending Ack for table entry %d. Ack No: %d Window Size: %d\n", i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            reset();
#endif
                            send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            Up(sem_row[i]);
                        }
                        else if (SM[i].receive_window.nospace == 1)
                        {
#ifdef DLOG
                            green();
                            printf("R Thread: Received packet %d for table entry %d. No space available. Sending Ack indicating space available for table entry %d. Ack No: %d Window Size: %d\n", seq_num_int, i, i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            printf("Sending Ack indicating no space available for table entry %d. Ack No: %d Window Size: %d\n", i, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);
                            reset();
#endif
                            send_ack(SM[i].src_sock, SM[i].dest_ip, SM[i].dest_port, SM[i].receive_window.last_inorder_packet, SM[i].receive_window.window_size);

                            Up(sem_row[i]);
                        }
                        else
                        {
                            Up(sem_row[i]);
                        }
                    }
                    else if (id_bit == '1')
                    {
                        // Ack
                        char seq_num[5];
                        strncpy(seq_num, msg + 1, 4);
                        seq_num[4] = '\0';
                        int ack_num = binary_to_decimal(seq_num, 4);
                        char window_size_update[5];
                        strncpy(window_size_update, msg + 5, 4);
                        window_size_update[4] = '\0';
                        int new_window_size = binary_to_decimal(window_size_update, 4);
                        Down(sem_row[i]);
                        int last_seq_ack = SM[i].send_window.last_seq_ack;
                        int window_size = SM[i].send_window.window_size;

                        if (((ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM) <= window_size)
                        {
                            if (ack_num == last_seq_ack)
                            {
                                // Do nothing
                            }
                            else
                            {
                                for (int j = 0; j < (ack_num - last_seq_ack + MAX_SEQ_NUM) % MAX_SEQ_NUM; j++)
                                {
                                    SM[i].send_window.buffer_is_valid[SM[i].send_window.seq_buf_index_map[(last_seq_ack + j + 1) % MAX_SEQ_NUM]] = 0;
                                }
                            }
                        }
                        SM[i].send_window.last_seq_ack = ack_num;
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

                        Up(sem_row[i]);
                    }
                }
            }
        }
    }
}

void *S_Thread()
{
    int sleep_time = floor(T / 2);
    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            if (SM[i].is_available == 0)
            {
                Down(table_lock);
                int next_to_send = (SM[i].send_window.last_seq_ack + 1) % MAX_SEQ_NUM;
                int next_to_send_index = SM[i].send_window.seq_buf_index_map[next_to_send];
                int window_size = SM[i].send_window.window_size;

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
                Up(table_lock);
            }
        }
        sleep(sleep_time);
    }
}

int main()
{
    srand(time(NULL) % getpid());
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
    pthread_t G, R, S;

    pthread_create(&G, NULL, Garbage_Collector, NULL);
    pthread_create(&R, NULL, R_Thread, NULL);
    pthread_create(&S, NULL, S_Thread, NULL);

#ifdef DLOG
    grey();
    printf("R, S and Garbage Collector Threads created\n");
    reset();
#endif

    signal(SIGTERM, sigchld_handler);
    signal(SIGINT, sigchld_handler);
    signal(SIGQUIT, sigchld_handler);
    while (1)
    {
        Down(sem_1);
        Down(sock_info_lock);
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
                SI->errorno = errno;
                SI->sock_id = -1;
#ifdef DLOG
                red();
                printf("Init: Bind failed for table entry %d\n", SI->sock_id);
                reset();
#endif
                Up(sem_2);
                Up(sock_info_lock);
                continue;
            }
            else
            {
#ifdef DLOG
                green();
                printf("Init: Bind successful for table entry %d\n", SI->sock_id);
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