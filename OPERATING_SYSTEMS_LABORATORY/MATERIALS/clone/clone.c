#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024) // 1MB stack size for the child process

int shared_variable = 0; // Shared variable between parent and child

// Function to be executed by the child process
int child_function(void *arg)
{
    printf("Child process: PID = %ld\n", (long)getpid());
    printf("Child process: Modifying shared variable...\n");
    shared_variable = 100; // Modify the shared variable
    printf("Child process: Shared variable modified to %d\n", shared_variable);
    return 0;
}

int main()
{
    char *stack;     // Stack for the child process
    char *stack_top; // Top of the stack for the child process
    pid_t child_pid;

    // Allocate memory for the child's stack
    stack = malloc(STACK_SIZE);
    if (stack == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    stack_top = stack + STACK_SIZE; // Calculate the top of the stack

    printf("Parent process: PID = %ld\n", (long)getpid());
    printf("Parent process: Shared variable = %d\n", shared_variable);

    // Create a child process using clone() with CLONE_VM flag
    child_pid = clone(child_function, stack_top, CLONE_VM | SIGCHLD, NULL);
    if (child_pid == -1)
    {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    // Wait for the child process to terminate
    if (waitpid(child_pid, NULL, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    printf("Parent process: Child terminated\n");
    printf("Parent process: Shared variable = %d\n", shared_variable);

    // Free the allocated stack memory
    free(stack);

    return 0;
}

// CLONE_FS
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>

#define STACK_SIZE (1024 * 1024) // 1MB stack size for the child process

// Function to be executed by the child process
int child_function(void *arg)
{
    printf("Child process: PID = %ld\n", (long)getpid());
    printf("Child process: Parent PID = %ld\n", (long)getppid());
    printf("Child process: Argument passed = %s\n", (char *)arg);

    // Change the directory in the child process
    if (chdir("/tmp") == -1)
    {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    printf("Child process: Current directory changed to /tmp\n");

    // Child process terminates
    return 0;
}

int main()
{
    char *stack;     // Stack for the child process
    char *stack_top; // Top of the stack for the child process
    pid_t child_pid;

    // Allocate memory for the child's stack
    stack = malloc(STACK_SIZE);
    if (stack == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    stack_top = stack + STACK_SIZE; // Calculate the top of the stack

    printf("Parent process: PID = %ld\n", (long)getpid());

    // Create a child process using clone() with CLONE_FS flag
    child_pid = clone(child_function, stack_top, CLONE_VM | CLONE_FS | SIGCHLD, "Hello from child");
    if (child_pid == -1)
    {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    printf("Parent process: Child PID = %ld\n", (long)child_pid);

    // Wait for the child process to terminate
    if (waitpid(child_pid, NULL, 0) == -1)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    printf("Parent process: Child terminated\n");

    // Free the allocated stack memory
    free(stack);

    return 0;
}
