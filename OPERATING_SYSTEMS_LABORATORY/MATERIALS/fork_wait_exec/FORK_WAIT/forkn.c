#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Maximum number of children */
#define MAX_NO_CLD 10

int main()
{
    int i, ncld, wtime, status;
    pid_t cpid, mypid, parpid;

    /* Parent process gets its own ID and its parent's ID */
    mypid = getpid();
    parpid = getppid();
    printf("Parent: PID = %u, PPID = %u\n", mypid, parpid);

    /* Parent process obtains the number of children from the user */
    printf("Parent: Number of children = ");
    scanf("%d", &ncld);
    if ((ncld < 0) || (ncld > MAX_NO_CLD))
        ncld = 5;
    printf("\n");

    /* Child creation loop */
    for (i = 0; i < ncld; ++i)
    {
        /* Create the next child */
        cpid = fork();

        /* The child process executes the following conditional block */
        if (cpid == 0)
        {
            /* Child process gets its own ID and its parent's ID */
            mypid = getpid();
            parpid = getppid();
            srand(mypid);
            wtime = 1 + rand() % 10;
            printf("Child %d: PID = %u, PPID = %u, work time = %d\n",
                   i, mypid, parpid, wtime);

            /* Child process does some work (sleeping is a hard work!) */
            sleep(wtime);
            printf("\nChild %d: Work done...\n", i);

            /* Child process exits with status i (the loop variable) */
            exit(i);
        }

        /* The parent continues to the next iteration */
    }

    /* Parent waits for all the children to terminate */
    for (i = 0; i < ncld; ++i)
    {
        /* Parent waits for any child to terminate */
        /* Use waitpid() if the wait is on a specific child */
        wait(&status);

        /* Parent uses the exit status to identify the child */
        printf("Parent: Child %d terminates...\n", WEXITSTATUS(status));
    }

    printf("\nParent terminates...\n");
    exit(0);
}

/*
Parent: PID = 36412, PPID = 29841
Parent: Number of children = 5

Child 0: PID = 36426, PPID = 36412, work time = 3
Child 1: PID = 36427, PPID = 36412, work time = 4
Child 2: PID = 36428, PPID = 36412, work time = 7
Child 3: PID = 36429, PPID = 36412, work time = 6
Child 4: PID = 36430, PPID = 36412, work time = 1

Child 4: Work done...
Parent: Child 4 terminates...

Child 0: Work done...
Parent: Child 0 terminates...

Child 1: Work done...
Parent: Child 1 terminates...

Child 3: Work done...
Parent: Child 3 terminates...

Child 2: Work done...
Parent: Child 2 terminates...

Parent terminates...
*/

/*
Parent: PID = 37944, PPID = 29841
Parent: Number of children = 10

Child 0: PID = 37963, PPID = 37944, work time = 9
Child 1: PID = 37964, PPID = 37944, work time = 10
Child 2: PID = 37965, PPID = 37944, work time = 9
Child 3: PID = 37966, PPID = 37944, work time = 10
Child 4: PID = 37967, PPID = 37944, work time = 3
Child 5: PID = 37968, PPID = 37944, work time = 9
Child 6: PID = 37969, PPID = 37944, work time = 7
Child 7: PID = 37970, PPID = 37944, work time = 3
Child 8: PID = 37971, PPID = 37944, work time = 6
Child 9: PID = 37972, PPID = 37944, work time = 5

Child 4: Work done...

Child 7: Work done...
Parent: Child 4 terminates...
Parent: Child 7 terminates...

Child 9: Work done...
Parent: Child 9 terminates...

Child 8: Work done...
Parent: Child 8 terminates...

Child 6: Work done...
Parent: Child 6 terminates...

Child 0: Work done...

Child 2: Work done...
Parent: Child 0 terminates...

Child 5: Work done...
Parent: Child 2 terminates...
Parent: Child 5 terminates...

Child 1: Work done...

Child 3: Work done...
Parent: Child 1 terminates...
Parent: Child 3 terminates...

Parent terminates...
*/