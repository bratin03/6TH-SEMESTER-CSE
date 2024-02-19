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
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_INPUT 1001
#define MAX_SIZE 100001
#define MAX_LEN 10001

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 2 && argc != 3)
    {
        printf("Usage:\n%s <City Name>\n%s <City Name> <Indentation Level>\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    char input_name[MAX_INPUT];
    int num_tab;

    // Set the city name and indentation level based on the provided arguments
    if (argc == 2)
    {
        strcpy(input_name, argv[1]);
        num_tab = 0; // Default indentation level is 0
    }
    else if (argc == 3)
    {
        strcpy(input_name, argv[1]);
        num_tab = atoi(argv[2]); // Convert indentation level to integer
    }

    FILE *fp = fopen("treeinfo.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int found = 0;
    char line[MAX_SIZE];

    // Search for the city in the file
    while (fgets(line, MAX_LEN, fp))
    {
        char *token = strtok(line, " \n");

        if (strcmp(token, input_name) == 0)
        {
            found = 1;
            break; // City found, exit the loop
        }
    }

    fclose(fp);

    // If the city is not found, print an error and exit
    if (!found)
    {
        printf("City %s not found\n", input_name);
        exit(EXIT_FAILURE);
    }

    fp = fopen("treeinfo.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Print indentation spaces and city information
    for (int i = 0; i < num_tab; i++)
    {
        printf("    ");
    }

    printf("%s (%d)\n", input_name, getpid());
    fflush(stdout); // Flush the output buffer to ensure immediate printing

    char num_tab_str[MAX_LEN];
    sprintf(num_tab_str, "%d", num_tab + 1);

    int status;

    // Iterate through the file to find the city and fork child processes
    while (fgets(line, MAX_LEN, fp))
    {
        char *token = strtok(line, " ");
        if (strcmp(token, input_name) == 0)
        {
            token = strtok(NULL, " \n");
            int num_child = atoi(token);

            // Fork child processes for each child of the city
            for (int i = 0; i < num_child; i++)
            {
                token = strtok(NULL, " \n");
                pid_t c_pid = fork();

                if (c_pid < 0)
                {
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }

                if (c_pid == 0)
                {
                    // Child process executes proctree program with appropriate arguments
                    execlp("./proctree", "./proctree", token, num_tab_str, NULL); // Call for child nodes executed
                    perror("execlp failed"); // This line should not be reached if execlp is successful
                    exit(EXIT_FAILURE);
                }

                // Parent waits for each child process to finish
                waitpid(c_pid, &status, 0);
            }
            break; // City found and processed, exit the loop
        }
    }

    fclose(fp);         // Close the file
    exit(EXIT_SUCCESS); // Exit
}
