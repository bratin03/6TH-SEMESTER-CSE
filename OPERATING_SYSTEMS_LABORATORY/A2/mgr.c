/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Define a structure to store process information
typedef struct data_struct
{
    pid_t pid;       // Process ID
    pid_t pid_group; // Process Group ID
    int status;      // Process status (0: Finished, 1: Terminated, 2: Suspended, 3: Killed)
    int arguement;   // Argument associated with the process
} process;

// Array to store process information
process PT[11];

int count = 0; // Counter for the number of processes

// Function to add an entry to the process table
void addentry(pid_t pid, pid_t pid_group, int status, char arguement)
{
    PT[count].pid = pid;
    PT[count].pid_group = pid_group;
    PT[count].status = status;
    PT[count].arguement = arguement;
    count++;
}

int idx = -1;       // Index for the current job in the process table
int jobrunning = 0; // Flag to indicate if a job is currently running

// Signal handler for SIGINT (Ctrl+C)
void sigintHandler(int signum)
{
    if (jobrunning)
    {
        // Send SIGINT to the current running job
        kill(PT[idx].pid, SIGINT);
        PT[idx].status = 1; // Mark the job as terminated
        printf("\n");
    }
    else
    {
        printf("\nmgr> : ");
        fflush(stdout);
    }
}

// Signal handler for SIGTSTP (Ctrl+Z)
void sigstpHandler(int signum)
{
    if (jobrunning)
    {
        // Send SIGTSTP to the current running job
        kill(PT[idx].pid, SIGTSTP);
        PT[idx].status = 2; // Mark the job as suspended
        printf("\n");
    }
    else
    {
        printf("\nmgr> : ");
        fflush(stdout);
    }
}

// Function to print the process table
void printtable()
{
    printf("NO\t\tPID\t\tPGID\t\tSTATUS\t\tNAME\n");
    printf("%d\t\t%d\t\t%d\t\tSELF\t\tmgr\n", 0, PT[0].pid, PT[0].pid_group);

    for (int i = 1; i < count; i++)
    {
        printf("%d\t\t%d\t\t%d\t\t", i, PT[i].pid, PT[i].pid_group);

        // Print the status based on the status code
        if (PT[i].status == 0)
            printf("FINISHED\t");
        else if (PT[i].status == 1)
            printf("TERMINATED\t");
        else if (PT[i].status == 2)
            printf("SUSPENDED\t");
        else if (PT[i].status == 3)
            printf("KILLED\t\t");

        printf("job %c\n", PT[i].arguement);
    }
}

// Function to print help information
void print_help()
{
    printf("Command\t:\tAction\n");
    printf("c\t:\t Continue a Suspended job\n");
    printf("h\t:\t Print this help message\n");
    printf("k\t:\t Kill a suspended job\n");
    printf("p\t:\t Print the process table\n");
    printf("q\t:\t Quit\n");
    printf("r\t:\t Run a new job\n");
}

int main(int argc, char *argv[])
{
    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Initialize the first entry in the process table for the manager
    PT[0].pid = getpid();
    PT[0].pid_group = getpgid(getpid());
    PT[0].status = -1; // Marked as -1 to indicate the manager itself
    count++;

    // Variable for user input
    char input;

    // Register signal handlers for SIGINT (Ctrl+C) and SIGTSTP (Ctrl+Z)
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigstpHandler);

    // Main loop for the manager program
    while (1)
    {
        jobrunning = 0; // Reset the jobrunning flag

        // Print manager prompt
        printf("mgr> : ");

        // Read user input
        scanf("%c", &input);
        getchar(); // Consume newline character

        // Process user input
        if (input == 'h')
        {
            // Print help message
            print_help();
        }
        if (input == 'p')
        {
            // Print the process table
            printtable();
        }
        if (input == 'r')
        {
            // Run a new job
            if (count >= 11)
            {
                printf("Process table is full. Quitting...\n");
                exit(EXIT_SUCCESS);
            }

            // Generate a random character for the job name
            char str[2];
            str[0] = 'A' + rand() % 26;
            char c = str[0];
            str[1] = '\0';

            // Fork a new process for the job
            pid_t cpid = fork();

            if (cpid == 0)
            {
                // Child process (job)
                setpgid(getpid(), getpid());
                execl("./job", "job", str, NULL);
            }
            else
            {
                // Parent process (manager)
                sleep(1);
                addentry(cpid, getpgid(cpid), 0, c); // Add the job entry to the process table
                jobrunning = 1;
                idx = count - 1;
                waitpid(cpid, NULL, WUNTRACED); // Wait for the job to complete or be suspended
            }
        }
        if (input == 'c')
        {
            // Continue a suspended job
            int i;
            int exist = 0;

            // Display the list of suspended jobs
            printf("Suspended jobs: ");
            for (i = 1; i < count; i++)
            {
                if (PT[i].status == 2)
                {
                    printf("%d ", i);
                    exist = 1;
                }
            }

            if (exist == 1)
            {
                int num;

                // Prompt user to pick a job to continue
                printf("(Pick One): ");
                scanf("%d", &num);
                // Update process table and resume the job
                PT[num].status = 0;
                jobrunning = 1;
                idx = num;
                kill(PT[num].pid, SIGCONT);
                waitpid(PT[num].pid, NULL, WUNTRACED);
                getchar(); // Consume newline character
            }
            else
            {
                printf("\n");
            }
        }
        if (input == 'k')
        {
            // Kill a suspended job
            int i;
            int exist = 0;

            // Display the list of suspended jobs
            printf("Suspended jobs: ");
            for (i = 1; i < count; i++)
            {
                if (PT[i].status == 2)
                {
                    printf("%d ", i);
                    exist = 1;
                }
            }

            if (exist == 1)
            {
                int num;

                // Prompt user to pick a job to kill
                printf("(Pick One): ");
                scanf("%d", &num);
                // Update process table and kill the job
                PT[num].status = 3;
                jobrunning = 1;
                idx = num;
                kill(PT[num].pid, SIGKILL);
                waitpid(PT[num].pid, NULL, WUNTRACED);
                getchar(); // Consume newline character
            }
            else
            {
                printf("\n");
            }
        }
        if (input == 'q')
        {
            // Quit the manager program
            exit(EXIT_SUCCESS);
        }
    }
}
