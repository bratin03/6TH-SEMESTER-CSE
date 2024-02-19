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
gcc smtpmail.c -o smtpmail
./smtpmail <my_port>

VERBOSE:
To enable verbose mode, uncomment the line "#define VERBOSE" in the source code or compiles as
gcc -DVERBOSE smtpmail.c -o smtpmail
./smtpmail <my_port>

Notes:
1. The server is designed to receive emails for all domains. In case we want to receive emails for a particular domain, we can change the IP_ADDR macro to the domain name or for multiple domains, we can use a list of domains. If we want to receive emails for all domains, we can use a wildcard.
2. The server is designed to receive emails for a particular user. In case we want to receive emails for users in the user.txt file, we can change the USER_FILE macro to the file name. If we want to receive emails for all users in case we want to relay, we can omit the check for the username.
3. In case the natural flow of the transaction is broken, the server assumes that the client checks for QUIT command and sends a QUIT command. If the client does not send a QUIT command, the server assumes that there was some error and the conversation will resume from the point where it broke
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
#include <errno.h>
#include <time.h>
#include <signal.h>

// #define VERBOSE

#define MAX_CONCURRENT_CONNECTIONS 5
#define MAX_BUFFER_SIZE 4096
#define MAX_LINE_LENGTH 81
#define MAX_NUM_LINES 51
#define IP_ADDR "127.0.0.1"
#define USER_FILE "user.txt"
#define MAX_MAIL_ADDR 101

