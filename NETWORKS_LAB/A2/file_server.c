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
Server Design Specification:

1. Socket Creation and Binding:
   - Create a socket on the server and bind it to a local address.

2. Connection Handling:
   - Listen for incoming client connections.

3. Client Connection Management:
   - Upon a client connection, accept the connection.
   - Fork a child process to handle the client request concurrently.

4. File Processing:
   - In the child process, receive the encryption key 'k' and the file from the client.
   - Apply Caesar Cipher encoding to the received file for encryption.
   - Send the encrypted file back to the client.

5. Connection Closure:
   - Close the client socket to release resources.

6. Child Process Completion:
   - The parent process waits for the child process to complete its task.

7. Continuous Listening:
   - The server continues to listen for new client connections, repeating the process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFF_SIZE 90             // Buffer size for sending and receiving data
#define PORT 8181                // Port number
#define MAX_CONCURRENT_CLIENTS 5 // Maximum number of concurrent client connections
#define FILE_NAME_SIZE 51        // Maximum size of file names

/*
Function Name: reset_to_null
Functionality: Sets each character in the given buffer to null ('\0') for the specified length.

Parameters:
- buffer: Pointer to the character array to be reset.
- n: Length of the buffer.
*/

void reset_to_null(char *buffer, int n)
{
    if (buffer == NULL || n < 0)
    {
        // Invalid input parameters.
        return;
    }
    int i;
    for (i = 0; i < n; i++)
    {
        buffer[i] = '\0';
    }
}

/*
Function Name: mod
Functionality: Computes the modulo of two integers.

Parameters:
- a: The dividend.
- b: The divisor.
*/

int mod(int a, int b)
{
    int r = a % b;            // Computes the remainder.
    return r < 0 ? r + b : r; // Reurns the non-negative remainder.
}

/*
Function Name: get_File_Name
Functionality: Generates file names based on the client's IP address and port number.

Parameters:
- cli_addr: Structure containing client address information.
- txt_file_name: Pointer to the character array to store the original file name.
- encrypt_file_name: Pointer to the character array to store the encrypted file name.
*/

void get_File_Name(struct sockaddr_in cli_addr, char txt_file_name[], char encrypt_file_name[])
{
    // Convert client IP address to string
    char client_ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, sizeof(client_ip)) == NULL)
    {
        perror("ERROR! inet_ntop failed.\n");
        // Error: Failed to convert client IP address to string.
        exit(EXIT_FAILURE);
    }

    // Generate a string representing the connection info (IP address + port)
    char connection_info[FILE_NAME_SIZE];
    snprintf(connection_info, sizeof(connection_info), "%s.%d", client_ip, ntohs(cli_addr.sin_port));
    printf("Connection established with %s\n", connection_info); // Print connection info

    // Create the original file name by appending ".txt" to the connection info
    strcpy(txt_file_name, connection_info);
    strcat(txt_file_name, ".txt");

    // Create the encrypted file name by appending ".enc" to the original file name
    strcpy(encrypt_file_name, txt_file_name);
    strcat(encrypt_file_name, ".enc");
}

/*
Function Name: receive_And_Write_Server
Functionality: Receives data from a client, writes it to a text file, and prints a success message.

Parameters:
- newsockfd: Socket file descriptor for the connected client.
- txt_file_name: Pointer to the character array containing the name of the text file to be created.
- k: Pointer to the integer variable storing the encryption key received from the client.
- buffer: Character array used for receiving and processing data.
*/

