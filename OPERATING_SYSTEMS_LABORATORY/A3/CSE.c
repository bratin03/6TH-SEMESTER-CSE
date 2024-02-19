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

// Define constants for buffer size, integer size, and maximum number of command-line arguments
#define BUFFER_SIZE 100 // Maximum size of character buffers
#define INT_SIZE 64     // Maximum size of integer-related buffers
#define MAX_ARG 100     // Maximum number of command-line arguments

// Function prototype declarations
void child(char *argv[]);

// Main function
int main(int argc, char *argv[])
{
    // Check if the program is run without any command-line arguments
    if (argc == 1)
    {
        // Declare file descriptors for two pipes
        int fd_child[2], fd_child_swap[2];

        // Create two pipes
        pipe(fd_child);
        pipe(fd_child_swap);

        // Convert file descriptors to strings for passing them as arguments to child processes
        char fd_Read[INT_SIZE];
        sprintf(fd_Read, "%d", fd_child[0]);
        char fd_Write[INT_SIZE];
        sprintf(fd_Write, "%d", fd_child[1]);
        char fd_Read_swap[INT_SIZE];
        sprintf(fd_Read_swap, "%d", fd_child_swap[0]);
        char fd_Write_swap[INT_SIZE];
        sprintf(fd_Write_swap, "%d", fd_child_swap[1]);

        // Display information about the supervisor mode
        printf("+++ CSE in Supervisor Mode : Started\n");
        printf("+++ CSE in Supervisor Mode : pfd = [%d %d]\n", fd_child[0], fd_child[1]);

        // Fork the first child in command-input mode using xterm
        printf("+++ CSE in Supervisor Mode : Forking first child in command-input mode\n");
        if (fork() == 0)
        {
            execlp("xterm", "xterm", "-T", "First Child", "-e", "./CSE", fd_Read, fd_Write, fd_Read_swap, fd_Write_swap, "Child_1", NULL);
        }
        else
        {
            // Fork the second child in execute mode using xterm
            printf("+++ CSE in Supervisor Mode : Forking second child in execute mode\n");
            if (fork() == 0)
            {
                execlp("xterm", "xterm", "-T", "Second Child", "-e", "./CSE", fd_Read, fd_Write, fd_Read_swap, fd_Write_swap, "Child_2", NULL);
            }
        }

        // Wait for both child processes to terminate
        wait(NULL);
        wait(NULL);
        printf("+++ CSE in supervisor mode : First child terminated\n+++ CSE in supervisor mode: Second child terminated\n");
    }
    // Check if the program is run with specific command-line arguments for the first child
    else
    {
        child(argv);
    }
}

/*
 * Function to manage the behavior of child processes (Child_1 or Child_2).
 * The behavior is determined based on the command-line argument specifying the child type.
 */
void child(char *argv[])
{
    // Determine the mode of operation based on the child type:
    //   - If the process is Child_1, set mode to 0 (command-input mode).
    //   - If the process is Child_2, set mode to 1 (execute mode).
    int mode = (strcmp(argv[5], "Child_1") == 0) ? 0 : 1;

    // Duplicate standard input and standard output file descriptors for later restoration
    int std_Read = dup(0);
    int std_Write = dup(1);

    // Initialize pipe file descriptors based on the child type
    int pipe_Read, pipe_Write;

    if (mode == 0)
    {
        pipe_Read = atoi(argv[3]);
        pipe_Write = atoi(argv[2]);
    }
    else
    {
        pipe_Read = atoi(argv[1]);
        pipe_Write = atoi(argv[4]);
    }

    // Infinite loop to continuously handle user input
    while (1)
    {
        fflush(stderr);

        // Command-input mode
        if (mode == 0)
        {
            // Display prompt for user input
            const char *prompt = "Enter command> ";
            fprintf(stderr, "%s", prompt);
            fflush(stderr);

            // Read user input into a buffer
            char buffer[BUFFER_SIZE];
            fgets(buffer, sizeof(buffer), stdin);
            size_t len = strlen(buffer);

            // Check if the input is not empty and doesn't end with a newline character
            if (len > 0 && buffer[len - 1] != '\n')
            {
                // Append a newline character to the buffer if it is not already present
                if (len < BUFFER_SIZE - 1)
                {
                    buffer[len] = '\n';
                    buffer[len + 1] = '\0';
                }
            }

            // Check if the user input is the "swaprole" command
            if (strcmp(buffer, "swaprole\n") == 0)
            {
                // Toggle the mode between command-input and execute modes
                mode = 1 - mode;
            }

            // Redirect standard output to the write end of the pipe and print the user input
            close(1);
            dup(pipe_Write);
            fprintf(stdout, "%s", buffer);
            fflush(stdout);
            close(1);
            dup(std_Write);

            // Check if the user input is the "exit" command and exit the process if true
            if (strcmp(buffer, "exit\n") == 0)
            {
                exit(EXIT_SUCCESS);
            }
        }
        // Execute mode
        else
        {
            // Display prompt for waiting for a command in execute mode
            const char *prompt = "Waiting for command> ";
            fprintf(stderr, "%s", prompt);
            fflush(stderr);

            // Read command from the pipe and print it to stderr
            char buffer[BUFFER_SIZE];
            close(0);
            dup(pipe_Read);
            fgets(buffer, BUFFER_SIZE, stdin);
            close(0);
            dup(std_Read);

            // Remove newline character from the command if present
            char *newline = strchr(buffer, '\n');
            if (newline != NULL)
            {
                *newline = '\0';
            }

            // Check if the received command is the "swaprole" command
            if (strcmp(buffer, "swaprole") == 0)
            {
                // Toggle the mode between command-input and execute modes
                mode = 1 - mode;
                fprintf(stderr, "%s", buffer);
                fprintf(stderr, "%s", "\n");
                continue;
            }

            // Check if the received command is the "exit" command and exit the process if true
            if (strcmp(buffer, "exit") == 0)
            {
                fprintf(stderr, "%s", buffer);
                fflush(stderr);

                exit(EXIT_SUCCESS);
            }

            // Print the received command to stderr
            fprintf(stderr, "%s", buffer);
            fflush(stderr);

            // Remove newline character from the command if present
            size_t length = strlen(buffer);
            if (length > 0 && buffer[length - 1] == '\n')
            {
                buffer[length - 1] = '\0';
            }

            // Tokenize the command into arguments
            char *arg[MAX_ARG];
            for (int i = 0; i < MAX_ARG; i++)
            {
                arg[i] = NULL;
            }

            char *token = strtok(buffer, " \n");
            int i = 0;

            // Store the tokenized arguments in an array
            while (token != NULL && i < MAX_ARG)
            {
                arg[i] = strdup(token);
                token = strtok(NULL, " \n");
                i++;
            }

            // Print a newline character to stderr
            fprintf(stderr, "%s", "\n");

            // Fork a child process to execute the command
            if (fork() == 0)
            {
                // Redirect standard input and output to the original file descriptors
                close(0);
                dup(std_Read);
                close(1);
                dup(std_Write);

                // Execute the command using execvp
                execvp(arg[0], arg);

                // Print an error message if execvp fails
                fprintf(stderr, "*** Unable to execute command\n");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }

            // Wait for the child process to complete
            wait(NULL);
        }
    }
}
