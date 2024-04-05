#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

// #define DEBUG1
// #define DEBUG2
#define PENDING
#define PENDING2

#define PROB 0.01
#define MAX_PAGE_PER_PROCESS 1000

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int pidscheduler;
int pidmmu;

typedef struct SM1
{
    int pid;                                // process id
    int mi;                                 // number of required pages
    int fi;                                 // number of frames allocated
    int pagetable[MAX_PAGE_PER_PROCESS][3]; // page table
    int totalpagefaults;
    int totalillegalaccess;
} SM1;

// Shared Memory
int shmid1 = -1;
int shmid2 = -1;
int shmid3 = -1;
int *semid1 = NULL;
int semid2 = -1;
int semid3 = -1;
int semid4 = -1;
int msgid1 = -1;
int msgid2 = -1;
int msgid3 = -1;
SM1 *sm1 = NULL;
int *sm2 = NULL;
int *sm3 = NULL;

int k, m, f;

void sighand(int signum)
{
    if (signum == SIGINT || signum == SIGQUIT)
    {
// kill scheduler,mmu and all the processes
#ifdef PENDING
        kill(pidscheduler, SIGINT);
        kill(pidmmu, SIGINT);
#endif
        if (sm1 != NULL)
            shmdt(sm1);
        if (shmid1 != -1)
            shmctl(shmid1, IPC_RMID, NULL);
        if (sm2 != NULL)
            shmdt(sm2);
        if (shmid2 != -1)
            shmctl(shmid2, IPC_RMID, NULL);
        if (sm3 != NULL)
            shmdt(sm3);
        if (shmid3 != -1)
            shmctl(shmid3, IPC_RMID, NULL);
        if (semid1 != NULL)
        {
            for (int i = 0; i < k; i++)
            {
                semctl(semid1[i], 0, IPC_RMID, 0);
            }
        }
        if (semid2 != -1)
            semctl(semid2, 0, IPC_RMID, 0);
        if (semid3 != -1)
            semctl(semid3, 0, IPC_RMID, 0);
        if (semid4 != -1)
            semctl(semid4, 0, IPC_RMID, 0);
        if (msgid1 != -1)
            msgctl(msgid1, IPC_RMID, NULL);
        if (msgid2 != -1)
            msgctl(msgid2, IPC_RMID, NULL);
        if (msgid3 != -1)
            msgctl(msgid3, IPC_RMID, NULL);
        exit(1);
    }
}

int main()
{
    srand(time(0));
    signal(SIGINT, sighand);
    signal(SIGQUIT, sighand);
    struct sembuf pop = {0, -1, 0};
    struct sembuf vop = {0, 1, 0};

    printf("Enter the number of processes: ");
    scanf("%d", &k);
    printf("Enter the Virtual Address Space size: ");
    scanf("%d", &m);
    printf("Enter the Physical Address Space size: ");
    scanf("%d", &f);

    // page table for k processes
    key_t key = ftok("master.c", 1);
    shmid1 = shmget(key, k * sizeof(SM1), IPC_CREAT | 0666);
    sm1 = (SM1 *)shmat(shmid1, NULL, 0);

    for (int i = 0; i < k; i++)
    {
        sm1[i].totalpagefaults = 0;
        sm1[i].totalillegalaccess = 0;
        sm1[i].pid = 0;
        sm1[i].mi = 0;
        sm1[i].fi = 0;
    }

    // free frames list
    key = ftok("master.c", 20);
    shmid2 = shmget(key, (f + 1) * sizeof(int), IPC_CREAT | 0666);
    sm2 = (int *)shmat(shmid2, NULL, 0);

    // initialize the frames, 1 means free, 0 means occupied, -1 means end of list
    for (int i = 0; i < f; i++)
    {
        sm2[i] = 1;
    }
    sm2[f] = -1;

    // process to page mapping
    key = ftok("master.c", 3);
    shmid3 = shmget(key, k * sizeof(int), IPC_CREAT | 0666);
    sm3 = (int *)shmat(shmid3, NULL, 0);

    // initially no frames are allocated to any process
    for (int i = 0; i < k; i++)
    {
        sm3[i] = 0;
    }

    // semaphore 1 for Processes
    // key = ftok("master.c", 4);
    // int semid1 = semget(key, 1, IPC_CREAT | 0666);
    // semctl(semid1, 0, SETVAL, 0);

    // semaphores for Processes

    // semaphore 2 for Scheduler
    key = ftok("master.c", 4);
    semid2 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid2, 0, SETVAL, 0);

    // semaphore 3 for Memory Management Unit
    key = ftok("master.c", 5);
    semid3 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid3, 0, SETVAL, 0);

    // semaphore 4 for Master
    key = ftok("master.c", 6);
    semid4 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid4, 0, SETVAL, 0);

    // Message Queue 1 for Ready Queue
    key = ftok("master.c", 7);
    msgid1 = msgget(key, IPC_CREAT | 0666);

    // Message Queue 2 for Scheduler
    key = ftok("master.c", 8);
    msgid2 = msgget(key, IPC_CREAT | 0666);

    // Message Queue 3 for Memory Management Unit
    key = ftok("master.c", 9);
    msgid3 = msgget(key, IPC_CREAT | 0666);

    semid1 = (int *)malloc(k * sizeof(int));
    for (int i = 0; i < k; i++)
    {
        key = ftok("master.c", i + 10);
        semid1[i] = semget(key, 1, IPC_CREAT | 0666);
        semctl(semid1[i], 0, SETVAL, 0);
    }

    // convert msgid to string
    char msgid1str[10], msgid2str[10], msgid3str[10];
    sprintf(msgid1str, "%d", msgid1);
    sprintf(msgid2str, "%d", msgid2);
    sprintf(msgid3str, "%d", msgid3);

    // convert shmids to string
    char shmid1str[10], shmid2str[10], shmid3str[10];
    sprintf(shmid1str, "%d", shmid1);
    sprintf(shmid2str, "%d", shmid2);
    sprintf(shmid3str, "%d", shmid3);

    // pass number of processes for now
    char strk[10];
    sprintf(strk, "%d", k);

    // create Scheduler process, pass msgid1str and msgid2str
    pidscheduler = fork();
    if (pidscheduler == 0)
    {
        execlp("./sched", "./sched", msgid1str, msgid2str, strk, NULL);
        perror("execlp");
    }
