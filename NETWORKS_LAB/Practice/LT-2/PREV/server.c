#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 8181

#define BUFF 1024
#define N 10
#define VOTERS 500
#define NAME 50

typedef struct
{
    char fn[NAME];
    char ln[NAME];
} CE;

typedef struct
{
    char ip[NAME];
    int port;
    int idx;
} VE;

typedef struct
{
    int sockfd;
    struct sockaddr_in cli;
} TP;

CE CT[N];
VE VT[VOTERS];

int V_Count = 0;
int n = 0;

pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;

void *checker()
{
    while (1)
    {
        sleep(30);
        pthread_mutex_lock(&table_lock);
        int vote_count[n];
        memset(vote_count, 0, sizeof(vote_count));
        for (int i = 0; i < V_Count; i++)
        {
            vote_count[VT[i].idx - 1]++;
        }
        pthread_mutex_unlock(&table_lock);
        for (int i = 0; i < n; i++)
        {
            printf("Candidate %d Name: %s %s Votes: %d\n", i + 1, CT[i].fn, CT[i].ln, vote_count[i]);
        }
    }
}

int table_check(struct sockaddr_in cli)
{
    pthread_mutex_lock(&table_lock);
    char *ip = inet_ntoa(cli.sin_addr);
    int port = ntohs(cli.sin_port);
    for (int i = 0; i < V_Count; i++)
    {
        if (VT[i].port == port && strcmp(VT[i].ip, ip) == 0)
        {
            pthread_mutex_unlock(&table_lock);
            return 1;
        }
    }
    pthread_mutex_unlock(&table_lock);
    return 0;
}

void writeInt(int fd, int n)
{
    n = htonl(n);
    send(fd, &n, sizeof(n), 0);
}

int readint(int fd)
{
    int n;
    recv(fd, &n, sizeof(n), 0);
    return ntohl(n);
}

void *voter(void *arg)
{
    TP *tp = (TP *)arg;
    int fd = tp->sockfd;
    struct sockaddr_in cli = tp->cli;
    if (table_check(cli))
    {
        close(fd);
        pthread_exit(0);
    }
    for (int i = 0; i < n; i++)
    {
        writeInt(fd, i + 1);
        char buffer[BUFF];
        sprintf(buffer, "%s %s", CT[i].fn, CT[i].ln);
        send(fd, buffer, strlen(buffer) + 1, 0);
    }
    writeInt(fd, 0);
    char c = '\0';
    send(fd, &c, 1, 0);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    struct timeval t;
    t.tv_sec = 60;
    t.tv_usec = 0;
    int ret = select(fd + 1, &readfds, 0, 0, &t);
    if (ret == 0)
    {
        close(fd);
        pthread_exit(0);
    }
    int vote = readint(fd);
    if (vote <= 0 || vote > n)
    {
        close(fd);
        pthread_exit(0);
    }
    pthread_mutex_lock(&table_lock);
    VT[V_Count].port = ntohs(cli.sin_port);
    char *ip = inet_ntoa(cli.sin_addr);
    strcpy(VT[V_Count].ip, ip);
    VT[V_Count].idx = vote;
    V_Count++;
    pthread_mutex_unlock(&table_lock);
    printf("Client %s, %d voted for %s %s\n", ip, ntohs(cli.sin_port), CT[vote - 1].fn, CT[vote - 1].ln);
    char retr[BUFF];
    strcpy(retr, "Vote recorded successfully");
    send(fd, retr, strlen(retr) + 1, 0);
    close(fd);
    pthread_exit(0);
}

int main()
{
    printf("Enter Number of Candidates: ");
    scanf("%d", &n);
    getchar();
    for (int i = 0; i < n; i++)
    {
        printf("Enter First Name and Last Name of candidate %d: ", i + 1);
        char buffer[BUFF];
        fgets(buffer, BUFF, stdin);
        char *fname = strtok(buffer, " \n");
        strcpy(CT[i].fn, fname);
        char *lname = strtok(NULL, " \n");
        strcpy(CT[i].ln, lname);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv, cli;
    int cli_len;
    memset(&serv, 0, sizeof(serv));
    memset(&cli, 0, sizeof(cli));
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(PORT);
    serv.sin_family = AF_INET;
    bind(sockfd, (const struct sockaddr *)&serv, sizeof(serv));

    listen(sockfd, 10);

    pthread_t tid;
    pthread_create(&tid, NULL, checker, NULL);

    while (1)
    {
        cli_len = sizeof(cli);
        int new = accept(sockfd, (struct sockaddr *)&cli, &cli_len);
        TP *tp = (TP *)malloc(sizeof(TP));
        tp->cli = cli;
        tp->sockfd = new;
        pthread_t cli_tid;
        pthread_create(&cli_tid, 0, voter, (void *)tp);
    }
}