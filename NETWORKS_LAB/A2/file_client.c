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
Client Design Specification:

1. User Input:
   - Prompt the user for the name of the file to be transferred and the encryption key 'k'.

2. Connection Establishment:
   - Establish a secure connection to the server.

3. File Transfer:
   - Send the encryption key 'k' and the contents of the file to the server.

4. File Reception and Storage:
   - Receive the encrypted file from the server.
   - Write the received data to a new file on the client side.

5. Connection Closure:
   - Close the connection to the server.

6. Repeat Process:
   - Repeat the above steps for subsequent files as necessary.
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
#include <string.h>

#define BUFF_SIZE 80       // Buffer size for sending and receiving data
#define PORT 8181          // Port number
#define FILE_NAME_SIZE 101 // Maximum file name size

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
Function Name: open_File_For_Read
Functionality: Opens a file in read-only mode and returns the file descriptor.

Parameters:
- file_name: Pointer to the character array containing the name of the file.

Returns:
- File descriptor of the opened file.
*/

int open_File_For_Read(char *file_name)
{
    int txt_fd = -1; // File descriptor of the file to be sent

    do
    {
        // Prompt user for the name of the file
        printf("Enter the name of the file to be transferred: ");
        scanf("%s", file_name);

        // Open the file in read-only mode
        txt_fd = open(file_name, O_RDONLY);

        if (txt_fd == -1)
        {
            // File not found. Print error message and prompt for a valid file name.
            printf("File not found! Try again.\n");
        }

    } while (txt_fd == -1);

    return txt_fd;
}

/*
Function Name: send_File_Client
Functionality: Sends the contents of the file to the server.

Parameters:
- sockfd: Socket file descriptor.
- txt_fd: File descriptor of the file to be sent.
- k: Key for encryption.
- buffer: Character array to be used as a buffer for sending data.
*/

void send_File_Client(int sockfd, int txt_fd, int k, char *buffer)
{
    ssize_t bytes_Read = 0; // Number of bytes read from the file

    // Send parameter 'k' to the client
    if (send(sockfd, &k, sizeof(k), 0) == -1)
    {
        perror("Error! Failed to send the key to the server.\n");
        close(sockfd); // Close the socket before exiting
        close(txt_fd); // Close the file descriptor before exiting
        exit(EXIT_FAILURE);
    }

    do
    {
        // Reset buffer to null before reading file data
        reset_to_null(buffer, BUFF_SIZE);

        // Read file data into the buffer
        bytes_Read = read(txt_fd, buffer, BUFF_SIZE - 1);
        if (bytes_Read == -1)
        {
            perror("ERROR! Failed to read file.\n");
            close(sockfd); // Close the socket before exiting
            close(txt_fd); // Close the file descriptor before exiting
            exit(EXIT_FAILURE);
        }
        buffer[bytes_Read] = '\0';

        // Send the buffer contents to the client
        ssize_t send_result = send(sockfd, buffer, strlen(buffer), 0);
        if (send_result == -1)
        {
            perror("ERROR! Failed to send file.\n");
            close(sockfd); // Close the socket before exiting
            close(txt_fd); // Close the file descriptor before exiting
            exit(EXIT_FAILURE);
        }

    } while (bytes_Read == BUFF_SIZE - 1);

    // Alteranively we can use '\n' to mark the end of file transfer is the file does not contain '\n' except at the end of the file.
    // Signal the end of file transfer by sending a special character ('$')
    strcpy(buffer, "$");
    if (send(sockfd, buffer, strlen(buffer), 0) == -1)
    {
        perror("ERROR! Failed to send file.\n");
        close(sockfd); // Close the socket before exiting
        close(txt_fd); // Close the file descriptor before exiting
        exit(EXIT_FAILURE);
    }
    close(txt_fd); // Close the file descriptor
}

/*
Function Name: receive_And_Write_Client
Functionality: Receives the encrypted file from the server and writes it to a file.

Parameters:
- sockfd: Socket file descriptor.
- file_name: Pointer to the character array containing the name of the file.
- buffer: Character array to be used as a buffer for receiving data.
*/

