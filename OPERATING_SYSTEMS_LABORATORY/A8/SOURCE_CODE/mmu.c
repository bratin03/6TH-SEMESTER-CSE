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
#include <limits.h>

#define DISPLAY
#define VERBOSE
// #define DEBUG
// Run with DEBUG defined for detailed output

#define MAX_PAGE_PER_PROCESS 10000
#define OUTPUT_FILE "result.txt"

/**
 * Sets the text color to default.
 */
void reset()
{
    printf("\033[0m");
}

// Structure for Page Table
typedef struct PageTable
{
    int pid;                                /**< Process ID */
    int totalPageFaults;                    /**< Total number of page faults */
    int totalInvalidAccess;                 /**< Total number of invalid accesses */
    int pagetable[MAX_PAGE_PER_PROCESS][3]; /**< Page Table */
} PageTable;
// Structure for Message Queue 2
typedef struct MQ2
{
    long type;     /**< Type of message */
    int pid;       /**< Process ID */
    int processNo; /**< Process Number */
} MQ2;

// Structure for Message Queue 3
typedef struct MQ3
{
    long type;     /**< Type of message */
    int Request;   /**< Request */
    int pid;       /**< Process ID */
    int processNo; /**< Process Number */
} MQ3;

FILE *fp;
int k;
PageTable *SM1;

/**
 * Signal handler for SIGINT and SIGQUIT.
 * @param signo The signal number.
 */
void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGQUIT)
    {

#ifdef DISPLAY
        reset();
        printf("\n");
        fprintf(fp, "\n");
#endif
        for (int i = 0; i < k; i++)
        {
#ifdef DISPLAY
            reset();
            printf("Process %d: Total Page Faults: %d, Total Invalid Access: %d\n", i + 1, SM1[i].totalPageFaults, SM1[i].totalInvalidAccess);
#endif
            fprintf(fp, "Process %d: Total Page Faults: %d, Total Invalid Access: %d\n", i + 1, SM1[i].totalPageFaults, SM1[i].totalInvalidAccess);
        }

        fclose(fp);
        printf("\nMMU: Memory Management Unit Terminated. Press enter to exit.\n");
        char trash;
        scanf("%c", &trash);
        exit(EXIT_SUCCESS);
    }
}

/**
 * Finds a free frame in the memory.
 * @param SM2 The shared memory 2.
 * @return The free frame number.
 */
int findFreeFrame(int *SM2)
{
    int j = 0;
    while (SM2[j] != -1)
    {
        if (SM2[j] == 1)
        {
            SM2[j] = 0;
            break;
        }
        j++;
    }
    if (SM2[j] == -1)
    {
        return -1;
    }
    return j;
}

/**
 * Handles the page fault.
 * @param SM1 The shared memory 1.
 * @param SM2 The shared memory 2.
 * @param SM3 The shared memory 3.
 * @param T The timestamp.
 * @param processNo The process number.
 * @param frameNo The frame number.
 * @return 1 if the page fault is handled, 0 otherwise.
 */
