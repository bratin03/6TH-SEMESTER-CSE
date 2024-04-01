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
#include <time.h>
#include <signal.h>

#define PROB 0.1

#define DEBUG1
#define DEBUG2
#define DEBUG3
#define DEBUG4

#define MAX_PAGE_PER_PROCESS 1000

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

int pidscheduler;
int pidmmu;

void sighand(int signum)
{
    if (signum == SIGINT)
    {
        // kill scheduler,mmu and all the processes
        kill(pidscheduler, SIGINT);
        kill(pidmmu, SIGINT);
        exit(1);
    }
}

typedef struct SM1
{
    int pid; // process id
    int mi;  // number of required pages
    int fi;  // number of frames allocated
    int pages[MAX_PAGE_PER_PROCESS][3];
    int totalpagefaults;
    int totalillegalaccess;
} SM1;

int main()
{
    srand(time(0));

    struct sembuf pop, vop;
    pop.sem_num = 0;
    pop.sem_op = -1;
    pop.sem_flg = 0;
    vop.sem_num = 0;
    vop.sem_op = 1;
    vop.sem_flg = 0;

    int k, m, f;
    printf("Enter the number of processes: ");
    scanf("%d", &k);
    printf("Enter the Virtual address space - Maximum number of pages required per process: ");
    scanf("%d", &f);
    printf("Enter Physical address space - Total number of frames: ");
    scanf("%d", &m);

#ifdef DEBUG1
    printf("Number of processes: %d\n", k);
    printf("Virtual address space - Maximum number of pages required per process: %d\n", f);
    printf("Physical address space - Total number of frames: %d\n", m);
#endif

    key_t key1 = ftok("/usr/bin", 1);
    int shmid1 = shmget(key1, k * sizeof(SM1), IPC_CREAT | 0666);
    if (shmid1 == -1)
    {
        perror("shmget");
        exit(1);
    }
    SM1 *sm1 = (SM1 *)shmat(shmid1, NULL, 0);
    for (int i = 0; i < k; i++)
    {
        sm1[i].pid = -1;
        sm1[i].mi = 0;
        sm1[i].fi = 0;
        sm1[i].totalpagefaults = 0;
        sm1[i].totalillegalaccess = 0;
    }

#ifdef DEBUG2
    printf("Shared memory 1 created for %d processes\n", k);
#endif

    key_t key2 = ftok("/usr/bin", 2);
    int shmid2 = shmget(key2, (f + 1) * sizeof(int), IPC_CREAT | 0666);
    if (shmid2 == -1)
    {
        perror("shmget");
        exit(1);
    }
    int *sm2 = (int *)shmat(shmid2, NULL, 0);
    for (int i = 0; i < f + 1; i++)
    {
        sm2[i] = 1;
    }
    sm2[f] = -1;
}
