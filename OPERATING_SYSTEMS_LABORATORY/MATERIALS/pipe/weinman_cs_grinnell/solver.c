#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

#define MAX_NAME_LENGTH 16
#define MAX_LINE_LENGTH 64

// Function prototypes
void child1(int pipe_fd[2]);
void child2(int pipe_fd[2]);

int main()
{
    FILE *file;
    char line[MAX_LINE_LENGTH];

    // Open the file for reading
    file = fopen("Iowa-cities.dat", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Create pipes for communication with children
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("Error creating pipes");
        exit(EXIT_FAILURE);
    }

    // Fork the first child
    pid_t pid1 = fork();
    if (pid1 == -1)
    {
        perror("Error forking child 1");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        // In the first child process
        close(pipe1[1]); // Close the write end of the pipe
        close(pipe2[0]); // Close the read end of the second pipe
        close(pipe2[1]); // Close the write end of the second pipe
        child1(pipe1);
        exit(EXIT_SUCCESS);
    }

    // Fork the second child
    pid_t pid2 = fork();
    if (pid2 == -1)
    {
        perror("Error forking child 2");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0)
    {
        // In the second child process
        close(pipe2[1]); // Close the write end of the pipe
        close(pipe1[0]); // Close the read end of the first pipe
        close(pipe1[1]); // Close the read end of the second pipe
        child2(pipe2);
        exit(EXIT_SUCCESS);
    }

    // In the parent process
    close(pipe1[0]); // Close the read end of the first pipe
    close(pipe2[0]); // Close the read end of the second pipe

    // Read data from the file and send it to both children
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // printf("Parent Line: %s\n", line);
        write(pipe1[1], line, sizeof(line));
        write(pipe2[1], line, sizeof(line));
        // char *token = strtok(line, ",\n");
        // char buffer1[1000];
        // char buffer2[1000];
        // strcpy(buffer1, "");
        // strcpy(buffer2, "");

        // int count = 0;
        // while (token != NULL)
        // {
        //     printf("Parent: %s\n", token);
        //     if (count == 1)
        //     {
        //         strcat(buffer2, token);
        //         strcat(buffer2, " ");
        //         printf("Parent token: %s\n", token);
        //     }
        //     if (count != 2)
        //     {
        //         strcat(buffer1, token);
        //         strcat(buffer1, " ");
        //         printf("Parent token: %s\n", token);
        //     }
        //     token = strtok(NULL, ",\n");
        //     count++;
        // }
        // printf("Parent Buffer1: %s\n", buffer1);
        // printf("Parent Buffer2: %s\n", buffer2);
        // strcat(buffer1, "\0");
        // strcat(buffer2, "\0");

        // // write(pipe1[1], buffer1, sizeof(buffer1));
        // // write(pipe2[1], buffer2, sizeof(buffer2));
        // strcpy(buffer1, "");
        // strcpy(buffer2, "");
    }

    // Close write ends of pipes to signal end of input to children
    close(pipe1[1]);
    close(pipe2[1]);

    // Wait for both children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Close the file
    fclose(file);

    return 0;
}

void child1(int pipe_fd[2])
{
    char line[MAX_LINE_LENGTH];
    char *token;
    char cityName[MAX_NAME_LENGTH];
    char cityWithMaxDecrease[MAX_NAME_LENGTH];
    int pop2000, pop1980, pop1990, maxDecrease = INT_MIN;
    close(pipe_fd[1]); // Close the write end of the pipe

    while (read(pipe_fd[0], line, sizeof(line)) > 0)
    {
        // printf("Child 1: %s\n", line);

        // Parse the line into tokens
        token = strtok(line, ",");
        strncpy(cityName, token, MAX_NAME_LENGTH);

        token = strtok(NULL, ",");
        pop2000 = atoi(token);

        token = strtok(NULL, ",");
        pop1990 = atoi(token);

        token = strtok(NULL, ",");
        pop1980 = atoi(token);

        // Calculate percentage decrease
        int decrease = ((pop1980 - pop2000));
        // printf("Child 1: %d City: %s Pop2000: %d Pop1980: %d\n", decrease, cityName, pop2000, pop1980);
        //  Update max decrease and corresponding city
        if (decrease > maxDecrease)
        {
            maxDecrease = decrease;
            strcpy(cityWithMaxDecrease, cityName);
        }
    }

    printf("City with the largest decrease: %s\n", cityWithMaxDecrease);

    close(pipe_fd[0]); // Close the read end of the pipe
}

void child2(int pipe_fd[2])
{
    char line[MAX_LINE_LENGTH];
    int pop2000, ruralPopulation = 0;
    const int totalPopulation2000 = 2926324;

    close(pipe_fd[1]); // Close the write end of the pipe

    while (read(pipe_fd[0], line, sizeof(line)) > 0)
    {
        //printf("Child 2: %s\n", line);
        //   Parse the line into tokens
        strtok(line, ","); // Ignore city name
        char *token = strtok(NULL, ",");
        //printf("Child 2 token: %s\n", token);
        pop2000 = atoi(token);
        //printf("Child 2 : %d\n", pop2000);
        // printf("Child 2: %d\n", pop2000);
        //  Accumulate rural population
        ruralPopulation += (pop2000 > 0) ? (pop2000) : 0;
    }

    // Calculate population outside the 60 largest cities
    int populationOutsideCities = totalPopulation2000 - ruralPopulation;

    printf("Population outside the 60 largest cities in 2000: %d\n", populationOutsideCities);

    close(pipe_fd[0]); // Close the read end of the pipe
}