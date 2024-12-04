#include <stdio.h>
#include <signal.h>
void abc();
int main()
{
    signal(SIGINT, abc);
    for (;;)
        ;
}
void abc()
{
    printf("You have pressed Ctrl-C\n");
}

/*
^CYou have pressed Ctrl-C
^CYou have pressed Ctrl-C
^CYou have pressed Ctrl-C
^CYou have pressed Ctrl-C
^CYou have pressed Ctrl-C
^Z
[1]+  Stopped                 ./a.out
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define CHILD_NUMBER 5

void handler(int sig)
{
    printf("PID %d received signal %d\n", getpid(), sig);
}

int main()
{
    printf("Parent %d\n", getpid());
    signal(SIGUSR1, handler);
    for (int i = 0; i < CHILD_NUMBER; i++)
    {
        if (fork() == 0)
        {
            printf("Child %d\n", getpid());
            while (1)
                ;
            printf("Child %d exiting\n", getpid());
        }
    }
    sleep(1);
    kill(getpid(), SIGUSR1);
    return 0;
}

/*
Parent 165175
Child 165177
Child 165180
Child 165179
Child 165176
Child 165178
PID 165175 received signal 10
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define CHILD_NUMBER 5

void handler(int sig)
{
    printf("PID %d received signal %d\n", getpid(), sig);
}

int main()
{
    printf("Parent %d\n", getpid());
    signal(SIGUSR1, handler);
    for (int i = 0; i < CHILD_NUMBER; i++)
    {
        if (fork() == 0)
        {
            printf("Child %d\n", getpid());
            while (1)
                ;
            printf("Child %d exiting\n", getpid());
        }
    }
    sleep(1);
    kill(-getpid(), SIGUSR1);
    return 0;
}

/*
Parent 165369
Child 165374
Child 165371
Child 165370
Child 165373
Child 165372
PID 165373 received signal 10
PID 165374 received signal 10
PID 165371 received signal 10
PID 165370 received signal 10
PID 165372 received signal 10
PID 165369 received signal 10
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define CHILD_NUMBER 5

void handler(int sig)
{
    printf("PID %d received signal %d\n", getpid(), sig);
}

int main()
{
    printf("Parent %d\n", getpid());
    signal(SIGUSR1, handler);
    for (int i = 0; i < CHILD_NUMBER; i++)
    {
        if (fork() == 0)
        {
            printf("Child %d\n", getpid());
            while (1)
                ;
            printf("Child %d exiting\n", getpid());
        }
    }
    sleep(1);
    kill(0, SIGUSR1);
    return 0;
}

/*
Parent 165552
Child 165553
Child 165556
Child 165554
Child 165557
Child 165555
PID 165556 received signal 10
PID 165554 received signal 10
PID 165552 received signal 10
PID 165557 received signal 10
PID 165555 received signal 10
*/