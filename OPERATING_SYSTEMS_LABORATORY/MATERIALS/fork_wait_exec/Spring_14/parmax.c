#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// int compareIntegers(const void *a, const void *b)
// {
//     return (*(int *)a - *(int *)b);
// }

int findMax(int arr[], int L, int R)
{
    int max = arr[L];
    for (int i = L + 1; i < R; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }
    return max;
}

int computeMax(int arr[], int L, int R, pid_t root_pid)
{
    if (R - L + 1 < 10)
    {
        int max = findMax(arr, L, R);
        printf("PID: %d\tParnet PID: %d\tMax: %d\n", getpid(), getppid(), max);
        if (getpid() == root_pid)
        {
            return max;
        }
        exit(max);
    }
    else
    {
        int M = (L + R) / 2;
        pid_t left_child_pid, right_child_pid;
        left_child_pid = fork();
        if (left_child_pid == 0)
        {
            int left_max = computeMax(arr, L, M, root_pid);
            exit(left_max);
        }
        right_child_pid = fork();
        if (right_child_pid == 0)
        {
            int right_max = computeMax(arr, M + 1, R, root_pid);
            exit(right_max);
        }

        int left_status, right_status;
        waitpid(left_child_pid, &left_status, 0);
        waitpid(right_child_pid, &right_status, 0);

        int max = (WEXITSTATUS(left_status) > WEXITSTATUS(right_status)) ? WEXITSTATUS(left_status) : WEXITSTATUS(right_status);
        printf("PID: %d\tParnet PID: %d\tMax: %d\n", getpid(), getppid(), max);
        if (getpid() == root_pid)
        {
            return max;
        }
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
    if (n <= 0)
    {
        fprintf(stderr, "Invalid Array Size!\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    int *A = (int *)malloc(sizeof(A) * n);
    for (int i = 0; i < n; i++)
    {
        A[i] = rand() % 128;
    }

    printf("Initially Unsorted Array:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }

    printf("\n\n");
    pid_t root_pid = getpid();
    int max = computeMax(A, 0, n - 1, root_pid);
    printf("\n\nMax: %d\n\n", max);
    // qsort(A, n, sizeof(int), compareIntegers);
    // printf("A[n-1] = %d\n", A[n - 1]);
}