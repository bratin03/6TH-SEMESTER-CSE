#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <array_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open an xterm window and execute parmax
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process

        // Construct the command to execute parmax
        execlp("xterm", "xterm", "-e", "./parmax", argv[1], (char *)NULL);

        // exec* functions return only if an error occurs
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        // Wait for the child (parmax) to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            printf("parmax process exited with status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("parmax process terminated by signal %d\n", WTERMSIG(status));
        }

        // Exit the wrapper process
        exit(EXIT_SUCCESS);
    }

    return 0;
}
