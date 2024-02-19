#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ARRAY_SIZE 100

// Function to find the maximum value in an array
int findMax(int arr[], int L, int R)
{
    int max = arr[L];
    for (int i = L + 1; i <= R; ++i)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }
    return max;
}

// Recursive function to create a tree of processes
int computeMax(int arr[], int L, int R)
{
    if (R - L < 10)
    {
        // Chunk size is less than 10, compute and print the maximum
        int max = findMax(arr, L, R);
        printf("PID: %d, Parent PID: %d, Max: %d\n", getpid(), getppid(), max);
        exit(max);
    }
    else
    {
        // Chunk size is 10 or more, create two child processes
        int M = (L + R) / 2;
        pid_t left_child_pid, right_child_pid;

        left_child_pid = fork();
        if (left_child_pid == 0)
        {
            // Left child process
            int left_max = computeMax(arr, L, M);
            exit(left_max);
        }

        right_child_pid = fork();
        if (right_child_pid == 0)
        {
            // Right child process
            int right_max = computeMax(arr, M + 1, R);
            exit(right_max);
        }

        // Wait for both child processes to finish
        int left_status, right_status;
        waitpid(left_child_pid, &left_status, 0);
        waitpid(right_child_pid, &right_status, 0);

        // Find and print the maximum of the two child processes' results
        int max = (WEXITSTATUS(left_status) > WEXITSTATUS(right_status)) ? WEXITSTATUS(left_status) : WEXITSTATUS(right_status);
        printf("PID: %d, Parent PID: %d, Max: %d\n", getpid(), getppid(), max);
        exit(max);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <array_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 0 || n > MAX_ARRAY_SIZE)
    {
        fprintf(stderr, "Invalid array size. Must be a positive integer up to %d.\n", MAX_ARRAY_SIZE);
        exit(EXIT_FAILURE);
    }

    // Seed for random number generation
    srand((unsigned int)time(NULL));

    // Generate an array of size n with random values
    int arr[MAX_ARRAY_SIZE];
    for (int i = 0; i < n; ++i)
    {
        arr[i] = rand() % 128; // Values in the range 0–127
    }

    // Print the initially unsorted array
    printf("Initially Unsorted Array:\n");
    for (int i = 0; i < n; ++i)
    {
        printf("%d ", arr[i]);
    }
    printf("\n\n");

    // Compute the maximum value using a tree of processes
    int max = computeMax(arr, 0, n - 1);

    // Print the final maximum value computed by the root process
    printf("\nFinal Max: %d\n", max);

    return 0;
}
