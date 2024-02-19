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
gcc mailclient.c -o mailclient
./mailclient <server_IP> <smtp_port> <pop3_port>

VERBOSE:
To enable verbose mode, uncomment the line "#define VERBOSE" in the source code or compiles as
gcc -DVERBOSE mailclient.c -o mailclient
./mailclient <server_IP> <smtp_port> <pop3_port>

Notes:
1. The SMTP part of the code doesn't validate the email address and password as mentioned in the assignment
2. The SMTP part of the code doesn't validate the fact that the sender mail id mathces the username. Practically it is possible to have a different sender mail id apart from the username
3. The SMTP part of the code doesn't validate if the sender's ip address matches with the ip address of the client. Practically it is possible to have a different sender's ip address apart from the client's ip address
4. In the case of natural flow of the transaction is broken, the SMTP client will display the error message to the user and send the QUIT command to the server and exit
5. The POP3 part of the code takes the username and password as input and then connects to the server and validates the username and password
6. The POP3 part of the code prints the mail list and the user can choose to see the mail by entering the mail number
7. The POP3 part of the code also allows the user to delete the mail by entering the mail number and pressing 'd'
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
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

// #define VERBOSE

#define USERNAME_SIZE 100
#define PASSWORD_SIZE 100
#define MAX_LINE_LENGTH 81
#define MAX_NUM_LINES 51
#define MAX_BUFFER_SIZE 4096
#define MAX_MAIL_ADDR 101

#define MENU_POP3 1
#define MENU_SMTP 2
#define MENU_QUIT 3

#define SMTP_GREETING 0
#define SMTP_HELO 1
#define SMTP_MAIL_FROM 2
#define SMTP_RCPT_TO 3
#define SMTP_DATA 4
#define SMTP_MAIL_SEND 5
#define SMTP_EXIT -1
#define SMTP_QUIT -2

#define POP3_GREETING 0
#define POP3_USER 1
#define POP3_PASS 2
#define POP3_COMMUNICATE 3
#define POP3_EXIT -1
#define POP3_QUIT -2

// Function to clear buffer
void clearBuffer(char *buffer)
{
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }
    return;
}

// Function to check if the given string is a valid email address
int isValidEmail(const char *email)
{
    const char *atSymbol = strchr(email, '@');
    if (atSymbol != NULL)
    {

        const char *domainPart = atSymbol + 1;

        struct in_addr ipv4;
        if (inet_pton(AF_INET, domainPart, &ipv4) == 1)
        {
            return 1;
        }

        struct in6_addr ipv6;
        if (inet_pton(AF_INET6, domainPart, &ipv6) == 1)
        {
            return 1;
        }
        return (strstr(email, "@") != NULL && strstr(email, ".") != NULL);
    }
    return 0;
}

