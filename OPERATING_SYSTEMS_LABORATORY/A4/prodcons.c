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
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Seed the random number generator using current time
    srand(time(NULL));

    // Check if the correct number of command-line arguments are provided
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <number_of_consumers> <number_of_items>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract the number of consumers (n) and number of items (t) from command-line arguments
    int n = atoi(argv[1]), t = atoi(argv[2]);

    // Shared memory ID
    int shmid;

    // Create a shared memory segment capable of storing two integers
    shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), 0777 | IPC_CREAT);

    // Check for errors in creating shared memory
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    int *M = (int *)shmat(shmid, NULL, 0);
    if (M == (int *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize M[0] to 0 (indicating no item available for consumption at the beginning)
    M[0] = 0;

    int i;

    // Fork child processes for consumers
    for (i = 1; i <= n; i++)
    {
        pid_t pid = fork();

        // Check for errors in forking
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Child process (Consumer)
        if (pid == 0)
        {
            printf("\t\t\t\t\tConsumer %d is alive\n", i);

            // Initialize counters for items read and their sum
            int itemsRead = 0;
            int sum = 0;

            // Infinite loop for consumer to read items
            while (1)
            {
                // Busy wait until M[0] becomes i or -1
                while (M[0] != i && M[0] != -1)
                    ;

                // If M[0] is i, read the item from M[1]
                if (M[0] == i)
                {
                    int item = M[1];

                    // Update counters
                    itemsRead++;
                    sum += item;

#ifdef VERBOSE
                    printf("\t\t\t\t\tConsumer %d reads %d\n", i, item);
#endif
                    // Set M[0] to 0 to indicate consumption
                    M[0] = 0;
                }
                // If M[0] is -1, break from the loop
                else if (M[0] == -1)
                {
                    break;
                }
            }

            // Print statistics for the consumer
            printf("\t\t\t\t\tConsumer %d has read %d items: Checksum = %d\n", i, itemsRead, sum);

            // Detach shared memory and exit the child process
            shmdt(M);
            exit(EXIT_SUCCESS);
        }
    }

    // Arrays to store count and sum for each consumer
    int count[n + 1];
    int sum[n + 1];

    // Initialize arrays with zeros
    memset(count, 0, sizeof(count));
    memset(sum, 0, sizeof(sum));

    // Print a message indicating the producer is alive
    printf("Producer is alive\n");

    // Production loop for producing t items
    for (i = 0; i < t; i++)
    {
        // Generate a random item and consumer
        int randomNumber = rand() % 900 + 100;
        int c = rand() % n + 1;

        // Busy wait until M[0] becomes 0
        while (M[0] != 0)
            ;

#ifdef VERBOSE
        printf("Produce produces %d for Consumer %d\n", randomNumber, c);
#endif
        // Set M[0] to c
        M[0] = c;

#ifdef SLEEP
        // Optional delay if SLEEP is set
        usleep(rand() % 10 + 1);
#endif

        // Set M[1] to the random number
        M[1] = randomNumber;

        // Update count and sum arrays
        count[c]++;
        sum[c] += randomNumber;
    }

    // Wait until M[0] becomes 0 (the last item is consumed)
    while (M[0] != 0)
        ;

    // Set M[0] to -1
    M[0] = -1;

    // Wait for all child processes (consumers) to terminate
    for (i = 1; i <= n; i++)
    {
        wait(NULL);
    }

    // Print final statistics for the producer
    printf("Producer has produced %d items\n", t);

    // Print statistics for each consumer
    for (i = 1; i <= n; i++)
    {
        printf("%d items for Consumer %d: Checksum = %d\n", count[i], i, sum[i]);
    }

    // Detach and remove the shared memory segment
    shmdt(M);
    shmctl(shmid, IPC_RMID, NULL);

    // Exit the program
    exit(EXIT_SUCCESS);
}
