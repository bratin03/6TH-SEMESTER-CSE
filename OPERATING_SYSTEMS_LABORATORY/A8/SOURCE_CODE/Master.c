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
#include <time.h>
#include <limits.h>

#define VERBOSE
// Run the program with VERBOSE defined to see the output of the program

/*
Funtions for printing colored text
*/
/**
 * Sets the text color to cyan.
 */
void cyan()
{
    printf("\033[1;36m");
}

/**
 * Sets the text color to yellow.
 */
void yellow()
{
    printf("\033[1;33m");
}

/**
 * Resets the text color to the default.
 */
void reset()
{
    printf("\033[0m");
}

#define CORRUPT_PROBABILITY 0.01   // Probability of a page being corrupted
#define MAX_PAGE_PER_PROCESS 10000 // Maximum number of pages per process
#define MAX_INT_DIGITS 20          // Maximum number of digits in an integer
#define PROCESS_GAP 250000         // Gap between creating two processes (in microseconds)
#define TOKEN_PATH "/usr/bin"

struct sembuf pop;
#define P(s) semop(s, &pop, 1)

int sched_PID = -1;
int mmu_PID = -1;

// Structure for Page Table
typedef struct PageTable
{
    int pid;                                /**< Process ID */
    int totalPageFaults;                    /**< Total number of page faults */
    int totalInvalidAccess;                 /**< Total number of invalid accesses */
    int pagetable[MAX_PAGE_PER_PROCESS][3]; /**< Page Table */
} PageTable;

// Structure for message queue 2
typedef struct MQ2
{
    long type;     /**< Type of message */
    int pid;       /**< Process ID */
    int processNo; /**< Process number */
} MQ2;

/**
 * Returns 1 with probability prob, else returns 0.
 * @param prob The probability of returning 1.
 * @return 1 with probability prob, else returns 0.
 */
int corruptPage(float prob)
{
    if (((double)rand() / RAND_MAX) < prob)
    {
        return 1;
    }
    return 0;
}

/**
 * Returns the minimum of two integers.
 * @param a The first integer.
 * @param b The second integer.
 * @return The minimum of two integers.
 */
int min(int a, int b)
{
    return a < b ? a : b;
}

// Global variables for shared memory, semaphores and message queues
int shmid1 = -1;
int shmid2 = -1;
int shmid3 = -1;
int *semid1 = NULL;
int semSched = -1;
int msgid1 = -1;
int msgid2 = -1;
int msgid3 = -1;
PageTable *SM1 = NULL;
int *SM2 = NULL;
int *SM3 = NULL;

// Global variables for the total number of processes, virtual address space size and physical address space size
int k, m, f;

/**
 * Cleans up the shared memory, semaphores and message queues.
 */
void cleanUp()
{
#ifdef VERBOSE
    cyan();
    printf("Master: Cleaning up\n");
    printf("Master: Sending signal to Scheduler and Memory Management Unit\n");
    printf("Master: Detaching and removing shared memory\n");
    printf("Master: Removing semaphores\n");
    printf("Master: Removing message queues\n");
    reset();
#endif
    if (sched_PID != -1)
        kill(sched_PID, SIGINT);
    if (mmu_PID != -1)
        kill(mmu_PID, SIGINT);
    if (SM1 != NULL)
        shmdt(SM1);
    if (shmid1 != -1)
        shmctl(shmid1, IPC_RMID, NULL);
    if (SM2 != NULL)
        shmdt(SM2);
    if (shmid2 != -1)
        shmctl(shmid2, IPC_RMID, NULL);
    if (SM3 != NULL)
        shmdt(SM3);
    if (shmid3 != -1)
        shmctl(shmid3, IPC_RMID, NULL);
    if (semid1 != NULL)
    {
        for (int i = 0; i < k; i++)
        {
            semctl(semid1[i], 0, IPC_RMID, 0);
        }
    }
    if (semSched != -1)
        semctl(semSched, 0, IPC_RMID, 0);
    if (msgid1 != -1)
        msgctl(msgid1, IPC_RMID, NULL);
    if (msgid2 != -1)
        msgctl(msgid2, IPC_RMID, NULL);
    if (msgid3 != -1)
        msgctl(msgid3, IPC_RMID, NULL);
}

/**
 * Signal handler for SIGINT and SIGQUIT.
 * @param signum The signal number.
 */
void sighand(int signum)
{

    if (signum == SIGINT || signum == SIGQUIT)
    {
#ifdef VERBOSE
        yellow();
        printf("\nMaster: Received signal %d\n", signum);
        reset();
#endif
        cleanUp();
        exit(EXIT_FAILURE);
    }
}

/**
 * Takes input from the user.
 */