#define HELLO 0
#define MAIL 1
#define RCPT 2
#define DATA 3
#define RECEIVE_DATA 4
#define QUIT -1

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
    char *result = NULL;
    int bytesRead = 0;

    while (1)
    {

        ssize_t bytesReadThisIteration = recv(socket, buffer + bytesRead, 1, 0);

        if (bytesReadThisIteration <= 0)
        {

            perror("Error reading from socket");
#ifdef VERBOSE
            printf("Client hang up unexpectedly\n");
#endif
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

// Function to check if QUIT command is received
// This function checks if the received command is QUIT after receiving the command
// This is done to check if the client has sent QUIT command in the middle of a transaction in case of a error or wants to restart the transaction
int check_for_quit(const char *reply_code, int newsockfd)
{
    if (strcmp(reply_code, "QUIT") == 0)
    {
        char buffer[MAX_BUFFER_SIZE];
        clearBuffer(buffer);
        sprintf(buffer, "221 %s closing connection\r\n", IP_ADDR);
        send(newsockfd, buffer, strlen(buffer), 0);
        return 1;
    }
    return 0;
}

// Function to check if the username is present in the user.txt file
// The function compares the username with the username in the user.txt file and the ip address with the ip address of the server
// In case we want to receive emails for a particular domain, we can change the IP_ADDR macro to the domain name or for multiple domains, we can use a list of domains
// If we want to receive emails for all domains, we can use a wildcard
// In case of relaying, if we want to receive for users which are not present in the user.txt file, we can omit this check
int check_username(const char *forward_path_buffer)
{
    const char *start = strchr(forward_path_buffer, '<');
    const char *end = strchr(forward_path_buffer, '>');
    if (start != NULL && end != NULL && start < end)
    {
        size_t length = end - start - 1;
        char extracted_forward_path[length + 1];
        strncpy(extracted_forward_path, start + 1, length);
        extracted_forward_path[length] = '\0';
        const char *at = strchr(extracted_forward_path, '@');
        if (at != NULL)
        {
            size_t username_length = at - extracted_forward_path;
            size_t domain_length = length - username_length - 1;
            char username[username_length + 1];
            char domain[domain_length + 1];
            strncpy(username, extracted_forward_path, username_length);
            username[username_length] = '\0';
            strncpy(domain, at + 1, domain_length);
            domain[domain_length] = '\0';
            // Remove the 1 || condition if we want to receive email for a particular domain
            if (1 || strcmp(domain, IP_ADDR) == 0)
            {
                // Omit this check if we want to receive emails for all users
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
            }
        }
    }

    return 0;
}

// Function to get current time
void getCurrentTime(char *timeStr)
{
    time_t rawTime;
    struct tm *timeInfo;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timeStr, 20, "%Y-%m-%d:%H:%M", timeInfo);
}

// Function to write email to file of the user
void writeEmailToFile(const char *reversePathBuffer, const char *forwardPathBuffer, const char *dataBuffer, const char *fileName)
{
    FILE *file = fopen(fileName, "a");

    if (file != NULL)
    {
        char currentTimeStr[20];
        getCurrentTime(currentTimeStr);

        char senderUsername[256], senderDomain[256];
        sscanf(reversePathBuffer, "<%[^@]@%[^>]>", senderUsername, senderDomain);

        char receiverUsername[256], receiverDomain[256];
        sscanf(forwardPathBuffer, "<%[^@]@%[^>]>", receiverUsername, receiverDomain);

        fprintf(file, "From: %s@%s\n", senderUsername, senderDomain);
        fprintf(file, "To: %s@%s\n", receiverUsername, receiverDomain);
        const char *ptr = dataBuffer;
        const char *lineStart = ptr;
        int lineCount = 0;
        while (*ptr != '\0')
        {
            if (*ptr == '\r' && *(ptr + 1) == '\n')
            {
                char line[ptr - lineStart + 1];
                strncpy(line, lineStart, ptr - lineStart);
                line[ptr - lineStart] = '\0';
                if (lineCount == 0 || lineCount == 1)
                {
                    lineCount++;
                    ptr += 2;
                    lineStart = ptr;
                    continue;
                }

                fprintf(file, "%s\n", line);
                if (lineCount == 2)
                {
                    fprintf(file, "Received: %s\n", currentTimeStr);
                }
                ptr += 2;
                lineStart = ptr;
                lineCount++;
            }
            else
            {
                ptr++;
            }
        }
        fprintf(file, ".\n");
        fclose(file);
    }
    else
    {
        perror("fopen ");
    }
}

// Main function
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("ERROR!\nUsage: %s <my_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, siginthandler);

    // Get port number from command line arguments
    int my_port = atoi(argv[1]);

    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR! Unable to Create Socket.\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(my_port);
    // inet_aton(IP_ADDR, &(serv_addr.sin_addr)); // Change this to INADDR_ANY if you want to receive emails for all domains
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Change this to inet_aton(IP_ADDR, &(serv_addr.sin_addr)); if you want to receive emails for a particular domain

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR! Unable to bind to local address\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("SMTP Server started at port: %d\n", my_port);
    if (listen(sockfd, MAX_CONCURRENT_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Greeting message
    char greeting[MAX_BUFFER_SIZE];
    clearBuffer(greeting);
    sprintf(greeting, "220 %s Service Ready\r\n", IP_ADDR);

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
            // Child process to handle the connection
            close(sockfd);

            printf("Connection Accepted.\n");

            char buffer[MAX_BUFFER_SIZE];
            clearBuffer(buffer);
            // Send greeting message
            strcpy(buffer, greeting);
            send(newsockfd, buffer, strlen(buffer), 0);
            // Buffer to store the reverse path, forward path and mail data
            char reverse_path_buffer[MAX_MAIL_ADDR], forward_path_buffer[MAX_MAIL_ADDR], mail_data_buffer[MAX_NUM_LINES * MAX_LINE_LENGTH];

            // Initial state of the state machine
            int state = HELLO;

            char command[5];
            while (1)
            {
                if (state == QUIT)
                {
                    // State -1 is the state where the server waits for the QUIT command
                    // In this state, the server receives the QUIT command and sends a 221 OK message
                    // It then closes the connection
                    // It then exits the child process
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message received: %s", buffer);
#endif
                    if (check_for_quit(command, newsockfd) == 1)
                    {
                        close(newsockfd);
                        printf("Closing Connection.\n");
                        exit(EXIT_SUCCESS);
                    }
                    else
                    {
                        // If the client does not send a QUIT command when it should in case of a natural transaction flow
                        // We assume that the client wants to restart the transaction
                        state = HELLO;
                    }
                }

                // State machine to handle the SMTP protocol
                switch (state)
                {

                case HELLO:
                {
                    // State 0 is the initial state where the server waits for the HELO command
                    // In this state, the server receives the HELO command and sends a 250 OK message
                    // It then transitions to the MAIL state
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(command, "HELO") != 0)
                    {
#ifdef VERBOSE
                        printf("HELO failed.\n");
#endif
                        if (check_for_quit(command, newsockfd) == 1)
                        {
                            printf("Closing Connection.\n");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    }

                    clearBuffer(buffer);
                    sprintf(buffer, "250 OK Hello %s\r\n", IP_ADDR);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    state = MAIL;
                    break;
                }
                case MAIL:
                {
                    // State 1 is the state where the server waits for the MAIL command
                    // In this state, the server receives the MAIL command and sends a 250 OK message
                    // It then transitions to the RCPT state
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(command, "MAIL") != 0)
                    {
#ifdef VERBOSE
                        printf("MAIL failed.\n");
#endif
                        if (check_for_quit(command, newsockfd) == 1)
                        {
                            printf("Closing Connection.\n");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    }
                    // Clear the buffers
                    clearBuffer(reverse_path_buffer);
                    clearBuffer(forward_path_buffer);
                    clearBuffer(mail_data_buffer);
                    // Extract the reverse path from the MAIL command
                    size_t length = strlen(buffer);
                    if (length > 12)
                    {
                        size_t reverse_path_length = length - 12;
                        strncpy(reverse_path_buffer, buffer + 10, reverse_path_length);
                        reverse_path_buffer[reverse_path_length] = '\0';
                    }
                    clearBuffer(buffer);
                    sprintf(buffer, "250 %s...Sender OK\r\n", reverse_path_buffer);
                    send(newsockfd, buffer, strlen(buffer), 0);
                    state = RCPT;
                    break;
                }
                case RCPT:
                {
                    // State 2 is the state where the server waits for the RCPT command
                    // In this state, the server receives the RCPT command and sends a 250 OK message
                    // It then transitions to the DATA state
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(command, "RCPT") != 0)
                    {
                        // In this case we can get either a single RCPT command or multiple RCPT commands
                        // The assignment specification asks for only one RCPT command
                        // In general, we can handle multiple RCPT commands by checking for a new DATA command
                        // However, we have not implemented this
                        // We also assume that if this single RCPT command fails, the client will send a QUIT command
#ifdef VERBOSE
                        printf("RCPT failed.\n");
#endif
                        if (check_for_quit(command, newsockfd) == 1)
                        {
                            printf("Closing Connection.\n");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    }
                    size_t length = strlen(buffer);
                    if (length > 10)
                    {
                        size_t forward_path_length = length - 10;
                        strncpy(forward_path_buffer, buffer + 8, forward_path_length);
                        forward_path_buffer[forward_path_length] = '\0';
                    }
                    clearBuffer(buffer);
                    if (check_username(forward_path_buffer) == 1)
                    {
                        clearBuffer(buffer);
                        sprintf(buffer, "250 %s...Recipient OK\r\n", forward_path_buffer);
                        send(newsockfd, buffer, strlen(buffer), 0);
                        state = DATA;
                    }
                    else
                    {
                        clearBuffer(buffer);
                        sprintf(buffer, "550 %s...User not found\r\n", forward_path_buffer);
                        send(newsockfd, buffer, strlen(buffer), 0);
                        state = QUIT;
                    }
                    break;
                }
                case DATA:
                {
                    // State 3 is the state where the server waits for the DATA command
                    // In this state, the server receives the DATA command and sends a 354 Enter mail, end with <CRLF>.<CRLF> message
                    // It then transitions to the RECEIVE_DATA state
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    extractCommand(buffer, command);
#ifdef VERBOSE
                    printf("Command: %s\n", command);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(command, "DATA") != 0)
                    {
#ifdef VERBOSE
                        printf("DATA failed.\n");
#endif
                        if (check_for_quit(command, newsockfd) == 1)
                        {
                            printf("Closing Connection.\n");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    }
                    clearBuffer(buffer);
                    sprintf(buffer, "354 Enter mail, end with <CRLF>.<CRLF>\r\n");
                    send(newsockfd, buffer, strlen(buffer), 0);
                    state = RECEIVE_DATA;
                    break;
                }
                case RECEIVE_DATA:
                {
                    // State 4 is the state where the server waits for the mail data
                    // In this state, the server receives the mail data and sends a 250 OK message
                    // It then transitions to the QUIT state
                    clearBuffer(buffer);
                    if (readUntilCRLF(newsockfd, buffer) == 0)
                    {
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
#ifdef VERBOSE

                    printf("Message received: %s", buffer);
#endif
                    char isEnd[4];
                    strncpy(isEnd, buffer, 3);
                    if (strcmp(isEnd, ".\r\n") == 0)
                    {
                        const char *start = strchr(forward_path_buffer, '<');
                        const char *end = strchr(forward_path_buffer, '>');
                        if (start != NULL && end != NULL && start < end)
                        {
                            size_t length = end - start - 1;
                            char extracted_forward_path[length + 1];
                            strncpy(extracted_forward_path, start + 1, length);
                            extracted_forward_path[length] = '\0';

                            const char *at = strchr(extracted_forward_path, '@');
                            if (at != NULL)
                            {
                                size_t username_length = at - extracted_forward_path;
                                size_t domain_length = length - username_length - 1;
                                char username[username_length + 1];
                                char domain[domain_length + 1];
                                strncpy(username, extracted_forward_path, username_length);
                                username[username_length] = '\0';
                                strncpy(domain, at + 1, domain_length);
                                domain[domain_length] = '\0';

                                char fileName[username_length + 11];
                                strcpy(fileName, "./");
                                strcat(fileName, username);
                                strcat(fileName, "/mymailbox");
                                writeEmailToFile(reverse_path_buffer, forward_path_buffer, mail_data_buffer, fileName);
                                clearBuffer(buffer);
                                sprintf(buffer, "250 OK Message accepted for delivery\r\n");
                                send(newsockfd, buffer, strlen(buffer), 0);
                                state = QUIT;
                                break;
                            }
                        }
                    }
                    else
                    {
                        strcat(mail_data_buffer, buffer);
                        break;
                    }
                    break;
                }
                }
            }
        }
        else
        {
            close(newsockfd);
        }
    }
}