int PageFaultHandler(PageTable *SM1, int *SM2, int *SM3, int T, int processNo, int frameNo)
{

    if (SM1[processNo].pagetable[frameNo][0] != -1 && SM1[processNo].pagetable[frameNo][1] == 0)
    {
        SM1[processNo].pagetable[frameNo][1] = 1;
        SM1[processNo].pagetable[frameNo][2] = T;
#ifdef DEBUG
        reset();
        printf("\tMMU: Frame Allocated - (Process %d, Page %d, Frame %d)\n", processNo + 1, frameNo, SM1[processNo].pagetable[frameNo][0]);
#endif
        return 1;
    }
    int inital_allocation = 0;
    int i;
    for (i = 0; i < SM3[processNo]; i++)
    {
        if (SM1[processNo].pagetable[i][0] != -1 && SM1[processNo].pagetable[i][1] == 0)
        {
            inital_allocation = 1;
            break;
        }
    }

    if (inital_allocation == 1)
    {
        if (i != frameNo)
        {
            SM1[processNo].pagetable[frameNo][0] = SM1[processNo].pagetable[i][0];
            SM1[processNo].pagetable[frameNo][1] = 1;
            SM1[processNo].pagetable[frameNo][2] = T;
            SM1[processNo].pagetable[i][0] = -1;
            SM1[processNo].pagetable[i][2] = INT_MAX;
        }
        else
        {
            SM1[processNo].pagetable[frameNo][1] = 1;
            SM1[processNo].pagetable[frameNo][2] = T;
        }
#ifdef DEBUG
        reset();
        printf("\tMMU: Frame Allocated - (Process %d, Page %d, Frame %d)\n", processNo + 1, frameNo, SM1[processNo].pagetable[frameNo][0]);
#endif
        return 1;
    }

    int freeFrame = findFreeFrame(SM2);
    if (freeFrame != -1)
    {
        SM1[processNo].pagetable[frameNo][0] = freeFrame;
        SM1[processNo].pagetable[frameNo][1] = 1;
        SM1[processNo].pagetable[frameNo][2] = T;
#ifdef DEBUG
        reset();
        printf("\tMMU: Frame Allocated - (Process %d, Page %d, Frame %d)\n", processNo + 1, frameNo, freeFrame);
#endif
        return 1;
    }
    else
    {
        int min = INT_MAX;
        int replaceFrame = -1;
        for (int j = 0; j < SM3[processNo]; j++)
        {
            if (SM1[processNo].pagetable[j][0] != -1)
            {
                if (SM1[processNo].pagetable[j][2] <= min)
                {
                    min = SM1[processNo].pagetable[j][2];
                    replaceFrame = j;
                }
            }
        }
        if (replaceFrame == -1)
        {
#ifdef DEBUG
            reset();
            printf("\tMMU: No frame to replace - (Process %d, Page %d)\n", processNo, frameNo);
#endif
            return 0;
        }
        SM1[processNo].pagetable[replaceFrame][1] = 0;
        SM1[processNo].pagetable[frameNo][0] = SM1[processNo].pagetable[replaceFrame][0];
        SM1[processNo].pagetable[frameNo][1] = 1;
        SM1[processNo].pagetable[frameNo][2] = T;
        SM1[processNo].pagetable[replaceFrame][0] = -1;
        SM1[processNo].pagetable[replaceFrame][2] = INT_MAX;
#ifdef DEBUG
        reset();
        printf("\tMMU: Frame Replaced - (Process %d, Page %d, Replaced Page %d, Frame %d)\n", processNo + 1, frameNo, replaceFrame, SM1[processNo].pagetable[frameNo][0]);
#endif
        return 1;
    }
    return 0;
}

/**
 * Frees the resources of a process.
 * @param SM1 The shared memory 1.
 * @param SM2 The shared memory 2.
 * @param SM3 The shared memory 3.
 * @param processNo The process number.
 */
void freeProcessResources(PageTable *SM1, int *SM2, int *SM3, int processNo)
{
    for (int j = 0; j < SM3[processNo]; j++)
    {
        if (SM1[processNo].pagetable[j][0] != -1 && SM1[processNo].pagetable[j][1] == 1)
        {
#ifdef DEBUG
            reset();
            printf("\tMMU: Frame freeing - (Process %d, Frame %d)\n", processNo + 1, SM1[processNo].pagetable[j][0]);
#endif
            SM2[SM1[processNo].pagetable[j][0]] = 1;
            SM1[processNo].pagetable[j][0] = -1;
            SM1[processNo].pagetable[j][1] = 0;
            SM1[processNo].pagetable[j][2] = INT_MAX;
        }
    }
}

/**
 * The main function.
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return 0, to terminate the program.
 */
