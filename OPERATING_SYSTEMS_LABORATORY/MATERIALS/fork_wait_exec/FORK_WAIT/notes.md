### 1

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
   int n, mypid, parpid;

   printf("Parent: n = ");
   scanf("%d", &n);

   /* Child creation */
   if (fork())
   { /* Parent process */
      mypid = getpid();
      parpid = getppid();
      printf("Parent: PID = %u, PPID = %u...\n", mypid, parpid);
   }
   else
   { /* Child process */
      mypid = getpid();
      parpid = getppid();
      printf("Child : PID = %u, PPID = %u...\n", mypid, parpid);
      n = n * n;
   }

   printf("Process PID = %u: n = %d\n", mypid, n);

   exit(0);
}
```

```txt
/*
Parent: n = 12
Parent: PID = 31147, PPID = 29841...
Process PID = 31147: n = 12
Child : PID = 31168, PPID = 31147...
Process PID = 31168: n = 144
*/
```

### 2

```c
/*
	                 PROCESS FORKING 		

  This program illustrates the fork() system call.

*/

/*
                     PROCESS FORKING

  This program illustrates the fork() system call.

*/

#include <stdio.h>
#include <sys/ipc.h>

main()
{
    int i;
    int x = 10;
    int pid1, pid2, status;

    printf("Before forking, the value of x is %d\n", x);

    /*
       After forking, we make the parent and its two children
           increment x in different ways to illustrate that they
       have different copies of x
    */
    if ((pid1 = fork()) == 0)
    {

        /* First child process */
        for (i = 0; i < 5; i++)
        {
            printf("\t\t\t At first child: x= %d\n", x);
            x = x + 10;
            sleep(1); /* Sleep for 1 second */
        }
    }
    else
    {

        /* Parent process */

        /* Create another child process */
        if ((pid2 = fork()) == 0)
        {

            /* Second child process */
            for (i = 0; i < 5; i++)
            {
                printf("\t\t\t\t\t\t At second child: x= %d\n", x);
                x = x + 20;
                sleep(1); /* Sleep for 1 second */
            }
        }
        else
        {

            /* Parent process */
            for (i = 0; i < 5; i++)
            {
                printf("At parent: x= %d\n", x);
                x = x + 5;
                sleep(1); /* Sleep for 1 second */
            }

            /*
                The waitpid() system call causes the parent
                to wait for a child process with a specific pid to complete
                its execution. The input parameter can
                specify the PID of the child process for
                which it has to wait.
            */

            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
    }
}
```
```txt
Before forking, the value of x is 10
At parent: x= 10
                         At first child: x= 10
                                                 At second child: x= 10
At parent: x= 15
                         At first child: x= 20
                                                 At second child: x= 30
At parent: x= 20
                                                 At second child: x= 50
                         At first child: x= 30
At parent: x= 25
                                                 At second child: x= 70
                         At first child: x= 40
At parent: x= 30
                                                 At second child: x= 90
                         At first child: x= 50
```

### 3
```c
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
```

```txt
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
```

```txt
1) Parent process keeps forking the child while the child process are sleeping
2) Parent then waits for the children to terminate
```
```txt
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
```

### 4
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int A[10], *B, i, t, pid;
    // Notice the pointer
    srand((unsigned int)time(NULL));
    B = (int *)malloc(10 * sizeof(int));
    t = 1 + rand() % 5;
    if ((pid = fork()))
    {
        for (i = 0; i < 10; ++i)
            A[i] = B[9 - i] = 10 + i;
        printf("Parent process going to sleep for %d seconds\n", t);
        sleep(t);
        printf("Parent process: A = %p, B = %p\n", A, B);
    }
    else
    {
        for (i = 0; i < 10; ++i)
            A[i] = B[9 - i] = i;
        i = t;
        while (i == t)
            t = 1 + rand() % 5;
        printf("Child process going to sleep for %d seconds\n", t);
        sleep(t);
        printf("Child process: A = %p, B = %p\n", A, B);
    }
    printf("A[] =");
    for (i = 0; i < 10; ++i)
        printf(" %d", A[i]);
    printf("\n");
    printf("B[] =");
    for (i = 0; i < 10; ++i)
        printf(" %d", B[i]);
    printf("\n");
    free(B);
    if (pid)
        wait(NULL);
    exit(0);
}
```

```txt
Parent process going to sleep for 5 seconds
Child process going to sleep for 2 seconds
Child process: A = 0x7ffe89050510, B = 0x55e25fe122a0
A[] = 0 1 2 3 4 5 6 7 8 9
B[] = 9 8 7 6 5 4 3 2 1 0
Parent process: A = 0x7ffe89050510, B = 0x55e25fe122a0
A[] = 10 11 12 13 14 15 16 17 18 19
B[] = 19 18 17 16 15 14 13 12 11 10
```