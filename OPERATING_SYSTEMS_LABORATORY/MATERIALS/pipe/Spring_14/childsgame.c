#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#define MIN 0
#define MAX 1

int scoreC = 0, scoreD = 0;
int winner = -1;

// Signal handler for child C
void handle_signal_c(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("C: I won! :-)\n");
        exit(EXIT_SUCCESS);
    }
    if (signum == SIGUSR2)
    {
        printf("C: I lost! :-(\n");
        exit(EXIT_SUCCESS);
    }
}

// Signal handler for child D
void handle_signal_d(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("D: I won! :-)\n");
        exit(EXIT_SUCCESS);
    }
    if (signum == SIGUSR2)
    {
        printf("D: I lost! :-(\n");
        exit(EXIT_SUCCESS);
    }
}

int main()
{
    // Seed for random number generation
    srand(time(NULL));

    // Pipes for communication
    int pipePC[2], pipePD[2];

    // Process IDs for child processes
    pid_t childC, childD;

    // Check for pipe creation errors
    if (pipe(pipePC) == -1 || pipe(pipePD) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create child C
    if ((childC = fork()) == -1)
    {
        perror("Fork failed for C");
        exit(EXIT_FAILURE);
    }

    if (childC == 0)
    {
        // Child C process
        close(pipePC[0]);
        close(pipePD[0]);
        close(pipePD[1]);

        // Set signal handlers for child C
        signal(SIGUSR1, handle_signal_c);
        signal(SIGUSR2, handle_signal_c);
        signal(SIGINT, handle_signal_c);

        // Initialize random number generation for child C
        srand(time(NULL) + getpid() + 1);

        // Infinite loop to generate and write random numbers
        while (1)
        {
            int randNum = rand() % 100 + 1;
            write(pipePC[1], &randNum, sizeof(int));
            pause(); // Wait for signal
        }
    }

    // Create child D
    if ((childD = fork()) == -1)
    {
        perror("Fork failed for D");
        exit(EXIT_FAILURE);
    }

    if (childD == 0)
    {
        // Child D process
        close(pipePC[0]);
        close(pipePD[0]);

        // Set signal handlers for child D
        signal(SIGUSR1, handle_signal_d);
        signal(SIGUSR2, handle_signal_d);
        signal(SIGINT, handle_signal_d);

        // Close unused pipe end
        close(pipePC[1]);

        // Initialize random number generation for child D
        srand(time(NULL) + getpid() + 1);

        // Infinite loop to generate and write random numbers
        while (1)
        {
            int randNum = rand() % 100 + 1;
            write(pipePD[1], &randNum, sizeof(int));
            pause(); // Wait for signal
        }
    }

    // Close unused pipe ends in the parent process
    close(pipePC[1]);
    close(pipePD[1]);

    // Main game loop
    while (1)
    {
        int choice = rand() % 2;
        int c, d;

        // Read random numbers from child processes
        read(pipePC[0], &c, sizeof(int));
        read(pipePD[0], &d, sizeof(int));

        // Print the choices of C and D
        printf("\nC chooses %d, D chooses %d\n\n", c, d);

        // Determine the game strategy based on 'choice' (MIN or MAX)
        if (choice == MIN)
        {
            // Player chooses MIN
            printf("P chooses MIN.\n");
            if (c < d)
            {
                // C wins the round
                scoreC++;
                printf("C gets a point! Scores: C=%d, D=%d\n", scoreC, scoreD);
            }
            else if (d < c)
            {
                // D wins the round
                scoreD++;
                printf("D gets a point! Scores: C=%d, D=%d\n", scoreC, scoreD);
            }
        }
        else
        {
            // Player chooses MAX
            printf("P chooses MAX.\n");
            if (c > d)
            {
                // C wins the round
                scoreC++;
                printf("C gets a point! Scores: C=%d, D=%d\n", scoreC, scoreD);
            }
            else if (d > c)
            {
                // D wins the round
                scoreD++;
                printf("D gets a point! Scores: C=%d, D=%d\n", scoreC, scoreD);
            }
        }

        // Ignore the round if choices are equal
        if (c == d)
        {
            printf("This round is ignored.\n");
        }

        // Check for game end conditions
        if (scoreC == 10 || scoreD == 10)
        {
            winner = (scoreC == 10) ? 0 : 1;
            printf("Child %c has reached 10 points. Game over!\n", (winner == 0) ? 'C' : 'D');
            if (winner == 0)
            {
                kill(childC, SIGUSR1);
                sleep(1);
                kill(childD, SIGUSR2);
            }
            else
            {
                kill(childD, SIGUSR1);
                sleep(1);
                kill(childC, SIGUSR2);
            }
            exit(EXIT_SUCCESS);
        }

        // Sleep for 1 second before the next round
        sleep(1);

        // Send SIGINT to both child processes to make them pause
        kill(childC, SIGINT);
        kill(childD, SIGINT);
    }

    // Wait for child processes to finish
    wait(NULL);
    wait(NULL);

    exit(EXIT_SUCCESS);
}
