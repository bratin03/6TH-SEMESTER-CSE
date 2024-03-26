#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SERVER_PORT 8080
#define MAXLINE 1024
#define MAXCLIENTS 10

int readint(int sockfd)
{
    int n;
    recv(sockfd, &n, sizeof(n), 0);
    int num = ntohl(n);
    return num;
}

void writeint(int sockfd, int n)
{
    int num = htonl(n);
    send(sockfd, &num, sizeof(num), 0);
}

int readTillEnd(int sockfd, char *buffer, char *end, int count)
{
    int i = 0;
    char c;
    while (1)
    {
        int ret = recv(sockfd, &c, sizeof(c), 0);
        if (ret == 0)
        {
            return 0;
        }
        else if (ret < 0)
        {
            return -1;
        }
        buffer[i++] = c;
        int isend = 1;
        if (i >= count)
        {
            for (int j = 0; j < count; j++)
            {
                if (buffer[i - count + j] != end[j])
                {
                    isend = 0;
                    break;
                }
            }
        }
        else
        {
            isend = 0;
        }
        if (isend)
        {
            break;
        }
    }
    buffer[i] = '\0';
    return i;
}

// #define CLIENT
#define SERVER
#define THREAD
// #define FORK
// #define FORK_SHARED

/*
Using Select
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(sockfd, &readfds);
select(sockfd + 1, &readfds, NULL, NULL, NULL);
int ret = FD_ISSET(sockfd, &readfds);
if (ret)
{
    // Read from sockfd
}
else if (ret == -1)
{
    // Error
    perror("Select Failed");
    exit(1);
}
else if (ret == 0)
{
    // Timeout
}
*/

#ifdef SERVER
int IP_PORT_checker(char *ip, int port, struct sockaddr_in *addr)
{
    char *addr_ip = inet_ntoa(addr->sin_addr);
    if (strcmp(ip, addr_ip) == 0 && port == ntohs(addr->sin_port))
    {
        return 1;
    }
    return 0;
}

typedef struct
{
    char *ip;
    int port;
} IP_PORT;

#ifdef THREAD
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
IP_PORT table_connect[MAXCLIENTS];
int table_connect_size = 0;
int IP_PORT_add(char *ip, int port)
{
    pthread_mutex_lock(&lock);
    if (table_connect_size < MAXCLIENTS)
    {
        table_connect[table_connect_size].ip = ip;
        table_connect[table_connect_size].port = port;
        table_connect_size++;
        pthread_mutex_unlock(&lock);
        return 1;
    }
    pthread_mutex_unlock(&lock);
    return 0;
}

int IP_PORT_table_checker(struct sockaddr_in *addr)
{
    pthread_mutex_lock(&lock);
    for (int i = 0; i < table_connect_size; i++)
    {
        if (IP_PORT_checker(table_connect[i].ip, table_connect[i].port, addr))
        {
            pthread_mutex_unlock(&lock);
            return 1;
        }
    }
    pthread_mutex_unlock(&lock);
    return 0;
}
#endif
#ifdef FORK_SHARED
struct sembuf pop, vop;
#define Down(s) semop(s, &pop, 1)
#define Up(s) semop(s, &vop, 1)
int shmid, semid, table_connect_id;
int *table_connect_size;
IP_PORT *table_connect;
int IP_PORT_add(char *ip, int port)
{
    Down(semid);
    if (*table_connect_size < MAXCLIENTS)
    {
        table_connect[*table_connect_size].ip = ip;
        table_connect[*table_connect_size].port = port;
        (*table_connect_size)++;
        Up(semid);
        return 1;
    }
    Up(semid);
    return 0;
}

int IP_PORT_table_checker(struct sockaddr_in *addr)
{
    Down(semid);
    for (int i = 0; i < *table_connect_size; i++)
    {
        if (IP_PORT_checker(table_connect[i].ip, table_connect[i].port, addr))
        {
            Up(semid);
            return 1;
        }
    }
    Up(semid);
    return 0;
}
#endif
#endif

#ifdef THREAD
typedef struct thread_args
{
    int sockfd;
    struct sockaddr_in client_addr;
} thread_args;

void *client_handler(void *args)
{
    thread_args *targs = (thread_args *)args;
    int sockfd = targs->sockfd;
    struct sockaddr_in client_addr = targs->client_addr;
    free(targs);
    // Handle client
    return NULL;
}
#endif

#ifdef FORK
void handle_client(int sockfd, struct sockaddr_in client_addr)
{
// Handle client
#ifdef FORK_SHARED
    shmdt(table_connect);
    shmdt(table_connect_size);
#endif
}
#endif

void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGQUIT)
    {
        printf("Received signal %d\n", signum);
#ifdef FORK_SHARED
        shmdt(table_connect);
        shmdt(table_connect_size);
        shmctl(shmid, IPC_RMID, NULL);
        shmctl(table_connect_id, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
#endif
#ifdef THREAD
        pthread_mutex_destroy(&lock);
#endif
        printf("Exiting\n");
        exit(0);
    }
}

int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
#ifdef CLIENT
    // Client code
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_aton("127.0.0.1", &server_addr.sin_addr);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to server");
        exit(1);
    }
#endif

#ifdef SERVER
    // Server code
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Error binding socket");
        exit(1);
    }
#ifdef FORK_SHARED
    pop.sem_num = 0;
    pop.sem_op = -1;
    pop.sem_flg = 0;
    vop.sem_num = 0;
    vop.sem_op = 1;
    vop.sem_flg = 0;
    key_t key = ftok("/usr/bin", 1);
    shmid = shmget(key, MAXCLIENTS * sizeof(IP_PORT), IPC_CREAT | 0666);
    table_connect = (IP_PORT *)shmat(shmid, NULL, 0);
    key_t key2 = ftok("/usr/bin", 2);
    semid = semget(key2, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);
    key_t key3 = ftok("/usr/bin", 3);
    table_connect_id = shmget(key3, sizeof(int), IPC_CREAT | 0666);
    table_connect_size = (int *)shmat(table_connect_id, NULL, 0);
    *table_connect_size = 0;
#endif
    listen(listenfd, 10);
    len = sizeof(cliaddr);
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        if (connfd < 0)
        {
            perror("Error accepting connection");
            exit(1);
        }
#ifdef THREAD
        pthread_t tid;
        thread_args *targs = (thread_args *)malloc(sizeof(thread_args));
        targs->sockfd = connfd;
        targs->client_addr = cliaddr;
        pthread_create(&tid, NULL, client_handler, (void *)targs);
#endif

#ifdef FORK
        if (fork() == 0)
        {
            close(listenfd);
            handle_client(connfd, cliaddr);
            close(connfd);
            exit(0);
        }
        close(connfd);
#endif
    }
#endif
}