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
   This program serves as the client for the wordserver.c program.
   It performs the following steps:

   1. Accepts a filename as a command-line argument.
   2. Sends the filename to the server.
   3. The server sequentially sends words from the file to the client.
   4. The client writes these words to a new file.
   5. The new file has the same name as the original file, but with "_copy" appended.
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
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <.txt file name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract the filename from the command-line argument
    char *filename = argv[1];     // Buffer for storing filename
    int sockfd, err;              // Socket file descriptor
    struct sockaddr_in serv_addr; // Server address structure

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("ERROR! Socket Creation Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address to binary form and store it in serv_addr structure
    if ((err = inet_aton("127.0.0.1", &serv_addr.sin_addr)) == 0)
    {
        perror("ERROR! IP Coversion Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the buffer to receive data
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char)); // Buffer for receiving data
    if (buffer == NULL)
    {
        perror("ERROR! Memory Allocation Failed.\n"); // Check if the socket file descriptor is valid before closing
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

    // Print the client information and filename
    printf("CLIENT:\n\t%s\n", filename);

    // Send the filename to the server using UDP
    sendto(sockfd, (const char *)filename, strlen(filename), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

    int bytes_received;                     // Variable to store the number of bytes received
    socklen_t serv_len = sizeof(serv_addr); // Length of the server address structure

    // Receive the response from the server
    if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &serv_len)) < 0)
    {
        perror("ERROR! recvfrom() Failed.\n");
        cleanup(sockfd, buffer);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the received data and print the server's response
    buffer[bytes_received] = '\0';
    printf("SERVER:\n\t%s\n", buffer);

    // Check if the server responded with "HELLO"
    if (strcmp(buffer, "HELLO") == 0)
    {
        char newfile[MAX_WORD_LENGTH + 10];
        // Extract the file name and extension from the original filename
        char *file_name = strtok(filename, ".");
        char *file_extension = strtok(NULL, ".");

        // Concatenate the new file name with "_copy" and the original file extension
        sprintf(newfile, "%s_copy.%s", file_name, file_extension);

        FILE *newFile = fopen(newfile, "w");

        // Check if the new file is created successfully
        if (newFile == NULL)
        {
            perror("ERROR! New File Creation Failed.\n");
            cleanup(sockfd, buffer);
            exit(EXIT_FAILURE);
        }

        int counter = 1; // Keep track of word request to be sent

        // Loop to receive and write words to the new file
        while (counter < MAX_WORD && strcmp(buffer, "END"))
        {
            char word[MAX_WORD] = "WORD";
            char idx[MAX_WORD];
            sprintf(idx, "%d", counter);
            strcat(word, idx);
            printf("CLIENT:\n\t%s\n", word);

            // Send the word to the server using UDP
            sendto(sockfd, (const char *)word, strlen(word), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

            // Receive the response from the server
            if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &serv_len)) < 0)
            {
                perror("ERROR! recvfrom() Failed.\n");
                cleanup(sockfd, buffer);
                exit(EXIT_FAILURE);
            }

            // Null-terminate the received data and print the server's response
            buffer[bytes_received] = '\0';
            printf("SERVER:\n\t%s\n", buffer);

            // Check if the server responded with "END"
            if (strcmp(buffer, "END") == 0)
            {
                if (fclose(newFile) != 0)
                {
                    perror("ERROR! File Closing Failed.\n");
                    exit(EXIT_FAILURE); // indicate failure
                }
                break;
            }

            // Write the word to the new file
            fputs(buffer, newFile);
            fputs("\n", newFile);
            counter++;
        }

        // Print a message indicating the successful creation of the new file
        printf("\nNew file created with name %s\n", newfile);
    }
    else
    {
        char file_error[MAX_WORD_LENGTH] = "NOTFOUND ";
        strcat(file_error, filename);
        if (strcmp(file_error, buffer) == 0)
        {
            // Print an error message if the server did not respond with "HELLO"
            printf("\nFile %s Not Found\n", filename);
            printf("\nTerminating...\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("ERROR! Some Unexpected Message Received.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Clean up resources and close the socket
    cleanup(sockfd, buffer);
    printf("\nTerminating...\n");
    return 0;
}