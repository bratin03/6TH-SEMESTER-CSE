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

#define P(s) semop(s, &pop, 1)

/**
 * Sets the text color to blue.
 */
void blue()
{
    printf("\033[1;34m");
}

/**
 * Sets the text color to red.
 */
void red()
{
    printf("\033[1;31m");
}

/**
 * Sets the text color to green.
 */
void green()
{
    printf("\033[1;32m");
}

/**
 * Sets the text color to pink.
 */
void pink()
{
    printf("\033[1;35m");
}

/**
 * Resets the text color to the default.
 */
void reset()
{
    printf("\033[0m");
}

// Structure for the message queue 1
typedef struct MQ1
{
    long type;     /**< The type of the message queue. */
    int pid;       /**< The process ID. */
    int processNo; /**< The process number. */
} MQ1;

// Structure for the message queue 3
typedef struct MQ3
{
    long type;     /**< The type of the message queue. */
    int Request;   /**< The request. */
    int pid;       /**< The process ID. */
    int processNo; /**< The process number. */
} MQ3;

/**
 * The main function.
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return The exit status.
 */
int main(int argc, char *argv[])
{
    struct sembuf pop;
    pop.sem_num = 0;
    pop.sem_op = -1;
    pop.sem_flg = 0;

    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <Reference String> <MQ ID 1> <MQ ID 3> <Process No>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *refstr = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1));
    strcpy(refstr, argv[1]);
    int msgid1 = atoi(argv[2]);
    int msgid3 = atoi(argv[3]);
    int processNo = atoi(argv[4]);

    key_t key = ftok(TOKEN_PATH, 8 + processNo);
    int semid = semget(key, 1, IPC_CREAT | 0666);

    int pid = getpid();

    MQ1 Ready_Queue;
    Ready_Queue.type = 1;
    Ready_Queue.pid = pid;
    Ready_Queue.processNo = processNo;

#ifdef VERBOSE
    blue();
    printf("\t\t\tProcess: Process %d with Process No %d Created\n", pid, processNo + 1);
    reset();
#endif

    msgsnd(msgid1, (void *)&Ready_Queue, sizeof(MQ1), 0);

    int i = 0;
    int isPageFault = 0;
    int j;
    P(semid);
    while (refstr[i] != '$')
    {
        MQ3 Page_Request;
        Page_Request.type = 1;
        Page_Request.pid = pid;
        Page_Request.processNo = processNo;
        if (isPageFault == 0)
        {
            j = 0;
            while (refstr[i] != '.' && refstr[i] != '\0')
            {
                j = j * 10 + (refstr[i] - '0');
                i++;
            }
            i++;
        }

#ifdef VERBOSE
        pink();
        printf("\t\t\tProcess: Process %d with Process No %d Requesting Page %d\n", pid, processNo + 1, j);
        reset();
#endif

        Page_Request.Request = j;
        msgsnd(msgid3, (void *)&Page_Request, sizeof(MQ3), 0);
        msgrcv(msgid3, (void *)&Page_Request, sizeof(MQ3), 2, 0);

        if (Page_Request.Request == -2)
        {
#ifdef VERBOSE
            red();
            printf("\t\t\tProcess: Process %d with Process No %d Terminated for Illegal Access to Page %d\n", pid, processNo + 1, j);
            reset();
#endif
            exit(EXIT_FAILURE);
        }
        else if (Page_Request.Request == -1)
        {
#ifdef VERBOSE
            red();
            printf("\t\t\tProcess: Process %d with Process No %d Page Fault for Page %d\n", pid, processNo, j);
            reset();
#endif
            isPageFault = 1;
            P(semid);
            continue;
        }
        else
        {
#ifdef VERBOSE
            green();
            printf("\t\t\tProcess: Process %d with Process No %d Got Frame %d for Page %d\n", pid, processNo + 1, Page_Request.Request, j);
            reset();
#endif
            isPageFault = 0;
            continue;
        }
    }

#ifdef VERBOSE
    green();
    printf("\t\t\tProcess: Process %d with Process No %d Received all Pages\n", pid, processNo + 1);
    reset();
#endif

    MQ3 Page_Request;
    Page_Request.type = 1;
    Page_Request.pid = pid;
    Page_Request.Request = -9;

    msgsnd(msgid3, (void *)&Page_Request, sizeof(MQ3), 0);
#ifdef VERBOSE
    green();
    printf("\t\t\tProcess: Process %d with Process No %d Terminated after completing all Pages\n", pid, processNo + 1);
    reset();
#endif

    exit(EXIT_SUCCESS);
}