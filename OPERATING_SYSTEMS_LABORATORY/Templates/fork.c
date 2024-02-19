#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
    // Variable to store the process ID
    pid_t pid;

    // Fork a child process
    pid = fork();

    // Check for fork errors
    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Parent process
    if (pid > 0)
    {
        printf("Parent process: My PID is %d\n", getpid());

        // Wait for the specific child process to finish
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);

        // Check for waitpid errors
        if (child_pid < 0)
        {
            perror("Waitpid failed");
            exit(EXIT_FAILURE);
        }

        // Check if the child process terminated successfully
        if (WIFEXITED(status))
        {
            printf("Child process %d terminated with status %d\n", child_pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process %d terminated by signal %d\n", child_pid, WTERMSIG(status));
        }
    }
    // Child process
    else if (pid == 0)
    {
        printf("Child process: My PID is %d, and my parent's PID is %d\n", getpid(), getppid());

        // Perform some task in the child process
        // ...

        // Exit the child process
        exit(EXIT_SUCCESS);
    }

    return 0;
}
