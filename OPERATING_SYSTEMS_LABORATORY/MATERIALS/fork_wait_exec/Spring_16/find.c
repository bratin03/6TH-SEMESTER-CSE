#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE 100

int findnum(int arr[], int L, int R, int num)
{
    for (int i = L; i <= R; i++)
    {
        if (arr[i] == num)
        {
            return i;
        }
    }
    return 255;
}

int findnum_arr(int arr[], int L, int R, int num, pid_t root_pid)
{
    if (R - L + 1 <= 5)
    {
        int find_in = findnum(arr, L, R, num);
        printf("PID: %d\tPPID: %d\tL: %d  R: %d  Index: %d\n", getpid(), getppid(), L, R, find_in);
        if (getpid() == root_pid)
        {
            return find_in;
        }
        exit(find_in);
    }
    else
    {
        int M = (L + R) / 2;
        pid_t left_child_pid, right_child_pid;
        left_child_pid = fork();
        if (left_child_pid == 0)
        {
            int find_left = findnum_arr(arr, L, M, num, root_pid);
            exit(find_left);
        }
        right_child_pid = fork();
        if (right_child_pid == 0)
        {
            int find_right = findnum_arr(arr, M + 1, R, num, root_pid);
            exit(find_right);
        }

        int left_status, right_status;
        waitpid(left_child_pid, &left_status, 0);
        waitpid(right_child_pid, &right_status, 0);

        int find_seg = 255;
        if (WEXITSTATUS(left_status) != 255)
        {
            find_seg = WEXITSTATUS(left_status);
        }
        if (WEXITSTATUS(right_status) != 255)
        {
            find_seg = WEXITSTATUS(right_status);
        }
        if (getpid() == root_pid)
        {
            return find_seg;
        }
        exit(find_seg);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <path to file> <search integer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Error opening the file");
        return 1;
    }

    int numbers[MAX_SIZE];
    int count = 0;

    // Read integers from the file
    while (fscanf(file, "%d", &numbers[count]) == 1)
    {
        count++;
        if (count == MAX_SIZE)
        {
            printf("Array is full. Cannot read more numbers.\n");
            break;
        }
    }

    // Close the file
    fclose(file);

    // Display the read numbers
    printf("Numbers read from file:\n");
    for (int i = 0; i < count; i++)
    {
        printf("%d\n", numbers[i]);
    }
    int num = atoi(argv[2]);
    int idx = findnum_arr(numbers, 0, count - 1, num, getpid());
    if (idx == 255)
    {
        printf("Element not found in the array.\n");
    }
    else
    {
        printf("Element found at index: %d\n", idx);
    }
    exit(EXIT_SUCCESS);
}