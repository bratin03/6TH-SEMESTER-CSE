#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#define N 10
#define MAX_LEN 21
#define PORT 8181
#define MAXV 500

pthread_mutex_t t1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t t2 = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    char f_name[MAX_LEN];
    char l_name[MAX_LEN];
} candidate;

typedef struct
{
    int port;
    char ip_addr[100];
    int vote;
} voter;

int vote_count = 0;

candidate tableC[N];
voter tableV[500];

int n;

typedef struct
{
    int fd;
    struct sockaddr_in cli;
} thread_param;

void *checker(void *arg)
{
    int votes[n];
    while (1)
    {
        sleep(5);
        memset(votes, 0, sizeof(votes));
        pthread_mutex_lock(&t1);
        for (int i = 0; i < vote_count; i++)
        {
            votes[tableV[i].vote - 1]++;
        }
        pthread_mutex_unlock(&t1);
        printf("---------------------------------------------------\n");
        for (int i = 0; i < n; i++)
        {
            printf("%s %s received %d votes\n", tableC[i].f_name, tableC[i].l_name, votes[i]);
        }
        printf("---------------------------------------------------\n");
    }
}

void *voter_thread(void *arg)
{
    thread_param *client = (thread_param *)arg;
    int fd = client->fd;
    char *ip = inet_ntoa(client->cli.sin_addr);
    int port = ntohs(client->cli.sin_port);
    pthread_mutex_lock(&t1);
    for (int i = 0; i < vote_count; i++)
    {
        if (strcmp(ip, tableV[i].ip_addr) == 0 && port == tableV[i].port)
        {
            close(fd);
            pthread_mutex_unlock(&t1);
            pthread_exit(NULL);
        }
    }
    pthread_mutex_unlock(&t1);
    for (int i = 0; i < n; i++)
    {
        int x = htonl(i + 1);
        int ret = send(fd, (void *)&x, sizeof(x), 0);
        if (ret < 0)
        {
            perror("send");
            exit(1);
        }
        char sendbuf[MAX_LEN];
        strcpy(sendbuf, tableC[i].f_name);
        send(fd, (void *)sendbuf, strlen(tableC[i].f_name) + 1, 0);
        strcpy(sendbuf, tableC[i].l_name);
        send(fd, (void *)sendbuf, strlen(tableC[i].l_name) + 1, 0);
    }
    int x = htonl(0);
    int ret = send(fd, (void *)&x, sizeof(x), 0);
    char c = '\0';
    send(fd, (void *)&c, 1, 0);
    fd_set readfd;
    FD_ZERO(&readfd);
    FD_SET(fd, &readfd);
    struct timeval t;
    t.tv_sec = 60;
    ret = select(fd + 1, &readfd, 0, 0, &t);
    if (ret == 0)
    {
        close(fd);
        pthread_exit(NULL);
    }
    recv(fd, (void *)&x, sizeof(x), 0);
    int vote = ntohl(x);
    if (vote > n)
    {
        close(fd);
        pthread_exit(NULL);
    }
    else
    {
        pthread_mutex_lock(&t1);
        strcpy(tableV[vote_count].ip_addr, ip);
        tableV[vote_count].port = port;
        tableV[vote_count].vote = vote;
        vote_count++;
        printf("Client <%s,%d> voted for <%s %s>\n", ip, port, tableC[vote - 1].f_name, tableC[vote - 1].l_name);
        pthread_mutex_unlock(&t1);
        char *str = "Vote recorded successfully";
        send(fd, (void *)str, strlen(str) + 1, 0);
        close(fd);
        pthread_exit(NULL);
    }
}

int main()
{
    printf("Enter number of Candidates: ");
    scanf("%d", &n);
    getchar();
    char *read;
    for (int i = 0; i < n; i++)
    {
        printf("Enter the First Name and Last Name of %d candidate: (Separated by Space) ", i + 1);
        size_t len = 0;
        getline(&read, &len, stdin);
        read[len] = '\0';
        char *delim = strtok(read, " ");
        char *delim2 = strtok(NULL, " ");
        if (delim != NULL)
        {
            strcpy(tableC[i].f_name, delim);
        }
        if (delim2 != NULL)
        {
            strcpy(tableC[i].l_name, delim2);
            tableC[i].l_name[strlen(tableC[i].l_name)-1] = '\0';
        }
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    serv.sin_family = AF_INET;
    bind(sockfd, (const struct sockaddr *)&serv, sizeof(serv));
    listen(sockfd, 5);
    printf("Listening For Icoming Connections:\n");
    pthread_t tid;
    pthread_create(&tid, NULL, checker, NULL);
    while (1)
    {
        struct sockaddr_in cli;
        socklen_t cli_len;
        int newfd = accept(sockfd, (struct sockaddr *)&cli, &cli_len);
        if (newfd < 0)
        {
            perror("accept");
            exit(1);
        }
        thread_param *new_param = (thread_param *)malloc(sizeof(thread_param));
        new_param->cli = cli;
        new_param->fd = newfd;
        pthread_t tid;
        pthread_create(&tid, NULL, voter_thread, (void *)new_param);
    }
}
