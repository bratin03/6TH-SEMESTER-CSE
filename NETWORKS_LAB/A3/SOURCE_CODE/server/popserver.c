/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
                    Name: Somya Kumar
                    Student ID: 21CS30050
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

/*
Usage:
gcc popserver.c -o popserver
./popserver <pop3_port>

VERBOSE:
To enable verbose mode, uncomment the line "#define VERBOSE" in the source code or compiles as
gcc -DVERBOSE popserver.c -o popserver
./popserver <pop3_port>

Notes:
1. The server is designed to handle multiple clients concurrently using fork.
2. The server checks for the existence of the user in the file "user.txt" and then checks for the password.
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
#include <ctype.h>
#include <sys/file.h>
#include <signal.h>

// #define VERBOSE

#define MAX_CONCURRENT_CONNECTIONS 10
#define MAX_BUFFER_SIZE 1024
#define MAX_LINE_LENGTH 100
#define IP_ADDR "127.0.0.1"
#define USER_FILE "user.txt"

#define AUTHENTICATION 1
#define AUTH_USER 1
#define AUTH_PASS 2

#define TRANSACTION 2
#define STAT 1
#define LIST 2
#define RETR 3
#define DELE 4
#define RSET 5

#define QUIT 3
#define EXIT -2
#define EXIT_LOOP -1

// Function to handle SIGINT
void siginthandler(int signum)
{
    printf("\nClosing Server.\n");
    exit(EXIT_SUCCESS);
}

// Function to clear buffer
void clearBuffer(char *buffer)
{
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }
    return;
}

// Function to read from socket until CRLF
int readUntilCRLF(int socket, char buffer[])
{
    int bytesRead = 0;

    while (1)
    {

        ssize_t bytesReadThisIteration = recv(socket, buffer + bytesRead, 1, 0);
        if (bytesReadThisIteration <= 0)
        {
            printf("Client hang up unexpectedly\n");
            return 0;
            break;
        }

        bytesRead += bytesReadThisIteration;

        if (bytesRead >= 2 && buffer[bytesRead - 2] == '\r' && buffer[bytesRead - 1] == '\n')
        {
            buffer[bytesRead] = '\0';
            break;
        }
    }
    return 1;
}

// Function to extract command from received string
void extractCommand(const char *receivedString, char *reply_code)
{

    if (receivedString != NULL && strlen(receivedString) >= 4)
    {
        strncpy(reply_code, receivedString, 4);
        int i = 0;
        for (i = 0; i < 4; i++)
        {
            reply_code[i] = toupper(reply_code[i]);
        }
        reply_code[4] = '\0';
    }
    else
    {
        return;
    }
}

// Function to extract username from received string
void extractUsername(const char *buffer, char *username)
{
    if (buffer == NULL || username == NULL)
    {
        return;
    }

    const char *spacePos = strchr(buffer, ' ');

    if (spacePos != NULL)
    {
        strncpy(username, spacePos + 1, strlen(spacePos + 1));
        char *crlfPos = strstr(username, "\r\n");
        if (crlfPos != NULL)
        {
            *crlfPos = '\0';
        }
    }
    else
    {
        username[0] = '\0';
    }
}

// Function to check if username exists
int check_username(const char *username)
{

    FILE *fp = fp = fopen(USER_FILE, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 0;
    }
    char file_buffer[MAX_BUFFER_SIZE];
    while (fgets(file_buffer, MAX_BUFFER_SIZE, fp) != NULL)
    {
        char file_username[MAX_BUFFER_SIZE];
        char *token = strtok(file_buffer, " ");
        strcpy(file_username, token);
        if (strcmp(username, file_username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);

    return 0;
}

// Function to extract password from received string
void extractPassword(const char *buffer, char *password)
{
    if (buffer == NULL || password == NULL)
    {
        return;
    }

    const char *spacePos = strchr(buffer, ' ');

    if (spacePos != NULL)
    {
        strncpy(password, spacePos + 1, strlen(spacePos + 1));
        char *crlfPos = strstr(password, "\r\n");
        if (crlfPos != NULL)
        {
            *crlfPos = '\0';
        }
    }
    else
    {

        password[0] = '\0';
    }
}

// Function to get password from file
void getPassword(const char *filename, const char *username, char *password)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *token = strtok(line, " \t\n");
        if (token != NULL && strcmp(token, username) == 0)
        {
            token = strtok(NULL, "\n");
            if (token != NULL)
            {
                while (isspace((unsigned char)*token))
                {
                    token++;
                }
                strncpy(password, token, MAX_LINE_LENGTH);
                password[MAX_LINE_LENGTH - 1] = '\0';
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
}

// Function to acquire and release lock
int acquire_lock(FILE *file)
{
    return 1;
    // Skip locking for now
    int fd = fileno(file);

    if (flock(fd, LOCK_EX) == -1)
    {
        return 0;
    }
    return 1;
}

// Function to release lock
int release_lock(FILE *file)
{
    return 1;
    // Skip locking for now
    int fd = fileno(file);

    if (flock(fd, LOCK_UN) == -1)
    {
        return 0;
    }
    return 1;
}

// Function to get file size
long getFileSize(FILE *file)
{
    long size;
    long currentPosition = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, currentPosition, SEEK_SET);
    return size;
}

// Function to count emails and store their positions
int countEmails(FILE *file, long emailPositions[])
{
    int count = 0;
    char line[MAX_LINE_LENGTH];
    long initialPosition = ftell(file);
    long initialPositionCopy = initialPosition;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '.' && line[1] == '\n')
        {
            emailPositions[count] = initialPosition;
            count++;
            initialPosition = ftell(file);
        }
    }

    emailPositions[count] = initialPosition;
    fseek(file, initialPositionCopy, SEEK_SET);
    return count;
}

// Function to restructure file after deleting emails
void restructureFile(FILE *fp, FILE *fp2, long *startOfMessage, int *deleted, int numMessages)
{
    long shift = 0;

    for (int i = 0; i < numMessages; i++)
    {
        if (!deleted[i])
        {
            fseek(fp, startOfMessage[i], SEEK_SET);
            char buffer[1];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, 1, fp)) > 0)
            {
                long cur = ftell(fp);
                if (cur >= startOfMessage[i + 1])
                {
                    fwrite(buffer, 1, 1, fp2);
                    break;
                }
                fwrite(buffer, 1, 1, fp2);
            }
        }
    }
    fseek(fp, shift, SEEK_SET);
    ftruncate(fileno(fp), ftell(fp));
    rewind(fp);
}

// Main function
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("ERROR!\nUsage: %s <pop3_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Register signal handler for SIGINT
    signal(SIGINT, siginthandler);

    // Get port number from command line arguments
    int pop3_port = atoi(argv[1]);

    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR! Unable to Create Socket.\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(pop3_port);
    // inet_aton(IP_ADDR, &(serv_addr.sin_addr)); // Change this to INADDR_ANY if you want to receive emails for all domains
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Change this to inet_aton(IP_ADDR, &(serv_addr.sin_addr)); if you want to receive emails for a particular domain

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR! Unable to bind to local address\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("POP3 Server started at port %d\n", pop3_port);
    if (listen(sockfd, MAX_CONCURRENT_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (fork() == 0)
        {
            // Child process to handle client
            close(sockfd);

            printf("Connection accepted.\n");

            char buffer[MAX_BUFFER_SIZE];
            clearBuffer(buffer);
            char command[5];
            char username[MAX_LINE_LENGTH];
            FILE *fp, *fp_write;

            // Send initial response
            char *initialResponse = "+OK POP3 server ready\r\n";
            strcpy(buffer, initialResponse);
            send(newsockfd, buffer, strlen(buffer), 0);

            // Data structures for handling emails
            long emailPositions[MAX_LINE_LENGTH];
            int emailCount;
            int *is_Deleted;
            int positions_found = 0;
            int state = AUTHENTICATION; // initial state set to AUTHENTICATION
            int auth_state = AUTH_USER; // initial state set to AUTH_USER
            int email_deleted_count = 0;
            int email_deleted_size = 0;

            while (1)
            {
                if (state == EXIT_LOOP)
                {
                    // State -1 is used to exit the loop
                    break;
                }
                // State machine to handle different states
                switch (state)
                {
                case AUTHENTICATION:
                {
                    // State to handle authentication
                    switch (auth_state)
                    {
                    case AUTH_USER:
                    {
                        // State to handle username
                        // The received message is read and the username is extracted
                        // The username is then checked for existence in the file "user.txt"
                        // If the username exists, the state is changed to AUTH_PASS
                        // If the username does not exist, then a message is sent to the client and the state is changed to EXIT
                        clearBuffer(buffer);
                        if (readUntilCRLF(newsockfd, buffer) == 0)
                        {
                            close(newsockfd);
                            exit(EXIT_FAILURE);
                        }
                        extractCommand(buffer, command);
#ifdef VERBOSE
                        printf("Command: %s\n", command);
                        printf("Message Received: %s", buffer);
#endif
                        if (strcmp(command, "USER") == 0)
                        {
                            extractUsername(buffer, username);
                            if (check_username(username) == 1)
                            {
                                sprintf(buffer, "+OK %s is a valid mailbox\r\n", username);
                                send(newsockfd, buffer, strlen(buffer), 0);
                                auth_state = AUTH_PASS;
                            }
                            else
                            {
                                sprintf(buffer, "-ERR %s is not a valid mailbox\r\n", username);
                                send(newsockfd, buffer, strlen(buffer), 0);
                            }
                        }
                        else if (strcmp(command, "QUIT") == 0)
                        {
                            sprintf(buffer, "+OK POP3 server signing off\r\n");
                            send(newsockfd, buffer, strlen(buffer), 0);
                            close(newsockfd);
                            printf("Connection closed.\n");
                            exit(EXIT_SUCCESS);
                        }
                        else
                        {
                            sprintf(buffer, "-ERR Unknown command\r\n");
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        break;
                    }
                    case AUTH_PASS:
                    {
                        // State to handle password
                        clearBuffer(buffer);
                        if (readUntilCRLF(newsockfd, buffer) == 0)
                        {
                            close(newsockfd);
                            exit(EXIT_FAILURE);
                        }
                        extractCommand(buffer, command);
#ifdef VERBOSE
                        printf("Command: %s\n", command);
                        printf("Message Received: %s", buffer);
#endif
                        if (strcmp(command, "PASS") == 0)
                        {
                            char password[MAX_LINE_LENGTH];
                            extractPassword(buffer, password);
                            char user_password[MAX_LINE_LENGTH];
                            getPassword(USER_FILE, username, user_password);
                            if (strcmp(password, user_password) != 0)
                            {
                                sprintf(buffer, "-ERR Invalid password\r\n");
                                send(newsockfd, buffer, strlen(buffer), 0);
                                state = EXIT;
                            }
                            else
                            {
                                char filename[150];
                                sprintf(filename, "%s/mymailbox", username);
                                fp = fopen(filename, "a+");
                                fp_write = fopen(filename, "a+");

                                int lock_status = 1;
                                if (fp == NULL)
                                {
                                    lock_status = 0;
                                }
                                else
                                {
                                    lock_status = acquire_lock(fp);
                                }
                                if (lock_status == 1)
                                {
                                    sprintf(buffer, "+OK Maildrop locked and ready\r\n");
                                    send(newsockfd, buffer, strlen(buffer), 0);
                                    state = TRANSACTION;
                                }
                                else
                                {
                                    sprintf(buffer, "-ERR unable to lock maildrop\r\n");
                                    send(newsockfd, buffer, strlen(buffer), 0);
                                    auth_state = AUTH_USER;
                                }
                            }
                        }
                        else if (strcmp(command, "QUIT") == 0)
                        {
                            sprintf(buffer, "+OK POP3 server signing off\r\n");
                            send(newsockfd, buffer, strlen(buffer), 0);
                            close(newsockfd);
                            printf("Connection closed.\n");
                            exit(EXIT_SUCCESS);
                        }
                        else
                        {
                            sprintf(buffer, "-ERR Unknown command\r\n");
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        break;
                    }
                    }
                    break;
                }
                case TRANSACTION:
                {
                    // State to handle transaction
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(newsockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message Received: %s", buffer);
#endif
                    if (positions_found == 0)
                    {
                        emailCount = countEmails(fp, emailPositions);
                        positions_found = 1;
                        is_Deleted = (int *)malloc(sizeof(int) * emailCount);
                        for (int i = 0; i < emailCount; i++)
                        {
                            is_Deleted[i] = 0;
                        }
                    }
                    int transaction_key;
                    if (strcmp(command, "STAT") == 0)
                    {
                        transaction_key = STAT;
                    }
                    else if (strcmp(command, "LIST") == 0)
                    {
                        transaction_key = LIST;
                    }
                    else if (strcmp(command, "RETR") == 0)
                    {
                        transaction_key = RETR;
                    }
                    else if (strcmp(command, "DELE") == 0)
                    {
                        transaction_key = DELE;
                    }
                    else if (strcmp(command, "RSET") == 0)
                    {
                        transaction_key = RSET;
                    }
                    else if (strcmp(command, "QUIT") == 0)
                    {
                        state = QUIT;
                        break;
                    }
                    switch (transaction_key)
                    {
                    case STAT:
                    {
                        // State to handle STAT command
                        int file_size = getFileSize(fp);
                        sprintf(buffer, "+OK %d %d\r\n", emailCount - email_deleted_count, file_size - email_deleted_size);
                        send(newsockfd, buffer, strlen(buffer), 0);
                        break;
                    }
                    case LIST:
                    {
                        // State to handle LIST command
                        int email_number = atoi(buffer + 5);
                        if (email_number == 0 && buffer[5] != '0')
                        {
                            int file_size = getFileSize(fp);

                            sprintf(buffer, "+OK %d messages (%d octets)\r\n", emailCount - email_deleted_count, file_size - email_deleted_size);
                            send(newsockfd, buffer, strlen(buffer), 0);
                            for (int i = 0; i < emailCount; i++)
                            {
                                if (is_Deleted[i] == 0)
                                {
                                    sprintf(buffer, "%d %ld\r\n", i + 1, emailPositions[i + 1] - emailPositions[i]);
                                    send(newsockfd, buffer, strlen(buffer), 0);
                                }
                            }
                            sprintf(buffer, ".\r\n");
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        else
                        {
                            if (email_number > emailCount || email_number <= 0)
                            {
                                sprintf(buffer, "-ERR no such message, only %d messages in maildrop\r\n", emailCount - email_deleted_count);
                                send(newsockfd, buffer, strlen(buffer), 0);
                            }
                            else if (is_Deleted[email_number - 1] == 1)
                            {
                                sprintf(buffer, "-ERR message %d already deleted\r\n", email_number);
                                send(newsockfd, buffer, strlen(buffer), 0);
                            }
                            else
                            {
                                sprintf(buffer, "+OK %d %ld\r\n", email_number, emailPositions[email_number] - emailPositions[email_number - 1]);
                                send(newsockfd, buffer, strlen(buffer), 0);
                            }
                        }
                        break;
                    }
                    case RETR:
                    {
                        // State to handle RETR command
                        int email_number = atoi(buffer + 5);
                        if (email_number > emailCount || email_number <= 0)
                        {
                            sprintf(buffer, "-ERR no such message, only %d messages in maildrop\r\n", emailCount - email_deleted_count);
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        else if (is_Deleted[email_number - 1] == 1)
                        {
                            sprintf(buffer, "-ERR message %d already deleted\r\n", email_number);
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        else
                        {

                            sprintf(buffer, "+OK %ld octets\r\n", emailPositions[email_number] - emailPositions[email_number - 1]);
                            send(newsockfd, buffer, strlen(buffer), 0);
                            long initialPositionCopy = ftell(fp);
                            fseek(fp, emailPositions[email_number - 1], SEEK_SET);
                            char line[100];
                            while (fgets(line, sizeof(line), fp) != NULL)
                            {
                                line[strcspn(line, "\n")] = '\0';
                                sprintf(buffer, "%s\r\n", line);
                                send(newsockfd, buffer, strlen(buffer), 0);
                                if (strcmp(buffer, ".\r\n") == 0)
                                {
                                    break;
                                }
                            }
                            fseek(fp, initialPositionCopy, SEEK_SET);
                        }
                        break;
                    }
                    case DELE:
                    {
                        // State to handle DELE command
                        int email_number = atoi(buffer + 5);
                        if (email_number > emailCount || email_number <= 0)
                        {
                            sprintf(buffer, "-ERR no such message, only %d messages in maildrop\r\n", emailCount - email_deleted_count);
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        else if (is_Deleted[email_number - 1] == 1)
                        {
                            sprintf(buffer, "-ERR message %d already deleted\r\n", email_number);
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        else
                        {
                            is_Deleted[email_number - 1] = 1;
                            email_deleted_count++;
                            email_deleted_size += emailPositions[email_number] - emailPositions[email_number - 1];
                            sprintf(buffer, "+OK message %d deleted\r\n", email_number);
                            send(newsockfd, buffer, strlen(buffer), 0);
                        }
                        break;
                    }
                    case RSET:
                    {
                        // State to handle RSET command
                        memset(is_Deleted, 0, sizeof(is_Deleted));
                        email_deleted_count = 0;
                        email_deleted_size = 0;
                        sprintf(buffer, "+OK maildrop has %d messages (%ld octets)\r\n", emailCount, emailPositions[emailCount]);
                        send(newsockfd, buffer, strlen(buffer), 0);
                        break;
                    }
                    default:
                    {
                        // State to handle unknown command
                        sprintf(buffer, "-ERR Unknown command\r\n");
                        send(newsockfd, buffer, strlen(buffer), 0);
                    }
                    break;
                    }
                    break;
                }
                case QUIT:
                {
                    // State to handle QUIT command
                    restructureFile(fp, fp_write, emailPositions, is_Deleted, emailCount);
                    fclose(fp);
                    fclose(fp_write);
                    release_lock(fp);
                    if (emailCount == email_deleted_count)
                    {
                        sprintf(buffer, "+OK POP3 server signing off (maildrop empty)\r\n");
                    }
                    else
                    {
                        sprintf(buffer, "+OK POP3 server signing off (%d messages left)\r\n", emailCount - email_deleted_count);
                    }
                    send(newsockfd, buffer, strlen(buffer), 0);
                    close(newsockfd);
                    printf("Connection closed.\n");
                    exit(EXIT_SUCCESS);
                }
                case EXIT:
                {
                    // State to exit
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(newsockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message Received: %s", buffer);
#endif
                    if (strcmp(command, "QUIT") == 0)
                    {
                        sprintf(buffer, "+OK POP3 server signing off\r\n");
                        send(newsockfd, buffer, strlen(buffer), 0);
                        close(newsockfd);
                        printf("Connection closed.\n");
                        exit(EXIT_SUCCESS);
                    }
                    else
                    {
                        sprintf(buffer, "-ERR Unknown command\r\n");
                        send(newsockfd, buffer, strlen(buffer), 0);
                        state = EXIT;
                    }
                }
                }
            }
            close(newsockfd);
            exit(EXIT_SUCCESS);
        }
    }
}