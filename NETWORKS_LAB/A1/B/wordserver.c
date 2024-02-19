/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

/*
   This program implements a UDP server in the wordserver.c file.
   It performs the following steps:

   1. Receives a filename from a client.
   2. Sends the words from the file to the client one by one.
   3. Allows the client to request a specific word by sending the index of the word in the file.
   4. Fulfills the client's request by sending the requested word.
   5. Permits the client to continue requesting words until the server sends the word "END".
   6. After sending the word "END" to the client, the server waits for the next request.
*/

/*
QUESTION: Is this a good file transfer protocol?

ANSWER:

Absence of Reliability:

Datagram sockets lack reliability since they are connectionless and do not ensure guaranteed delivery. 
In the event of packet loss during transmission, there is no mechanism for retransmission, leaving the client susceptible to missing words or the entire file without recovery options.

Limited Error Handling:

While the protocol does handle the case where the requested file is not found, it does not account for other potential errors during the file transfer process. 
For instance, if the server fails to send a word, there's no mechanism to detect and recover from such failures.

Limited Scalability:

Datagram sockets might not be the best choice for large file transfers due to their limitations on payload size

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8181           // Port number for the server
#define BUFFER_SIZE 1024    // Size of the buffer for receiving and sending data
#define MAX_WORD_LENGTH 101 // Maximum length of a word
#define MAX_WORD 10001      // Maximum number of words

// Function to read words from a file and store them in an array
int get_Words_From_File(FILE *fp, char *arr[])
{
    char word[MAX_WORD_LENGTH]; // Temporary storage for a word
    int wordCount = 0;          // Counter for the number of words read from the file

    // Read words from the already opened file and store them in the array
    while (fscanf(fp, "%s", word) == 1)
    {
        // Allocate memory for the word in the array
        arr[wordCount] = (char *)malloc(strlen(word) + 1);

        // Copy the word to the array
        strcpy(arr[wordCount], word);

        // Increment the word count
        wordCount++;
    }

    // Null-terminate the array and close the file
    arr[wordCount] = NULL;
    if (fclose(fp) != 0)
    {
        perror("ERROR! File Closing Failed.\n");
        exit(EXIT_FAILURE); // indicate failure
    }
    return wordCount;
}

// Function to process a word request and return the requested word
char *process_word_request(char *words[], int idx)
{
    return words[idx];
}

// Function to clean up resources and close the socket
void cleanup(int sockfd, char *buffer)
{
    // Free the dynamically allocated buffer
    if (buffer != NULL)
    {
        free(buffer);
    }

    // Check if the socket file descriptor is valid before closing
    if (sockfd != -1)
    {
        // Attempt to close the socket
        if (close(sockfd) == -1)
        {
            perror("ERROR! Closing Socket Failed.\n");
            exit(EXIT_FAILURE); // indicate failure
        }
    }
}

int main()
{
    int sockfd;                             // Socket file descriptor
    struct sockaddr_in cli_addr, serv_addr; // Server and Client address structure

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("ERROR! Socket Creation Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if ((bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        perror("ERROR! Binding Failed.\n");
        // Check if the socket file descriptor is valid before closing
        if (sockfd != -1)
        {
            // Attempt to close the socket
            if (close(sockfd) == -1)
            {
                perror("ERROR! Closing Socket Failed.\n");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_FAILURE);
    }

    // Print information on the terminal
    printf("Server is now active on PORT: %d\n", PORT);
    printf("Waiting for incoming requests...\n");

    int bytes_received = 0;               // Variable to store the number of bytes received
    socklen_t cli_len = sizeof(cli_addr); // Length of the client address structure

    // Allocate memory for the buffer
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // Check if memory allocation for the buffer is successful
    if (buffer == NULL)
    {
        perror("ERROR! Memory Allocation Failed.\n");
        if (sockfd != -1)
        {
            // Attempt to close the socket
            if (close(sockfd) == -1)
            {
                perror("ERROR! Closing Socket Failed.\n");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_FAILURE);

    }

    int request_counter = 0; // Number of client requests received

    for (;;)
    {
        // Receive the filename from the client
        if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &cli_len)) < 0)
        {
            perror("ERROR! recvfrom() Failed.\n");
            cleanup(sockfd, buffer);
            exit(EXIT_FAILURE);
        }

        // Increment number of requests received
        request_counter++;
        printf("\nRequest %d received\n\n", request_counter);

        // Null-terminate the received data and print the client's response
        buffer[bytes_received] = '\0';
        printf("CLIENT:\n\t%s\n", buffer);

        // Open the client requested file
        FILE *file = fopen(buffer, "r");

        if (file == NULL)
        {
            // If file not found, send an error message to the client
            char file_error[110] = "NOTFOUND ";
            strcat(file_error, buffer);
            printf("SERVER:\n\t%s\n", file_error);
            sendto(sockfd, (char *)file_error, strlen(file_error), 0, (struct sockaddr *)&cli_addr, cli_len);
        }
        else
        {
            // Read words from the file and send them to the client one by one
            char *words[MAX_WORD];
            int total_words = get_Words_From_File(file, words);

            // Send the first word
            char *word_to_send = process_word_request(words, 0);
            printf("SERVER:\n\t%s\n", word_to_send);

            sendto(sockfd, word_to_send, strlen(word_to_send), 0, (struct sockaddr *)(&cli_addr), cli_len);

            // Check if the first word is "HELLO"
            if (strcmp(word_to_send, "HELLO"))
            {
                printf("ERROR! Some Unexpected Message Delivered.\n");
                exit(EXIT_FAILURE);
            }

            // Loop to handle subsequent word requests from the client
            while (strcmp(word_to_send, "END") != 0)
            {
                if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &cli_len)) < 0)
                {
                    perror("ERROR! recvfrom() Failed.\n");
                    cleanup(sockfd, buffer);
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received] = '\0';
                printf("CLIENT:\n\t%s\n", buffer);

                int idx;

                // Use sscanf to extract the integer value (word index) from the client request
                sscanf(buffer, "%*[^0-9]%d", &idx);

                // Check if the requested word index is valid
                if (idx >= total_words)
                {
                    printf("ERROR! Invalid Word Request\n");
                    cleanup(sockfd, buffer);
                    exit(EXIT_FAILURE);
                }

                // Send the requested word to the client
                char *word_to_send = process_word_request(words, idx);
                printf("SERVER:\n\t%s\n", word_to_send);
                sendto(sockfd, word_to_send, strlen(word_to_send), 0, (struct sockaddr *)(&cli_addr), cli_len);

                // Check if the requested word is "END" to terminate the loop
                if (strcmp(word_to_send, "END") == 0)
                {
                    break;
                }
            }
        }
    }

    // Clean up resources and close the socket
    cleanup(sockfd, buffer);
    printf("\nTerminating...\n");
    return 0;
}