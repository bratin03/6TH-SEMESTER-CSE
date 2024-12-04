#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        wait(NULL);
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Child || My PID: 136156 || My Parent PID: 136155
I am Parent || My PID: 136155 || My Parent PID: 135668
Child PID: 136156
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(10);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        // wait(NULL);
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Parent || My PID: 138483 || My Parent PID: 137168
Child PID: 138484
I am Child || My PID: 138484 || My Parent PID: 19307
*/

/*
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
1 S  1000  138484   19307  0  80   0 -   594 hrtime pts/3    00:00:00 bin.out
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(10);
        printf("Child PID: %d\n", pid);
    }
}

/*
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 do_wai pts/3    00:00:00 bash
0 S  1000  138012  137168  0  80   0 -   627 hrtime pts/3    00:00:00 a.out
1 Z  1000  138013  138012  0  80   0 -     0 -      pts/3    00:00:00 a.out <defunct>
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(10);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(5);
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Parent || My PID: 139030 || My Parent PID: 137168
Child PID: 139031
I am Child || My PID: 139031 || My Parent PID: 19307
*/

/*
 bratin  bratin03  ../MATERIALS/LT1  ps -t pts/3 -l
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
0 S  1000  139030  137168  0  80   0 -   627 hrtime pts/3    00:00:00 bin.out
1 S  1000  139031  139030  0  80   0 -   594 hrtime pts/3    00:00:00 bin.out

 bratin  bratin03  ../MATERIALS/LT1  ps -t pts/3 -l
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
1 S  1000  139031   19307  0  80   0 -   594 hrtime pts/3    00:00:00 bin.out
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(5);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(10);
        printf("Child PID: %d\n", pid);
    }
}

/* Output of the C code:
I am Child || My PID: 139353 || My Parent PID: 139352
I am Parent || My PID: 139352 || My Parent PID: 137168
Child PID: 139353
*/

/*
 bratin  bratin03  ../MATERIALS/LT1  ps -t pts/3 -l
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
0 S  1000  139352  137168  0  80   0 -   627 hrtime pts/3    00:00:00 bin.out
1 S  1000  139353  139352  0  80   0 -   594 hrtime pts/3    00:00:00 bin.out

 bratin  bratin03  ../MATERIALS/LT1  ps -t pts/3 -l
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
0 S  1000  139352  137168  0  80   0 -   627 hrtime pts/3    00:00:00 bin.out
1 Z  1000  139353  139352  0  80   0 -     0 -      pts/3    00:00:00 bin.out <defunct>
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    signal(SIGCHLD, SIG_IGN);
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(2);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(2);
    }
    else
    {
        wait(NULL);
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Child || My PID: 140255 || My Parent PID: 140254
I am Parent || My PID: 140254 || My Parent PID: 137168
Child PID: 140255
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    signal(SIGCHLD, SIG_IGN);
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(2);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(2);
    }
    else
    {
        // wait(NULL);
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Parent || My PID: 140435 || My Parent PID: 137168
Child PID: 140436
I am Child || My PID: 140436 || My Parent PID: 19307
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    signal(SIGCHLD, SIG_IGN);
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sleep(2);
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        sleep(2);
    }
    else
    {
        // wait(NULL);
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        printf("Child PID: %d\n", pid);
    }
}
/* Output of the C code:
I am Parent || My PID: 140693 || My Parent PID: 137168
Child PID: 140694
I am Child || My PID: 140694 || My Parent PID: 19307
*/

/*
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
1 S  1000  140694   19307  0  80   0 -   627 hrtime pts/3    00:00:00 bin.out
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    signal(SIGCHLD, SIG_IGN);
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("I am Child || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
    }
    else
    {
        sleep(5);
        printf("I am Parent || My PID: %d || My Parent PID: %d\n", getpid(), getppid());
        printf("Child PID: %d\n", pid);
    }
}

/* Output of the C code:
I am Child || My PID: 141101 || My Parent PID: 141100
I am Parent || My PID: 141100 || My Parent PID: 137168
Child PID: 141101
*/

/*
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000  137168  137160  0  80   0 -  2981 pipe_r pts/3    00:00:00 bash
0 S  1000  141100  137168  0  80   0 -   594 hrtime pts/3    00:00:00 bin.out
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main()
{
    int a = 0;
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        a = 1;
        printf("Child: a = %d\n &a = %p\n", a, &a);
    }
    else
    {
        a = 2;
        printf("Parent: a = %d\n &a = %p\n", a, &a);
    }
    printf("a = %d\n &a = %p\n", a, &a);
    return 0;
}
/* Output of the C code:
Parent: a = 2
 &a = 0x7ffe7351eff0
a = 2
 &a = 0x7ffe7351eff0
Child: a = 1
 &a = 0x7ffe7351eff0
a = 1
 &a = 0x7ffe7351eff0
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    int a = 0;
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        srand(getpid());
        a = rand() % 100;
        exit(a);
    }
    int status;
    wait(&status);
    printf("Child exited with status %d\n", WEXITSTATUS(status));
}

/* Output of the C code:
Child exited with status 46
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int A[10], *B, i, t, pid;

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
/* Output of the C code:
Child process going to sleep for 3 seconds
Child process: A = 0x7ffde7238af0, B = 0x55b1c8eba2a0
A[] = 0 1 2 3 4 5 6 7 8 9
B[] = 9 8 7 6 5 4 3 2 1 0
Parent process going to sleep for 2 seconds
Parent process: A = 0x7ffde7238af0, B = 0x55b1c8eba2a0
A[] = 10 11 12 13 14 15 16 17 18 19
B[] = 19 18 17 16 15 14 13 12 11 10
*/