void receive_And_Write_Client(int sockfd, char *file_name, char *buffer)
{
    // Create an encrypted file name by appending ".enc" to the original file name
    char encrypt_file_name[FILE_NAME_SIZE + 4];
    strcpy(encrypt_file_name, file_name);
    strcat(encrypt_file_name, ".enc");

    // Open the encrypted file for writing
    int encrypt_fd = open(encrypt_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (encrypt_fd < 0)
    {
        // Error: Failed to create the encrypted file. Print error message and exit.
        perror("Error! .enc File Creation Failed.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive and write data until the end-of-file marker ('$') is encountered
    ssize_t bytes_received = 0, i;
    int eof = 0;
    while ((bytes_received = recv(sockfd, buffer, BUFF_SIZE, 0)) > 0)
    {
        for (i = 0; i < bytes_received; i++)
        {
            // Check for the end-of-file marker
            if (buffer[i] == '$')
            {
                eof = 1;
                break;
            }

            // Write non-null characters to the encrypted file
            if (buffer[i] != '\0')
            {
                ssize_t write_result = write(encrypt_fd, &buffer[i], 1);
                if (write_result == -1)
                {
                    // Error: Failed to write to the encrypted file. Print error message and exit.
                    perror("ERROR! Failed to write to the encrypted file.\n");
                    close(encrypt_fd);
                    close(sockfd);
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
            ssize_t send_result = send(sockfd, "ACK", 3, 0); // Send acknowledgement to the server
            if (send_result < 0)
            {
                perror("ERROR! Failed to send acknowledgement to the server.\n");
                close(encrypt_fd);
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            printf("Encrypted file received successfully.\n");
            break;
        }

        // Reset the buffer to null for the next iteration
        reset_to_null(buffer, BUFF_SIZE);
    }

    close(encrypt_fd); // Close the encrypted file

    // Print success message with file information
    printf("The %s file is encrypted successfully.\nOriginal File Name: %s\nEncrypted File Name: %s\n\n", file_name, file_name, encrypt_file_name);
}

/*
Function Name: main
Functionality: Establishes a connection to a server, sends a file, and receives the encrypted version.
*/

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    // Allocate memory for buffer and file_name
    char *buffer = (char *)malloc(BUFF_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        perror("ERROR! Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    reset_to_null(buffer, BUFF_SIZE);
    char *file_name = (char *)malloc(FILE_NAME_SIZE * sizeof(char));
    if (file_name == NULL)
    {
        perror("ERROR! Memory allocation failed.\n");
        free(buffer); // Cleanup before exit
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        // Open the file for reading
        int txt_fd = open_File_For_Read(file_name);

        // Get the encryption key 'k' from the user
        int k;
        printf("Enter the key K: ");
        scanf("%d", &k);

        // Create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            // Error: Unable to create a socket. Print error message and exit.
            perror("ERROR! Unable to Create Socket.\n");
            exit(EXIT_FAILURE);
        }

        // Set up server address
        serv_addr.sin_family = AF_INET;

        // Convert and set the server IP address
        if (inet_aton("127.0.0.1", &serv_addr.sin_addr) == 0)
        {
            // Error: Invalid IP address. Print error message, close the socket, and exit.
            perror("ERROR! Invalid IP address.\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Set the server port
        serv_addr.sin_port = htons(PORT);
        // Connect to the server
        if ((connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
        {
            // Error: Unable to connect to the server. Print error message and exit.
            perror("ERROR! Unable to Connect to Server.\n");
            exit(EXIT_FAILURE);
        }

        // Print connection information
        printf("Connected to Server at %s:%d.\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

        // Send the file to the server
        send_File_Client(sockfd, txt_fd, k, buffer);

        // Receive the encrypted file from the server and write it to a new file
        receive_And_Write_Client(sockfd, file_name, buffer);

        // Close the socket
        close(sockfd);
    }
    free(buffer);       // Cleanup before exit
    free(file_name);    // Cleanup before exit
    exit(EXIT_SUCCESS); // Exit with success
}