void takeInput()
{
    printf("Enter the Total number of Processes: ");
    scanf("%d", &k);
    printf("Enter the Virtual Address Space size - (Maximum number of pages required per process): ");
    scanf("%d", &m);
    printf("Enter the Physical Address Space size - (Total number of frames): ");
    scanf("%d", &f);
    if (k <= 0 || m <= 0 || f <= 0)
    {
        fprintf(stderr, "Invalid Input\n");
        exit(EXIT_FAILURE);
    }
    if (m > MAX_PAGE_PER_PROCESS)
    {
        fprintf(stderr, "Virtual Address Space size exceeds the maximum limit\n");
        fprintf(stderr, "Maximum limit is %d\n", MAX_PAGE_PER_PROCESS);
        fprintf(stderr, "To change the limit, change the value of MAX_PAGE_PER_PROCESS in master.c and mmu.c\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Initializes the shared memory, semaphores and message queues.
 */
void initSharedMemory()
{
    key_t key = ftok(TOKEN_PATH, 1);
    shmid1 = shmget(key, k * sizeof(PageTable), IPC_CREAT | 0666);
    SM1 = (PageTable *)shmat(shmid1, NULL, 0);

    for (int i = 0; i < k; i++)
    {
        SM1[i].pid = 0;
    }

    key = ftok(TOKEN_PATH, 2);
    shmid2 = shmget(key, (f + 1) * sizeof(int), IPC_CREAT | 0666);
    SM2 = (int *)shmat(shmid2, NULL, 0);

    for (int i = 0; i < f; i++)
    {
        SM2[i] = 1;
    }
    SM2[f] = -1;

    key = ftok(TOKEN_PATH, 3);
    shmid3 = shmget(key, k * sizeof(int), IPC_CREAT | 0666);
    SM3 = (int *)shmat(shmid3, NULL, 0);

    for (int i = 0; i < k; i++)
    {
        SM3[i] = 0;
    }

    key = ftok(TOKEN_PATH, 4);
    semSched = semget(key, 1, IPC_CREAT | 0666);
    semctl(semSched, 0, SETVAL, 0);

    key = ftok(TOKEN_PATH, 5);
    msgid1 = msgget(key, IPC_CREAT | 0666);

    key = ftok(TOKEN_PATH, 6);
    msgid2 = msgget(key, IPC_CREAT | 0666);

    key = ftok(TOKEN_PATH, 7);
    msgid3 = msgget(key, IPC_CREAT | 0666);

    semid1 = (int *)malloc(k * sizeof(int));
    for (int i = 0; i < k; i++)
    {
        key = ftok(TOKEN_PATH, i + 8);
        semid1[i] = semget(key, 1, IPC_CREAT | 0666);
        semctl(semid1[i], 0, SETVAL, 0);
    }
#ifdef VERBOSE
    cyan();
    printf("Master: Created Shared Memory, Semaphores and Message Queues\n");
    reset();
#endif
}

/**
 * Allocates frames to the processes in proportion to the size of the process.
 * @param SM1 The Page Table.
 * @param SM2 The Physical Address Space.
 * @param SM3 The Virtual Address Space.
 * @param totalPages The total number of pages.
 * @param k The total number of processes.
 * @param f The total number of frames.
 */
void proportional_allocation(PageTable *SM1, int *SM2, int *SM3, int *totalPages, int k, int f)
{
    // Proportional Allocation
    int processNumAlloc[k];
    for (int i = 0; i < k; i++)
    {
        // Calculate the fraction of the total number of pages
        double temp = ((double)SM3[i] / (*totalPages)) * f;
        processNumAlloc[i] = min((int)temp, SM3[i]); // Allocate the minimum of the calculated value and the total number of pages required by the process
#ifdef VERBOSE
        yellow();
        printf("Process No %d: Number of Frames Allocated: %d\n", i + 1, processNumAlloc[i]);
        reset();
#endif
    }
    // Allocate the frames to the processes (Don't load the pages into the frames)
    int last_frame_allocated = 0;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < processNumAlloc[i]; j++)
        {
            SM1[i].pagetable[j][0] = last_frame_allocated;
            SM2[last_frame_allocated] = 0;
            last_frame_allocated++;
        }
    }
#ifdef VERBOSE
    yellow();
    printf("Free Frames: %d\n", f - last_frame_allocated);
    reset();
#endif
}

/**
 * Creates the page reference string for each process.
 * @param refi The page reference string.
 * @param refString The page reference string in string format.
 * @param k The total number of processes.
 * @param m The Virtual Address Space size.
 * @param SM3 The Virtual Address Space.
 * @param totalPages The total number of pages.
 */
void createPageRef(int **refi, char **refString, int k, int m, int *SM3, int *totalPages)
{
    for (int i = 0; i < k; i++)
    {
        SM3[i] = rand() % m + 1;
        *totalPages += SM3[i];
        for (int j = 0; j < m; j++)
        {
            SM1[i].pagetable[j][0] = -1;
            SM1[i].pagetable[j][1] = 0;
            SM1[i].pagetable[j][2] = INT_MAX;
        }

        int y = 0;
        int x = rand() % (8 * SM3[i] + 1) + 2 * SM3[i];
        refi[i] = (int *)malloc(x * sizeof(int));

        int *digits = (int *)malloc(x * sizeof(int));
        for (int j = 0; j < x; j++)
        {
            refi[i][j] = rand() % SM3[i];

            if (corruptPage(CORRUPT_PROBABILITY))
            {
                refi[i][j] = rand() % m;
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
        refString[i] = (char *)malloc(y * sizeof(char));
        memset(refString[i], '\0', y);
        for (int j = 0; j < x; j++)
        {
            char temp[digits[j] + 1];
            sprintf(temp, "%d", refi[i][j]);
            strcat(refString[i], temp);
            strcat(refString[i], ".");
        }
        strcat(refString[i], "$");
#ifdef VERBOSE
        cyan();
        printf("Virtual Address Space for Process %d: %d\n", i + 1, SM3[i]);
        yellow();
        printf("Ref String for Process %d: %s\n", i + 1, refString[i]);
        reset();
#endif
        free(digits);
    }
}

/**
 * Creates the processes.
 * @param refstr The page reference string.
 * @param msgid1str The message queue 1 ID in string format.
 * @param msgid3str The message queue 3 ID in string format.
 * @param k The total number of processes.
 */
void createProcess(char **refstr, char *msgid1str, char *msgid3str, int k)
{
    for (int i = 0; i < k; i++)
    {
        usleep(PROCESS_GAP);
        int pid = fork();
        if (pid != 0)
        {
            SM1[i].pid = pid;
        }
        else
        {
            char processNoStr[10];
            sprintf(processNoStr, "%d", i);
#ifdef VERBOSE
            cyan();
            printf("Master: Creating Process %d\n", i);
            reset();
#endif
            execlp("./process", "./process", refstr[i], msgid1str, msgid3str, processNoStr, NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * The main function.
 * @return The exit status of the program.
 */
int main(int argc, char *argv[])
{
    if (argc != 4 && argc != 1)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        fprintf(stderr, "To give the number of processes, virtual address space size and physical address space size as command line arguments\n");
        fprintf(stderr, "Usage: %s <k> <m> <f>\n", argv[0]);
        fprintf(stderr, "Or\n");
        fprintf(stderr, "To take input from the user\n");
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    signal(SIGINT, sighand);
    signal(SIGQUIT, sighand);
    pop.sem_num = 0;
    pop.sem_op = -1;
    pop.sem_flg = 0;

    if (argc == 4)
    {
        k = atoi(argv[1]);
        m = atoi(argv[2]);
        f = atoi(argv[3]);
    }
    else
    {
        takeInput();
    }

    initSharedMemory();

    char msgid1String[MAX_INT_DIGITS], msgid2String[MAX_INT_DIGITS], msgid3String[MAX_INT_DIGITS];
    sprintf(msgid1String, "%d", msgid1);
    sprintf(msgid2String, "%d", msgid2);
    sprintf(msgid3String, "%d", msgid3);

    char shmid1String[MAX_INT_DIGITS], shmid2String[MAX_INT_DIGITS], shmid3String[MAX_INT_DIGITS];
    sprintf(shmid1String, "%d", shmid1);
    sprintf(shmid2String, "%d", shmid2);
    sprintf(shmid3String, "%d", shmid3);

    char Stringk[10];
    sprintf(Stringk, "%d", k);

#ifdef VERBOSE
    cyan();
    printf("Master: Creating Scheduler\n");
    reset();
#endif

    sched_PID = fork();
    if (sched_PID == 0)
    {
        execlp("./sched", "./sched", msgid1String, msgid2String, Stringk, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

#ifdef VERBOSE
    cyan();
    printf("Master: Creating Memory Management Unit\n");
    reset();
#endif

    mmu_PID = fork();
    if (mmu_PID == 0)
    {
        execlp("xterm", "xterm", "-T", "MMU", "-e", "./mmu", msgid2String, msgid3String, shmid1String, shmid2String, shmid3String, Stringk, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    MQ2 msg2;
    msgrcv(msgid2, &msg2, sizeof(MQ2), 1, 0);
    mmu_PID = msg2.pid;

    int **refi = (int **)malloc((k) * sizeof(int *));
    char **refString = (char **)malloc((k) * sizeof(char *));
    int *totalPages = (int *)malloc(sizeof(int));
    *totalPages = 0;

    createPageRef(refi, refString, k, m, SM3, totalPages);
    proportional_allocation(SM1, SM2, SM3, totalPages, k, f);
    createProcess(refString, msgid1String, msgid3String, k);

    P(semSched);

    cleanUp();

    exit(EXIT_SUCCESS);
}