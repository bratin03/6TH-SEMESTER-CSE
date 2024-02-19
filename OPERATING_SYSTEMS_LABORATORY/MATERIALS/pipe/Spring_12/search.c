#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 100

void search(int array[], int start, int end, int target, int pipe_fd[])
{
    int index = -1;

    for (int i = start; i <= end; ++i)
    {
        if (array[i] == target)
        {
            index = i;
            break;
        }
    }

    // Send the result back to the parent through the pipe
    write(pipe_fd[1], &index, sizeof(int));
}

int main()
{
    int array[MAX_SIZE];
    int size, target;

    // Read the size of the array
    printf("Enter the size of the array (max. %d): ", MAX_SIZE);
    scanf("%d", &size);

    // Read the array elements
    printf("Enter %d integers for the array:\n", size);
    for (int i = 0; i < size; ++i)
    {
        scanf("%d", &array[i]);
    }

    // Read the target integer to be searched
    printf("Enter the integer to be searched: ");
    scanf("%d", &target);

    // Create a pipe for communication between parent and child processes
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Fork first child process
    pid_t child1 = fork();

    if (child1 == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0)
    {
        // Child process 1
        close(pipe_fd[0]); // Close reading end of the pipe

        // Search the first half of the array
        search(array, 0, size / 2 - 1, target, pipe_fd);

        close(pipe_fd[1]); // Close writing end of the pipe
        exit(EXIT_SUCCESS);
    }

    // Fork second child process
    pid_t child2 = fork();

    if (child2 == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0)
    {
        // Child process 2
        close(pipe_fd[0]); // Close reading end of the pipe

        // Search the second half of the array
        search(array, size / 2, size - 1, target, pipe_fd);

        close(pipe_fd[1]); // Close writing end of the pipe
        exit(EXIT_SUCCESS);
    }

    // Parent process
    close(pipe_fd[1]); // Close writing end of the pipe

    int result1, result2;

    // Read results from child processes
    read(pipe_fd[0], &result1, sizeof(int));
    read(pipe_fd[0], &result2, sizeof(int));

    close(pipe_fd[0]); // Close reading end of the pipe

    // Wait for both child processes to finish
    wait(NULL);
    wait(NULL);

    // Print the search results
    if (result1 != -1)
    {
        printf("The integer %d is found at index %d by process 1.\n", target, result1);
    }
    else
    {
        printf("The integer %d is not found by process 1.\n", target);
    }

    if (result2 != -1)
    {
        printf("The integer %d is found at index %d by process 2.\n", target, result2);
    }
    else
    {
        printf("The integer %d is not found by process 2.\n", target);
    }

    return 0;
}