// Function to read a line from stdin
void readLine(char *buffer, int maxLength)
{
    fgets(buffer, maxLength, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
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
            printf("Server hang up unexpectedly\n");
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

// Function to extract reply code from the received string
void extractReplyCode(const char *receivedString, char *reply_code)
{

    if (receivedString != NULL && strlen(receivedString) >= 3)
    {
        strncpy(reply_code, receivedString, 3);
        reply_code[3] = '\0';
    }
    else
    {
        return;
    }
}

// Function to communicate with the server for the mail part
void extractReplyCode_pop3(const char *buffer, char *replyCode)
{
    if (buffer == NULL || replyCode == NULL)
    {
        return;
    }

    if (strncmp(buffer, "+OK", 3) == 0)
    {
        strncpy(replyCode, "+OK", 3);
        replyCode[3] = '\0';
    }
    else if (strncmp(buffer, "-ERR", 4) == 0)
    {
        strncpy(replyCode, "-ERR", 4);
        replyCode[4] = '\0';
    }
    else
    {
        strncpy(replyCode, buffer, 3);
        replyCode[3] = '\0';
    }
}

// Function to print the mail list
void printMailList(int sockfd, int numberOfEmails)
{
    printf("%5s\t%-30s\t%-30s\t%-30s\n", "Sl. No.", "Sender", "Received Time", "Subject");
    int i;
    for (i = 1; i <= numberOfEmails; i++)
    {
        char buffer[MAX_BUFFER_SIZE];
        clearBuffer(buffer);
        char reply_code[4];

        char sender[100], receivedTime[100], subject[100];
        sprintf(buffer, "RETR %d\r\n", i);
        if (send(sockfd, buffer, strlen(buffer), 0) < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        clearBuffer(buffer);
        if (readUntilCRLF(sockfd, buffer) == 0)
        {
            close(sockfd);
            exit(EXIT_FAILURE);
        }
#ifdef VERBOSE
        printf("Message Received: %s", buffer);
#endif
        extractReplyCode_pop3(buffer, reply_code);
        if (strcmp(reply_code, "+OK") != 0)
        {
            continue;
        }

        while (1)
        {
            clearBuffer(buffer);
            if (readUntilCRLF(sockfd, buffer) == 0)
            {
                close(sockfd);
                exit(EXIT_FAILURE);
            }
#ifdef VERBOSE
            printf("Message Received: %s", buffer);
#endif
            if (buffer[0] == '.' && buffer[1] == '\r' && buffer[2] == '\n')
            {
                printf("%5d\t%-30s\t%-30s\t%-30s\n", i, sender, receivedTime, subject);
                break;
            }
            char key[100];
            char content[100];
            char *token = strtok((char *)buffer, " ");
            if (token != NULL)
            {
                strncpy(key, token, sizeof(key) - 1);
                key[sizeof(key) - 1] = '\0';
                token = strtok(NULL, "\r\n");
                if (token != NULL)
                {
                    strncpy(content, token, sizeof(content) - 1);
                    content[sizeof(content) - 1] = '\0';
                }
                if (strcmp(key, "From:") == 0)
                {
                    strcpy(sender, content);
                }
                else if (strcmp(key, "Received:") == 0)
                {
                    strcpy(receivedTime, content);
                }
                else if (strcmp(key, "Subject:") == 0)
                {
                    strcpy(subject, content);
                }
            }
        }
    }
}

// Function to communicate with the server for the mail part
void communicateForMail(int sockfd)
{
    char buffer[MAX_BUFFER_SIZE];
    clearBuffer(buffer);
    char reply_code[4];
    sprintf(buffer, "STAT\r\n");
    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    clearBuffer(buffer);
    if (readUntilCRLF(sockfd, buffer) == 0)
    {
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    extractReplyCode_pop3(buffer, reply_code);
#ifdef VERBOSE
    printf("Reply code: %s\n", reply_code);
    printf("Message received: %s", buffer);
#endif
    int numberOfEmails;
    sscanf(buffer, "+OK %d", &numberOfEmails);

    while (1)
    {
        int indent = 0;
        for (indent = 0; indent < 40; indent++)
        {
            printf("-");
        }
        printf("User Mailbox");
        for (indent = 0; indent < 40; indent++)
        {
            printf("-");
        }
        printf("\n");
        printMailList(sockfd, numberOfEmails);
        int mail_no;
        printf("\nEnter mail no. to see: ");
        scanf("%d", &mail_no);
        if (mail_no == -1)
        {
            break;
        }
        clearBuffer(buffer);
        sprintf(buffer, "RETR %d\r\n", mail_no);
        if (send(sockfd, buffer, strlen(buffer), 0) < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        clearBuffer(buffer);
        if (readUntilCRLF(sockfd, buffer) == 0)
        {
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        extractReplyCode_pop3(buffer, reply_code);
        if (strcmp(reply_code, "+OK") != 0)
        {
            printf("\nMail no. out of range, give again.\n\n");
            continue;
        }
        else
        {
            clearBuffer(buffer);
            printf("\n");
            while (1)
            {
                clearBuffer(buffer);
                if (readUntilCRLF(sockfd, buffer) == 0)
                {
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                if (buffer[0] == '.' && buffer[1] == '\r' && buffer[2] == '\n')
                {
                    break;
                }
                size_t bufferLength = strlen(buffer);
                if (bufferLength >= 2 && buffer[bufferLength - 2] == '\r' && buffer[bufferLength - 1] == '\n')
                {
                    buffer[bufferLength - 2] = '\0';
                    buffer[bufferLength - 1] = '\0';
                }
                printf("%s\n", buffer);
            }
            getchar();
            char key;
            key = getchar();
            if (key == 'd')
            {
                clearBuffer(buffer);
                sprintf(buffer, "DELE %d\r\n", mail_no);
                if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }
                clearBuffer(buffer);
                if (readUntilCRLF(sockfd, buffer) == 0)
                {
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                extractReplyCode_pop3(buffer, reply_code);

#ifdef VERBOSE
                printf("Reply code: %s\n", reply_code);
                printf("Message received: %s", buffer);
#endif
                if (strcmp(reply_code, "+OK") == 0)
                {
                    printf("\nMail deleted.\n\n");
                }
                else
                {
                    printf("\nMail delete failed.\n\n");
                }
            }
        }
    }
}

// Main function
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 4)
    {
        printf("ERROR!\nUsage: %s <server_IP> <smtp_port> <pop3_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get server IP and port numbers
    char *server_IP = argv[1];
    int smtp_port = atoi(argv[2]);
    int pop3_port = atoi(argv[3]);

    // For the smtp part, we do not validate the email address and password as mentioned in the assignment
    // Proper checks are done for the pop3 part

    // For the smtp part we also do not validate the fact that the sender mail id mathces the username. Practically it is possible to have a different sender mail id apart from the username
    // We also do not validate if the sender's ip address matches with the ip address of the client. Practically it is possible to have a different sender's ip address apart from the client's ip address
    // Take username and password as input

    // For the pop3 part, we take the username and password as input and then connect to the server and validate the username and password

    char username[USERNAME_SIZE];
    printf("Enter Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    char password[PASSWORD_SIZE];
    printf("Enter Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    int sockfd;
    struct sockaddr_in smtp_server_addr;
    smtp_server_addr.sin_family = AF_INET;
    smtp_server_addr.sin_port = htons(smtp_port);
    inet_aton(server_IP, &smtp_server_addr.sin_addr);

    struct sockaddr_in pop3_server_addr;
    pop3_server_addr.sin_family = AF_INET;
    pop3_server_addr.sin_port = htons(pop3_port);
    inet_aton(server_IP, &pop3_server_addr.sin_addr);

    while (1)
    {
        int key;
        printf("\n1. Manage Mail \n2. Send Mail \n3. Quit\nChoose an option(1,2,3): ");
        scanf("%d", &key);
        getchar();
        printf("\n");
        // State machine to handle the menu
        switch (key)
        {
        case MENU_POP3:
        {
            // POP3
            // The following code is a state machine to handle the POP3 protocol
            // It takes the input from the user and connects to the server using the POP3 protocol

            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("socket");
                exit(EXIT_FAILURE);
            }

            if (connect(sockfd, (struct sockaddr *)&pop3_server_addr, sizeof(pop3_server_addr)) < 0)
            {
                perror("connect");
                exit(EXIT_FAILURE);
            }

            int state = POP3_GREETING;
            char reply_code[4];

            char buffer[MAX_BUFFER_SIZE];
            while (1)
            {
                if (state == POP3_EXIT)
                {
                    break;
                }
                // State machine to handle the POP3 protocol
                switch (state)
                {
                case POP3_GREETING:
                {
                    // State 0 is the initial state
                    // In this state, the client waits for the server to send a greeting message
                    // The client then transitions to next state to send the USER command
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = POP3_EXIT;
                        break;
                    }
                    extractReplyCode_pop3(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "+OK") != 0)
                    {
                        printf("Server not ready.\n");
                        state = POP3_EXIT;
                        break;
                    }
                    state = POP3_USER;
                    break;
                }
                case POP3_USER:
                {
                    // State 1 is the USER state
                    // In this state, the client sends the USER command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "USER %s\r\n", username);
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = POP3_EXIT;
                        break;
                    }
                    extractReplyCode_pop3(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "+OK") != 0)
                    {
                        printf("Username not found.\n");
                        state = POP3_QUIT;
                        break;
                    }
                    state = POP3_PASS;
                    break;
                }
                case POP3_PASS:
                {
                    // State 2 is the PASS state
                    // In this state, the client sends the PASS command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "PASS %s\r\n", password);
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = POP3_EXIT;
                        break;
                    }
                    extractReplyCode_pop3(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "+OK") != 0)
                    {
                        printf("%s", buffer + 5);
                        state = POP3_QUIT;
                        break;
                    }
                    state = POP3_COMMUNICATE;
                }
                case POP3_COMMUNICATE:
                {
                    // State 3 is the COMMUNICATE state
                    // In this state, the client sends the LIST command to the server
                    // It also reads the reply from the server and transitions to the next state
                    communicateForMail(sockfd);
                    state = POP3_QUIT;
                    break;
                }
                case POP3_QUIT:
                {
                    // State -1 is the QUIT state
                    // In this state, the client sends the QUIT command to the server
                    // It also reads the reply from the server and transitions to the exit state
                    clearBuffer(buffer);
                    sprintf(buffer, "QUIT\r\n");
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = POP3_EXIT;
                        break;
                    }
                    extractReplyCode_pop3(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "+OK") != 0)
                    {
                        printf("QUIT failed.\n");
                        state = POP3_QUIT;
                        break;
                    }
                    close(sockfd);
                    state = POP3_EXIT;
                    break;
                }
                }
            }

            break;
        }
        case MENU_SMTP:
        {
            // SMTP
            // The following code is a state machine to handle the SMTP protocol
            // It takes the input from the user and sends the mail to the server using the SMTP protocol

            // Take input from the user
            char from[MAX_MAIL_ADDR], to[MAX_MAIL_ADDR], subject[MAX_MAIL_ADDR], body[MAX_NUM_LINES][MAX_LINE_LENGTH + 1];
            int numLines = 0;

            printf("From: ");
            readLine(from, sizeof(from));
            printf("To: ");
            readLine(to, sizeof(to));
            printf("Subject: ");
            readLine(subject, sizeof(subject));
            while (1)
            {
                char line[MAX_LINE_LENGTH];
                readLine(line, sizeof(line));
                if (strcmp(line, ".") == 0)
                {
                    break;
                }
                strcpy(body[numLines], line);
                numLines++;
            }

            // Validate the input
            if (isValidEmail(from) == 0 || isValidEmail(to) == 0 || strlen(subject) == 0)
            {
                printf("Incorrect format.\n");
                break;
            }

            // Create a socket and open a connection with the server
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("socket");
                exit(EXIT_FAILURE);
            }

            // Get the client IP address
            char client_IP[INET_ADDRSTRLEN];
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            if (getsockname(sockfd, (struct sockaddr *)&client_addr, &client_addr_len) == -1)
            {
                perror("getsockname");
                exit(EXIT_FAILURE);
            }
            // Connect to the server
            if (connect(sockfd, (struct sockaddr *)&smtp_server_addr, sizeof(smtp_server_addr)) < 0)
            {
                perror("connect");
                exit(EXIT_FAILURE);
            }

            // State machine to handle the SMTP protocol
            int state = SMTP_GREETING;
            char reply_code[4];
            char buffer[MAX_BUFFER_SIZE];
            while (1)
            {
                if (state == SMTP_EXIT)
                {
                    break;
                }
                switch (state)
                {
                case SMTP_GREETING:
                {
                    // State 0 is the initial state
                    // In this state, the client waits for the server to send a greeting message
                    // The client then transitions to next state to send the HELO command
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE

                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "220") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_EXIT;
                        break;
                    }
                    state = SMTP_HELO;
                    break;
                }
                case SMTP_HELO:
                {
                    // State 1 is the HELO state
                    // In this state, the client sends the HELO command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "HELO %s\r\n", inet_ntop(AF_INET, &client_addr.sin_addr, client_IP, INET_ADDRSTRLEN));
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE

                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "250") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    state = SMTP_MAIL_FROM;
                    break;
                }
                case SMTP_MAIL_FROM:
                {
                    // State 2 is the MAIL FROM state
                    // In this state, the client sends the MAIL FROM command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "MAIL FROM:<%s>\r\n", from);
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "250") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    state = SMTP_RCPT_TO;
                    break;
                }
                case SMTP_RCPT_TO:
                {
                    // State 3 is the RCPT TO state
                    // In this state, the client sends the RCPT TO command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "RCPT TO:<%s>\r\n", to);
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif

                    if (strcmp(reply_code, "250") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    state = SMTP_DATA;
                    break;
                }
                case SMTP_DATA:
                {
                    // State 4 is the DATA state
                    // In this state, the client sends the DATA command to the server
                    // It also reads the reply from the server and transitions to the next state
                    clearBuffer(buffer);
                    sprintf(buffer, "DATA\r\n");
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "354") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    state = SMTP_MAIL_SEND;
                    break;
                }
                case SMTP_MAIL_SEND:
                {
                    // State 5 is the MAIL SEND state
                    // In this state, the client sends the mail to the server line by line
                    // It also reads the reply from the server and transitions to the next state

                    // Send the Sender, Receiver and Subject
                    clearBuffer(buffer);
                    sprintf(buffer, "From: %s\r\nTo: %s\r\nSubject: %s\r\n", from, to, subject);
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }

                    // Send the body of the mail line by line
                    for (int i = 0; i < numLines; i++)
                    {
                        clearBuffer(buffer);
                        sprintf(buffer, "%s\r\n", body[i]);
                        if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                        {
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    }

                    // Send the end of mail indicator
                    clearBuffer(buffer);
                    sprintf(buffer, ".\r\n");
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    clearBuffer(buffer);
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "250") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    printf("Mail sent successfully.\n");
                    state = SMTP_QUIT;
                    break;
                }
                case SMTP_QUIT:
                {
                    // State -1 is the QUIT state
                    // In this state, the client sends the QUIT command to the server
                    // It also reads the reply from the server and transitions to the exit state
                    clearBuffer(buffer);
                    sprintf(buffer, "QUIT\r\n");
                    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    if (readUntilCRLF(sockfd, buffer) == 0)
                    {
                        close(sockfd);
                        state = SMTP_EXIT;
                        break;
                    }
                    extractReplyCode(buffer, reply_code);
#ifdef VERBOSE
                    printf("Reply code: %s\n", reply_code);
                    printf("Message received: %s", buffer);
#endif
                    if (strcmp(reply_code, "221") != 0)
                    {
                        printf("Error in sending mail: %s", buffer);
                        state = SMTP_QUIT;
                        break;
                    }
                    close(sockfd);
                    state = SMTP_EXIT;
#ifdef VERBOSE
                    printf("Connection closed.\n");
#endif
                    break;
                }
                }
            }
            break;
        }
        case 3:
        {
            // Quit
            printf("Quitting...\n");
            exit(EXIT_SUCCESS);
            break;
        }

        default:
        {
            // Invalid option
            printf("Invalid option. Please select option 1 or 2 or 3.\n");
            break;
        }
        }
    }
}
