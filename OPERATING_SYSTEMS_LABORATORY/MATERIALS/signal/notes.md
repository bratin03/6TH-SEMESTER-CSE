1. 
```c
/*
** sigint.c -- grabs SIGINT
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigint_handler(int sig)
{
	(void)sig; // remove unused variable warning

	write(0, "Ahhh! SIGINT!\n", 14);
}

int main(void)
{
	char s[200];
	struct sigaction sa = {
	    .sa_handler = sigint_handler,
	    .sa_flags = 0, // or SA_RESTART
	};

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("Enter a string:\n");

	if (fgets(s, sizeof s, stdin) == NULL)
		perror("fgets");
	else 
		printf("You entered: %s\n", s);

	return 0;
}
``` 

```txt
Enter a string:
My Name is Bratin M^CAhhh! SIGINT!
fgets: Interrupted system call
```

2. 
```c
/*
** sigint.c -- grabs SIGINT
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void sigint_handler(int sig)
{
    (void)sig; // remove unused variable warning

    write(0, "Ahhh! SIGINT!\n", 14);
}

int main(void)
{
    char s[200];
    struct sigaction sa = {
        .sa_handler = sigint_handler,
        .sa_flags = 0, // or SA_RESTART
        .sa_mask = 0,
    };
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("Enter a string:\n");

    if (fgets(s, sizeof s, stdin) == NULL)
        perror("fgets");
    else
        printf("You entered: %s\n", s);

    return 0;
}
```

```txt
Enter a string:
I am B^CAhhh! SIGINT!
My Name is B^CAhhh! SIGINT!
I am Frustrat^CAhhh! SIGINT!
I am Leaving Now
You entered: I am Leaving Now

```

2.  
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t child_pid;

    if ((child_pid = fork()) == 0)
    {
        // Child process
        printf("Child process: PID=%d, PGID=%d\n", getpid(), getpgrp());
        sleep(5);
        printf("Child process: PID=%d, PGID=%d\n", getpid(), getpgrp());
    }
    else if (child_pid > 0)
    {
        sleep(3);
        // Parent process
        setpgid(child_pid, child_pid); // Set child's PGID to its PID
        printf("Parent process: PID=%d, PGID=%d\n", getpid(), getpgrp());
        wait(NULL); // Wait for the child to finish
    }
    else
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}
```

```txt
Child process: PID=2474, PGID=2473
Parent process: PID=2473, PGID=2473
Child process: PID=2474, PGID=2474
```

3.   
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3

void child_process(int id)
{
    printf("Child %d started (PID=%d) (PGID=%d)\n", id, getpid(), getpgid(0));

    // Simulate some work in the child process
    sleep(2);

    printf("Child %d exiting. (PID=%d) (PGID=%d)\n", id, getpid(), getpgid(0));
    exit(EXIT_SUCCESS);
}

void parent_process(pid_t *child_pids)
{
    printf("Parent process (PID=%d)\n", getpid());

    // Set up process group for the parent and its children
    setpgid(getpid(), getpid());

    // Wait for all children to finish
    for (int i = 0; i < NUM_CHILDREN; ++i)
    {
        int status;
        waitpid(child_pids[i], &status, 0);
        printf("Child %d exited with status %d\n", i, WEXITSTATUS(status));
    }

    printf("Parent process exiting\n");
}

void signal_handler(int signo)
{
    // Signal handler for the parent process
    if (signo == SIGUSR1)
    {
        printf("PID %d received SIGUSR1\n", getpid());
    }
}

int main()
{
    printf("Parent PID: %d\n", getpid());
    pid_t child_pids[NUM_CHILDREN];

    // Set up signal handler for the parent process
    signal(SIGUSR1, signal_handler);

    // Spawn child processes
    for (int i = 0; i < NUM_CHILDREN; ++i)
    {
        pid_t child_pid = fork();

        if (child_pid == 0)
        {
            // In child process
            child_process(i);
        }
        else if (child_pid > 0)
        {
            // In parent process
            child_pids[i] = child_pid;
        }
        else
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Allow some time for child processes to start
    sleep(1);

    // Send a signal to the entire process group
    kill(-getpgid(getpid()), SIGUSR1);

    // Parent process waits for children and handles signals
    parent_process(child_pids);

    return 0;
}
```

```txt
Parent PID: 3278
Child 0 started (PID=3279) (PGID=3278)
Child 1 started (PID=3280) (PGID=3278)
Child 2 started (PID=3281) (PGID=3278)
PID 3280 received SIGUSR1
PID 3281 received SIGUSR1
PID 3278 received SIGUSR1
Child 2 exiting. (PID=3281) (PGID=3278)
Parent process (PID=3278)
PID 3279 received SIGUSR1
Child 0 exiting. (PID=3279) (PGID=3278)
Child 1 exiting. (PID=3280) (PGID=3278)
Child 0 exited with status 0
Child 1 exited with status 0
Child 2 exited with status 0
Parent process exiting
```

4.  

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3

void child_process(int id)
{
    printf("Child %d started (PID=%d) (PGID=%d)\n", id, getpid(), getpgid(0));

    // Simulate some work in the child process
    sleep(2);

    printf("Child %d exiting. (PID=%d) (PGID=%d)\n", id, getpid(), getpgid(0));
    exit(EXIT_SUCCESS);
}

void parent_process(pid_t *child_pids)
{
    printf("Parent process (PID=%d)\n", getpid());

    // Set up process group for the parent and its children
    setpgid(getpid(), getpid());

    // Wait for all children to finish
    for (int i = 0; i < NUM_CHILDREN; ++i)
    {
        int status;
        waitpid(child_pids[i], &status, 0);
        printf("Child %d exited with status %d\n", i, WEXITSTATUS(status));
    }

    printf("Parent process exiting\n");
}

void signal_handler(int signo)
{
    // Signal handler for the parent process
    if (signo == SIGUSR1)
    {
        printf("PID %d received SIGUSR1\n", getpid());
    }
}

int main()
{
    printf("Parent PID: %d\n", getpid());
    pid_t child_pids[NUM_CHILDREN];

    // Set up signal handler for the parent process
    signal(SIGUSR1, signal_handler);

    // Spawn child processes
    for (int i = 0; i < NUM_CHILDREN; ++i)
    {
        pid_t child_pid = fork();

        if (child_pid == 0)
        {
            // In child process
            child_process(i);
        }
        else if (child_pid > 0)
        {
            // In parent process
            child_pids[i] = child_pid;
        }
        else
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Allow some time for child processes to start
    sleep(1);

    // Send a signal to the entire process group
    kill(getpgid(getpid()), SIGUSR1);

    // Parent process waits for children and handles signals
    parent_process(child_pids);

    return 0;
}
```

```txt
Parent PID: 3462
Child 0 started (PID=3463) (PGID=3462)
Child 1 started (PID=3464) (PGID=3462)
Child 2 started (PID=3465) (PGID=3462)
PID 3462 received SIGUSR1
Parent process (PID=3462)
Child 0 exiting. (PID=3463) (PGID=3462)
Child 1 exiting. (PID=3464) (PGID=3462)
Child 2 exiting. (PID=3465) (PGID=3462)
Child 0 exited with status 0
Child 1 exited with status 0
Child 2 exited with status 0
Parent process exiting
```