int main(int argc, char *argv[])
{
    int T = 0;
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);

    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s <MQ ID 2> <MQ ID 3> <SM ID 1> <SM ID 2> <SM ID 3> <Total Processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    MQ2 msg2;
    MQ3 Page_Request;
    int myPID = getpid();
    msg2.pid = myPID;
    msg2.processNo = -1;
    msg2.type = 1;

    int msgid2 = atoi(argv[1]);
    msgsnd(msgid2, (void *)&msg2, sizeof(MQ2), 0);
    int msgid3 = atoi(argv[2]);
    int shmid1 = atoi(argv[3]);
    int shmid2 = atoi(argv[4]);
    int shmid3 = atoi(argv[5]);
    k = atoi(argv[6]);

    SM1 = (PageTable *)shmat(shmid1, NULL, 0);
    int *SM2 = (int *)shmat(shmid2, NULL, 0);
    int *SM3 = (int *)shmat(shmid3, NULL, 0);

    fp = fopen(OUTPUT_FILE, "w");

    while (1)
    {
        msgrcv(msgid3, (void *)&Page_Request, sizeof(MQ3), 1, 0);
        Page_Request.type = 2;
        int i = Page_Request.processNo;
        int page = Page_Request.Request;
        if (page != -9)
        {
            T++;
#ifdef DISPLAY
            reset();
            printf("Global Ordering - (Timestamp %d, Process %d, Page %d)\n", T, i + 1, page);
#endif
            fprintf(fp, "Global Ordering - (Timestamp %d, Process %d, Page %d)\n", T, i + 1, page);
        }
        if (page == -9)
        {
#ifdef DEBUG
            reset();
            printf("\tMMU: Process %d Terminated\n", i + 1);
#endif
            freeProcessResources(SM1, SM2, SM3, i);
            msg2.type = 2;
            msg2.pid = Page_Request.pid;
            msg2.processNo = Page_Request.processNo;
            msgsnd(msgid2, (void *)&msg2, sizeof(MQ2), 0);
        }
        else if (page >= SM3[i])
        {
#ifdef VERBOSE
            reset();
            printf("TRYING TO ACCESS INVALID PAGE REFERENCE\n");
#endif
#ifdef DEBUG
            reset();
            printf("\tMMU: Invalid Page Reference - (Process %d, Page %d)\n", i + 1, page);
#endif
            SM1[i].totalInvalidAccess++;
            Page_Request.Request = -2;
            msgsnd(msgid3, (void *)&Page_Request, sizeof(MQ3), 0);
#ifdef DISPLAY
            reset();
            printf("Invalid Page Reference - (Process %d, Page %d)\n", i + 1, page);
#endif
            fprintf(fp, "Invalid Page Reference - (Process %d, Page %d)\n", i + 1, page);
            freeProcessResources(SM1, SM2, SM3, i);
            msg2.type = 2;
            msg2.pid = Page_Request.pid;
            msg2.processNo = Page_Request.processNo;
            msgsnd(msgid2, (void *)&msg2, sizeof(MQ2), 0);
        }
        else if (SM1[i].pagetable[page][0] != -1 && SM1[i].pagetable[page][1] == 1)
        {
#ifdef DEBUG
            reset();
            printf("\tMMU: Page Hit - (Process %d, Page %d, Frame %d)\n", i + 1, page, SM1[i].pagetable[page][0]);
#endif
            SM1[i].pagetable[page][2] = T;
            Page_Request.Request = SM1[i].pagetable[page][0];
            msgsnd(msgid3, (void *)&Page_Request, sizeof(MQ3), 0);
        }
        else
        {
#ifdef DEBUG
            reset();
            printf("\tMMU: Page Fault - (Process %d, Page %d)\n", i + 1, page);
#endif

            SM1[i].totalPageFaults++;
            Page_Request.Request = -1;
            msgsnd(msgid3, (void *)&Page_Request, sizeof(MQ3), 0);
#ifdef DISPLAY
            reset();
            printf("Page fault sequence - (Process %d, Page %d)\n", i + 1, page);
#endif
            fprintf(fp, "Page fault sequence - (Process %d, Page %d)\n", i + 1, page);

            PageFaultHandler(SM1, SM2, SM3, T, i, page);
            msg2.type = 1;
            msg2.pid = Page_Request.pid;
            msg2.processNo = Page_Request.processNo;
            msgsnd(msgid2, (void *)&msg2, sizeof(MQ2), 0);
        }
    }

    for (int i = 0; i < k; i++)
    {
#ifdef DISPLAY
        reset();
        printf("Process %d: Total Page Faults: %d, Total Invalid Access: %d\n", i + 1, SM1[i].totalPageFaults, SM1[i].totalInvalidAccess);
#endif
        fprintf(fp, "Process %d: Total Page Faults: %d, Total Invalid Access: %d\n", i + 1, SM1[i].totalPageFaults, SM1[i].totalInvalidAccess);
    }
}