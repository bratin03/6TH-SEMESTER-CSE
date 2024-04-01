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
#include <time.h>

#define DEBUG1
#define DEBUG2
#define VERBOSE

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

typedef struct message1
{
    long type;
    int pid;
    int no;
} message1;

typedef struct message2
{
    long type;
    int pid;
} message2;

int main(int argc, char *argv[])
{
    struct sembuf pop = {0, -1, 0};
    struct sembuf vop = {0, 1, 0};

    if (argc != 4)
    {
        printf("Usage: %s <Message Queue 1 ID> <Message Queue 2 ID> <# Processes>\n", argv[0]);
        exit(1);
    }

#ifdef DEBUG1
    printf("\t\tScheduler arguments: %s %s %s\n", argv[1], argv[2], argv[3]);
#endif

    int msgid1 = atoi(argv[1]);
    int msgid2 = atoi(argv[2]);
    int k = atoi(argv[3]);

    key_t key;
    int *semid1 = (int *)malloc(k * sizeof(int));
    for (int i = 0; i < k; i++)
    {
        key = ftok("master.c", 11 + i);
        semid1[i] = semget(key, 1, IPC_CREAT | 0666);
        semctl(semid1[i], 0, SETVAL, 0);
    }
    key = ftok("master.c", 7);
    int semid4 = semget(key, 1, IPC_CREAT | 0666);

    int pid = getpid();

    message1 msg1;
    message2 msg2;

    while (k > 0)
    {
        // wait for processes to come
        msgrcv(msgid1, (void *)&msg1, sizeof(message1), 0, 0);
#ifdef VERBOSE
        printf("\t\tScheduling process %d Process No %d\n", msg1.pid, msg1.no);
#endif
        // signal process to start
        V(semid1[msg1.no]);

        // wait for mmu
        msgrcv(msgid2, (void *)&msg2, sizeof(message2), 0, 0);

        // check the type of message
        if (msg2.type == 2)
        {
            printf("\t\tProcess %d terminated\n", msg2.pid);
            k--;
        }
        else if (msg2.type == 1)
        {
            printf("\t\tProcess %d added to end of queue\n", msg2.pid);
            msg1.pid = msg2.pid;
            msg1.type = 1;
            msgsnd(msgid1, (void *)&msg1, sizeof(message1), 0);
        }
        else
        {
#ifdef DEBUG1
            printf("\t\tScheduler: Invalid message type\n");
#endif
        }
    }

    printf("\t\tScheduler terminated\n");

    // signal master on semid4
    V(semid4);

    return 0;
}