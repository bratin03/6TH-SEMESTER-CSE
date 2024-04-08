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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#define VERBOSE
// Run with VERBOSE defined for detailed output

#define TOKEN_PATH "/usr/bin"

/**
 * Sets the text color to purple.
 */
void purple()
{
    printf("\033[1;35m");
}

/**
 * Sets the text color to green.
 */
void green()
{
    printf("\033[1;32m");
}

/**
 * Sets the text color to red.
 */
void red()
{
    printf("\033[1;31m");
}

/**
 * Resets the text color to default.
 */
void reset()
{
    printf("\033[0m");
}

#define V(s) semop(s, &vop, 1)

// Structure for message queue 1
typedef struct MQ1
{
    long type;     /**< Type of message */
    int pid;       /**< Process ID */
    int processNo; /**< Process number */
} MQ1;

// Structure for message queue 2
typedef struct MQ2
{
    long type;     /**< Type of message */
    int pid;       /**< Process ID */
    int processNo; /**< Process number */
} MQ2;

/**
 * The main function.
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return The exit status of the program.
 */
int main(int argc, char *argv[])
{
    struct sembuf vop;
    vop.sem_num = 0;
    vop.sem_op = 1;
    vop.sem_flg = 0;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <MQ1 ID> <MQ2 ID> <Number of processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int msgid1 = atoi(argv[1]);
    int msgid2 = atoi(argv[2]);
    int k = atoi(argv[3]);

    key_t key = ftok(TOKEN_PATH, 4);
    int semid = semget(key, 1, IPC_CREAT | 0666);

    int *semid1 = (int *)malloc(sizeof(int) * k);
    for (int i = 0; i < k; i++)
    {
        key = ftok(TOKEN_PATH, i + 8);
        semid1[i] = semget(key, 1, IPC_CREAT | 0666);
    }

    MQ1 Ready_Queue;
    MQ2 msg2;

    while (k > 0)
    {
        msgrcv(msgid1, (void *)&Ready_Queue, sizeof(MQ1), 0, 0);

#ifdef VERBOSE
        purple();
        printf("\t\tScheduler: Process %d arrived with PID: %d\n", Ready_Queue.processNo + 1, Ready_Queue.pid);
        reset();
#endif

        V(semid1[Ready_Queue.processNo]);
        msgrcv(msgid2, (void *)&msg2, sizeof(MQ2), 0, 0);

        if (msg2.type == 2)
        {
#ifdef VERBOSE
            red();
            printf("\t\tScheduler: Process %d with PID: %d terminated\n", msg2.processNo + 1, msg2.pid);
            reset();
#endif
            k--;
        }
        else if (msg2.type == 1)
        {
#ifdef VERBOSE
            green();
            printf("\t\tScheduler: Process %d with PID: %d preempted\n", msg2.processNo + 1, msg2.pid);
            reset();
#endif
            Ready_Queue.pid = msg2.pid;
            Ready_Queue.type = 1;
            Ready_Queue.processNo = msg2.processNo;
            msgsnd(msgid1, (void *)&Ready_Queue, sizeof(MQ1), 0);
        }
    }

#ifdef VERBOSE
    purple();
    printf("\t\tScheduler: All processes terminated\n");
    printf("\t\tScheduler: Terminating\n");
    reset();
#endif

    V(semid);

    exit(EXIT_SUCCESS);
}