void receive_And_Write_Server(int newsockfd, const char *txt_file_name, int *k, char *buffer)
{
    // Receive the encryption key 'k' from the client
    int k_copy;
    ssize_t recv_result = recv(newsockfd, &k_copy, sizeof(k_copy), 0);
    *k = k_copy;

    // Check for errors or client disconnection during key reception
    if (recv_result < 0)
    {
        perror("ERROR! Receiving data from client failed.\n");
        close(newsockfd);
        exit(EXIT_FAILURE);
    }

    // Open or create the text file for writing
    int txt_fd = open(txt_file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (txt_fd < 0)
    {
        // Error: Failed to create the text file. Print error message and exit.
        perror("ERROR! Text file creation failed.\n");
        close(newsockfd);
        exit(EXIT_FAILURE);
    }

    // Receive and write data until the end-of-file marker ('$') is encountered
    int bytes_received = 0;
    int eof = 0;
    while ((bytes_received = recv(newsockfd, buffer, BUFF_SIZE, 0)) > 0)
    {
        for (int i = 0; i < bytes_received; i++)
        {
            // Check for the end-of-file marker
            if (buffer[i] == '$')
            {
                eof = 1;
                break;
            }

            // Write non-null characters to the text file
            if (buffer[i] != '\0')
            {
                ssize_t write_result = write(txt_fd, &buffer[i], 1);
                if (write_result < 0)
                {
                    perror("ERROR! Writing to text file failed.\n");
                    close(txt_fd);
                    close(newsockfd);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                break;
            }
        }

        // Break the loop if end-of-file marker is encountered
        if (eof == 1)
        {
            break;
        }

        // Reset the buffer to null for the next iteration
        reset_to_null(buffer, BUFF_SIZE);
    }

    // Print success message
    printf("Text file received from client.\n");

    // Close the text file
    close(txt_fd);
}

/*
Function Name: Caesar_Cipher_Encoder
Functionality: Applies Caesar Cipher encoding to a text file and writes the result to an encrypted file.

Parameters:
- txt_file_name: Pointer to the character array containing the name of the text file to be read.
- encrypt_file_name: Pointer to the character array containing the name of the encrypted file to be created.
- k: Integer representing the Caesar Cipher key.
- buffer: Character array used for reading and processing data.
*/

void Caesar_Cipher_Encoder(const char *txt_file_name, const char *encrypt_file_name, int k, char *buffer)
{
    // Open the text file for reading
    int txt_fd = open(txt_file_name, O_RDONLY);
    if (txt_fd < 0)
    {
        // Error: Failed to open the text file for reading. Print error message and exit.
        perror("ERROR! Text file opening failed.\n");
        exit(EXIT_FAILURE);
    }

    // Open or create the encrypted file for writing
    int encrypt_fd = open(encrypt_file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (encrypt_fd < 0)
    {
        // Error: Failed to open/create the encrypted file. Print error message, close the text file, and exit.
        perror("ERROR! Encrypted file opening failed.\n");
        close(txt_fd);
        exit(EXIT_FAILURE);
    }

    // Read data from the text file and apply Caesar Cipher encoding
    ssize_t bytes_read = 0;
    while ((bytes_read = read(txt_fd, buffer, BUFF_SIZE)) > 0)
    {
        for (int i = 0; i < bytes_read; i++)
        {
            if (buffer[i] != '\0')
            {
                // Apply Caesar Cipher encoding to alphabetic characters
                if (buffer[i] >= 'A' && buffer[i] <= 'Z')
                {
                    buffer[i] = mod(buffer[i] - 'A' + k, 26) + 'A';
                }
                else if (buffer[i] >= 'a' && buffer[i] <= 'z')
                {
                    buffer[i] = mod(buffer[i] - 'a' + k, 26) + 'a';
                }

                ssize_t write_result = write(encrypt_fd, &buffer[i], 1);
                if (write_result < 0)
                {
                    perror("ERROR! Writing to encrypted file failed.\n");
                    close(txt_fd);
                    close(encrypt_fd);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                break;
            }
        }

        // Reset the buffer to null for the next iteration
        reset_to_null(buffer, BUFF_SIZE);
    }

    // Print success message
    printf("Caesar Cipher encoding completed.\n");

    // Close the files
    close(txt_fd);
    close(encrypt_fd);
}

/*
Function Name: send_File_Server
Functionality: Sends the contents of an encrypted file to a connected client over a socket.

Parameters:
- newsockfd: Socket file descriptor for the connected client.
- encrypt_file_name: Pointer to the character array containing the name of the encrypted file to be read.
- buffer: Character array used for reading and sending file data.
*/

void send_File_Server(int newsockfd, const char *encrypt_file_name, char *buffer)
{
    // Read from the encrypted file and send data to the client
    ssize_t bytes_read = 0;
    int encrypt_fd = open(encrypt_file_name, O_RDONLY);
    if (encrypt_fd < 0)
    {
        // Error: Failed to open the encrypted file for reading. Print error message, close the socket, and exit.
        perror("ERROR! Encrypted file opening failed.\n");
        close(newsockfd);
        exit(EXIT_FAILURE);
    }

    do
    {
        // Reset buffer to null before reading file data
        reset_to_null(buffer, BUFF_SIZE);

        // Read data from the encrypted file
        bytes_read = read(encrypt_fd, buffer, BUFF_SIZE - 1);
        if (bytes_read < 0)
        {
            // Error: Failed to read from the encrypted file. Print error message, close the socket, and return.
            perror("ERROR! Reading from encrypted file failed.\n");
            close(newsockfd);
            close(encrypt_fd);
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';

        // Send the buffer contents to the client
        ssize_t send_result = send(newsockfd, buffer, strlen(buffer), 0);
        if (send_result < 0)
        {
            // Error: Failed to send data to the client. Print error message, close the socket, and return.
            perror("ERROR! Sending data to client failed.\n");
            close(newsockfd);
            close(encrypt_fd);
            exit(EXIT_FAILURE);
        }

    } while (bytes_read > 0);

    // Alteranively we can use '\n' to mark the end of file transfer is the file does not contain '\n' except at the end of the file.
    // Signal the end of file transfer by sending a special character ('#')
    strcpy(buffer, "$");
    ssize_t send_result = send(newsockfd, buffer, strlen(buffer), 0);
    if (send_result < 0)
    {
        // Error: Failed to send end-of-file marker to the client. Print error message, close the socket, and return.
        perror("ERROR! Sending end-of-file marker to client failed.\n");
        close(newsockfd);
        close(encrypt_fd);
        exit(EXIT_FAILURE);
    }

    // Close the encrypted file
    close(encrypt_fd);

    // Print success message
    printf("Encrypted File Sent to client.\n");
}

/*
Function Name: handle_client_request
Functionality: Handles a client request by receiving a file, applying Caesar Cipher encoding, and sending the encrypted file back.

Parameters:
- sockfd: Original socket file descriptor for the server.
- newsockfd: Socket file descriptor for the connected client.
- cli_addr: Structure containing client address information.
- buffer: Character array used for reading and processing data.
*/

void handle_client_request(int sockfd, int newsockfd, struct sockaddr_in cli_addr, char *buffer)
{
    // Close the original server socket
    if (close(sockfd) < 0)
    {
        perror("ERROR! Closing original server socket failed.\n");
        exit(EXIT_FAILURE);
    }

    // Generate file names based on client information
    char txt_file_name[FILE_NAME_SIZE + 4];
    char encrypt_file_name[FILE_NAME_SIZE + 8];
    get_File_Name(cli_addr, txt_file_name, encrypt_file_name);

    // Receive file from the client and write it to a text file
    int k;
    receive_And_Write_Server(newsockfd, txt_file_name, &k, buffer);

    // Apply Caesar Cipher encoding to the text file
    Caesar_Cipher_Encoder(txt_file_name, encrypt_file_name, k, buffer);

    // Send the encrypted file back to the client
    send_File_Server(newsockfd, encrypt_file_name, buffer);

    // Receive acknowledgement from the client
    ssize_t bytes_received = 0;
    if ((bytes_received = recv(newsockfd, buffer, BUFF_SIZE, 0)) < 0)
    {
        perror("ERROR! Receiving Acknowledgement from client failed.\n");
        close(newsockfd);
        exit(EXIT_FAILURE);
    }

    // Check for errors or client disconnection during acknowledgement reception
    if (strcmp(buffer, "ACK") != 0)
    {
        perror("ERROR! Acknowledgement from client failed.\n");
        close(newsockfd);
        exit(EXIT_FAILURE);
    }

    // Close the client socket and exit
    if (close(newsockfd) < 0)
    {
        perror("ERROR! Closing client socket failed.\n");
        exit(EXIT_FAILURE);
    }

    // Print success message
    printf("Client request handled successfully.\n");
    printf("Client connection closed.\n");
    printf("-----------------------------------------\n");
    printf("Waiting for new client connections...\n\n");
    exit(EXIT_SUCCESS);
}

/*
Function Name: main
Functionality: Initializes a server, listens for client connections, and handles client requests concurrently.
*/

int main()
{
    // Initialize server variables
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    // Allocate memory for the buffer
    char *buffer = (char *)malloc(BUFF_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        // Error: Failed to allocate memory for the buffer. Print error message and exit.
        perror("ERROR! Buffer allocation failed\n");
        exit(EXIT_FAILURE);
    }
    reset_to_null(buffer, BUFF_SIZE);

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // Error: Failed to create a socket. Print error message and exit.
        perror("ERROR! Unable to create socket\n");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the socket to the local address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        // Error: Failed to bind to the local address. Print error message, close the socket, and exit.
        perror("ERROR! Unable to bind to local address\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("========================\n");
    // Display Server Status
    printf("Server Setup Status: Successful!\n");
    printf("========================\n");

    printf("Server listening for connections...\n\n");

    // Listen for client connections
    if (listen(sockfd, MAX_CONCURRENT_CLIENTS) < 0)
    {
        // Error: Failed to listen for connections. Print error message, close the socket, and exit.
        perror("ERROR! Unable to listen for connections.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Main server loop
    for (;;)
    {
        // Accept a client connection
        clilen = sizeof(cli_addr);
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0)
        {
            // Error: Failed to accept the client connection. Print error message, close the socket, and exit.
            perror("ERROR! Unable to accept client connection.\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("-----------------------------------------\n");
        printf("Client connection accepted.\n");
        printf("Client IP address: %s\n", inet_ntoa(cli_addr.sin_addr));
        printf("Client port number: %d\n", ntohs(cli_addr.sin_port));

        // Fork a child process to handle the client request concurrently
        pid_t c_pid = fork();

        if (c_pid < 0)
        {
            // Error: Failed to fork. Print error message, close the client socket, close the server socket, and exit.
            perror("");
            close(newsockfd);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (c_pid == 0)
        {
            // Child process: Handle the client request
            handle_client_request(sockfd, newsockfd, cli_addr, buffer);
        }
        close(newsockfd); // Close the client socket in the parent process
    }

    // Free allocated memory and close the server socket
    free(buffer);
    close(sockfd);

    // Exit the program successfully
    exit(EXIT_SUCCESS);
}
