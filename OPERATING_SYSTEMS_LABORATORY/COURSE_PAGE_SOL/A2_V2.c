#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define SELF 0
#define RUNNING 1
#define FINISHED 2
#define TERMINATED 3
#define SUSPENDED 4
#define KILLED 5

#define MAXCHLD 10

struct procentry
{
    pid_t pid;
    pid_t pgid;
    int state;
    char arg;
};

int termtype;

void ignoreC(int sig)
{
    termtype = TERMINATED;
    printf("\n");
}

void ignoreZ(int sig)
{
    termtype = SUSPENDED;
    printf("\n");
}

void printHelp()
{
    printf("   Command : Action\n");
    printf("      c    : Continue a suspended job\n");
    printf("      h    : Print this help message\n");
    printf("      k    : Kill a suspended job\n");
    printf("      p    : Print the process table\n");
    printf("      q    : Quit\n");
    printf("      r    : Run a new job\n");
}

void printPT(struct procentry PT[], int nchld)
{
    int i;

    printf("NO    PID         PGID        STATUS          NAME\n");
    for (i = 0; i <= nchld; ++i)
    {
        printf("%-6d%-12d%-12d", i, PT[i].pid, PT[i].pgid);
        switch (PT[i].state)
        {
        case SELF:
            printf("SELF            mgr\n");
            break;
        case RUNNING:
            printf("RUNNING         job %c\n", PT[i].arg);
            break;
        case FINISHED:
            printf("FINISHED        job %c\n", PT[i].arg);
            break;
        case TERMINATED:
            printf("TERMINATED      job %c\n", PT[i].arg);
            break;
        case SUSPENDED:
            printf("SUSPENDED       job %c\n", PT[i].arg);
            break;
        case KILLED:
            printf("KILLED          job %c\n", PT[i].arg);
            break;
        default:
            printf("UNKNOWN\n");
        }
    }
}

int runjob(struct procentry PT[], int nchld, char arg[])
{
    int pid;

    arg[0] = 'A' + rand() % 26;
    ++nchld;
    if (nchld > MAXCHLD)
    {
        fprintf(stderr, "Process table is full. Quiting...\n");
        exit(1);
    }
    pid = fork();
    if (pid)
    {
        PT[nchld].pid = pid;
        PT[nchld].pgid = pid;
        PT[nchld].state = RUNNING;
        PT[nchld].arg = arg[0];
        termtype = FINISHED;
        waitpid(pid, NULL, 0);
        PT[nchld].state = termtype;
        if (termtype == TERMINATED)
        {
            kill(pid, SIGINT);
            waitpid(pid, NULL, 0);
        }
        if (termtype == SUSPENDED)
        {
            kill(pid, SIGTSTP);
        }
    }
    else
    {
        setpgid(getpid(), getpid());
        // printf("pid = %d, pgid = %d\n", getpid(), getpgid(getpid()));
        execlp("./job", "job", arg, NULL);
    }
    return nchld;
}

int getjobno(struct procentry PT[], int nchld)
{
    int sfnd, i;

    printf("Suspended jobs: ");
    sfnd = 0;
    for (i = 1; i <= nchld; ++i)
    {
        if (PT[i].state == SUSPENDED)
        {
            if (sfnd)
                printf(", ");
            printf("%d", i);
            ++sfnd;
        }
    }
    if (sfnd == 0)
    {
        printf("None\n");
        return -1;
    }
    printf(" (Pick one): ");
    scanf("%d", &i);
    while (getchar() != '\n')
        ;
    if ((i >= 1) && (i <= nchld) && (PT[i].state == SUSPENDED))
        return i;
    printf("Invalid choice...\n");
    return -1;
}

void killjob(struct procentry PT[], int nchld)
{
    int i;

    i = getjobno(PT, nchld);
    if (i >= 1)
    {
        kill(PT[i].pid, SIGKILL);
        waitpid(PT[i].pid, NULL, 0);
        PT[i].state = KILLED;
    }
}

void contjob(struct procentry PT[], int nchld)
{
    int i;

    i = getjobno(PT, nchld);
    if (i >= 1)
    {
        kill(PT[i].pid, SIGCONT);
        PT[i].state = RUNNING;
        termtype = FINISHED;
        waitpid(PT[i].pid, NULL, 0);
        PT[i].state = termtype;
        if (termtype == TERMINATED)
        {
            kill(PT[i].pid, SIGINT);
            waitpid(PT[i].pid, NULL, 0);
        }
        if (termtype == SUSPENDED)
        {
            kill(PT[i].pid, SIGTSTP);
        }
    }
}

int main()
{
    char cmd, arg[2];
    struct sigaction sa;
    struct procentry PT[MAXCHLD + 1];
    int nchld;

    srand((unsigned int)time(NULL));

    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = ignoreC;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = ignoreC;
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = ignoreZ;
    sigaction(SIGTSTP, &sa, NULL);

    PT[0].pid = getpid();
    PT[0].state = SELF;
    PT[0].pgid = getpgid(PT[0].pid);
    nchld = 0;
    arg[1] = 0;
    arg[0] = 'A' - 1;
    while (1)
    {
        printf("mgr> ");
        cmd = getchar();
        if ((cmd == '\n') || (cmd == -1))
            continue;
        while (getchar() != '\n')
            ;
        if ((cmd == 'c') || (cmd == 'C'))
            contjob(PT, nchld);
        else if ((cmd == 'h') || (cmd == 'H'))
            printHelp();
        else if ((cmd == 'k') || (cmd == 'K'))
            killjob(PT, nchld);
        else if ((cmd == 'p') || (cmd == 'P'))
            printPT(PT, nchld);
        else if ((cmd == 'q') || (cmd == 'Q'))
            break;
        else if ((cmd == 'r') || (cmd == 'R'))
            nchld = runjob(PT, nchld, arg);
        else
            printf("Command should be one of c, h, k, p, q, r\n");
    }
    exit(0);
}