#ifdef PENDING
    // create Memory Management Unit process, pass msgid2str and msgid3str, shmid1str and shmid2str
    pidmmu = fork();
    if (pidmmu == 0)
    {
        // execlp("xterm", "xterm", "-T", "Memory Management Unit", "-e", "./mmu", msgid2str, msgid3str, shmid1str, shmid2str, NULL);
        execlp("./mmu", "./mmu", msgid2str, msgid3str, shmid1str, shmid2str, NULL);
        perror("execlp");
    }
#endif

    int **refi = (int **)malloc((k) * sizeof(int *));
    char **refstr = (char **)malloc((k) * sizeof(char *));

    // initialize the Processes
    for (int i = 0; i < k; i++)
    {
        // generate random number of pages between 1 to m
        sm1[i].mi = rand() % m + 1;
        sm1[i].fi = 0;
// #ifdef DEBUG1
        printf("Process %d: %d pages\n", i, sm1[i].mi);
// #endif
        for (int j = 0; j < m; j++)
        {
            sm1[i].pagetable[j][0] = -1;      // no frame allocated
            sm1[i].pagetable[j][1] = 0;       // invalid
            sm1[i].pagetable[j][2] = INT_MAX; // timestamp
        }

        int y = 0;
        int x = rand() % (8 * sm1[i].mi + 1) + 2 * sm1[i].mi;
#ifdef DEBUG1
        printf("Process %d: %d references\n", i, x);
#endif
        refi[i] = (int *)malloc(x * sizeof(int));

        int *digits = (int *)malloc(x * sizeof(int));
        for (int j = 0; j < x; j++)
        {
            refi[i][j] = rand() % sm1[i].mi;
            if (((double)rand() / RAND_MAX) < PROB)
            {
                refi[i][j] = rand() % m;
#ifdef DEBUG2
                if (refi[i][j] >= sm1[i].mi)
                {
                    printf("\nIllegal Access by Process %d %d\n", i, refi[i][j]);
                }
#endif
            }
            int temp = refi[i][j];
            if (temp == 0)
            {
                digits[j] = 1;
            }
            else
            {
                digits[j] = 0;
                while (temp != 0)
                {
                    temp = temp / 10;
                    digits[j]++;
                }
            }
            y += digits[j];
            y++;
        }
        y += 2;
        refstr[i] = (char *)malloc(y * sizeof(char));
        memset(refstr[i], '\0', y);
        for (int j = 0; j < x; j++)
        {
            char temp[digits[j] + 1];
            sprintf(temp, "%d", refi[i][j]);
            strcat(refstr[i], temp);
            strcat(refstr[i], ".");
        }
        strcat(refstr[i], "$");
// #ifdef DEBUG1
        printf("Process %d: %s\n", i, refstr[i]);
// #endif
        free(digits);
    }

#ifdef PENDING2
    // create Processes
    for (int i = 0; i < k; i++)
    {
        usleep(250000);
        int pid = fork();
        if (pid != 0)
        {
            sm1[i].pid = pid;
        }
        else
        {
            // pass ref[i], msgid1str, msgid3str
            char processNo[10];
            sprintf(processNo, "%d", i);
            execlp("./process", "./process", refstr[i], msgid1str, msgid3str, processNo, NULL);
            perror("execlp");
        }
    }

    // wait for Scheduler to signal
    P(semid4);

    // terminate Scheduler
    kill(pidscheduler, SIGINT);

    // terminate Memory Management Unit
    kill(pidmmu, SIGINT);

#endif
    // detach and remove shared memory
    shmdt(sm1);
    shmctl(shmid1, IPC_RMID, NULL);

    shmdt(sm2);
    shmctl(shmid2, IPC_RMID, NULL);

    shmdt(sm3);
    shmctl(shmid3, IPC_RMID, NULL);

    // remove semaphores
    for (int i = 0; i < k; i++)
    {
        semctl(semid1[i], 0, IPC_RMID, 0);
    }
    semctl(semid2, 0, IPC_RMID, 0);
    semctl(semid3, 0, IPC_RMID, 0);
    semctl(semid4, 0, IPC_RMID, 0);

    // remove message queues
    msgctl(msgid1, IPC_RMID, NULL);
    msgctl(msgid2, IPC_RMID, NULL);
    msgctl(msgid3, IPC_RMID, NULL);

    return 0;
}