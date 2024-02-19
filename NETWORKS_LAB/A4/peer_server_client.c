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
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <signal.h>

// Function to handle SIGINT
void siginthandler(int signum)
{
    printf("\nExiting.\n");
    exit(EXIT_SUCCESS);
}

// Entry in the table
typedef struct
{
    char name[20];
    char ip[33];
    int port;
} table_entry;

// Table of users
table_entry user_info[3];

// Populate the table
void populate_table(table_entry *user_info)
{
    strcpy(user_info[0].name, "user_1");
    strcpy(user_info[0].ip, "127.0.0.1");
    user_info[0].port = 50000;

    strcpy(user_info[1].name, "user_2");
    strcpy(user_info[1].ip, "127.0.0.1");
    user_info[1].port = 50001;

    strcpy(user_info[2].name, "user_3");
    strcpy(user_info[2].ip, "127.0.0.1");
    user_info[2].port = 50002;
}

// Lookup user by name
table_entry lookup(char user_name[])
{
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        if (strcmp(user_info[i].name, user_name) == 0)
        {
            return user_info[i];
        }
    }
}

// read from socket until endline
int readUntilEndline(int sockfd, char buffer[])
{
    int i = 0;
    char c;
    while (1)
    {
        if (recv(sockfd, &c, 1, 0) == 0)
        {
            return 0;
        }
        if (c == '\n')
        {
            buffer[i] = '\0';
            break;
        }
        buffer[i] = c;
        i++;
    }
    return 1;
}

// Main function
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("Usage: %s <user_name>\n", argv[0]);
        printf("For user 1: %s user_1\n", argv[0]);
        printf("For user 2: %s user_2\n", argv[0]);
        printf("For user 3: %s user_3\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, siginthandler);

    // connected_fd keeps track if any connection is established with the user already
    int connected_fd[3];
    for (int i = 0; i < 3; i++)
    {
        connected_fd[i] = 0;
    }

    // Populate the table
    populate_table(user_info);

    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    fd_set readfds;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Find the port number for the user
    if (strcmp(argv[1], "user_1") == 0)
    {
        server_addr.sin_port = htons(user_info[0].port);
    }
    else if (strcmp(argv[1], "user_2") == 0)
    {
        server_addr.sin_port = htons(user_info[1].port);
    }
    else if (strcmp(argv[1], "user_3") == 0)
    {
        server_addr.sin_port = htons(user_info[2].port);
    }
    else
    {
        printf("Invalid user name\n");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Print the port number
    printf("Running on port %d\n", ntohs(server_addr.sin_port));

    // While loop to keep the server running
    while (1)
    {
        // Set the file descriptors
        FD_ZERO(&readfds);
        // Add the server_fd and stdin to the set
        FD_SET(server_fd, &readfds);
        FD_SET(0, &readfds);

        int max_fd = server_fd; // max_fd keeps track of the maximum file descriptor

        for (int i = 0; i < 3; i++)
        {
            if (connected_fd[i] > 0)
            {
                // If any connection is established, add it to the set
                FD_SET(connected_fd[i], &readfds);
                max_fd = (max_fd > connected_fd[i]) ? max_fd : connected_fd[i];
            }
        }

        // Set the timeout
        struct timeval tv;
        tv.tv_sec = 300;
        tv.tv_usec = 0;

        int retval;
        if ((retval = select(max_fd + 1, &readfds, NULL, NULL, &tv)) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (retval == 0)
        {
            // If timeout occurs, close all the connections
            for (int i = 0; i < 3; i++)
            {
                if (connected_fd[i] > 0)
                {
                    close(connected_fd[i]);
                    connected_fd[i] = 0;
                }
            }
        }

        // Check if server_fd is set
        if (FD_ISSET(server_fd, &readfds))
        {
            // If any incoming connection is there, accept it and add it to the connected_fd
            int client_addr_len = sizeof(client_addr);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            char buffer[1024] = {0};
            readUntilEndline(new_socket, buffer);
            char sender[10] = {0};
            for (int i = 0; i < 6; i++)
            {
                sender[i] = buffer[i];
            }
            sender[6] = '\0';
            if (strcmp(sender, "user_1") == 0)
            {
                connected_fd[0] = new_socket;
            }
            else if (strcmp(sender, "user_2") == 0)
            {
                connected_fd[1] = new_socket;
            }
            else if (strcmp(sender, "user_3") == 0)
            {
                connected_fd[2] = new_socket;
            }
            char message[1024] = {0};
            sprintf(message, "%s", buffer + 7);
            printf("Message from %s: %s\n", sender, message);
        }

        for (int i = 0; i < 3; i++)
        {
            // Check if there is any message from the connected_fd
            if (connected_fd[i] > 0 && FD_ISSET(connected_fd[i], &readfds))
            {
                char buffer[1024] = {0};
                if (readUntilEndline(connected_fd[i], buffer) == 0)
                {
                    close(connected_fd[i]);
                    connected_fd[i] = 0;
                    continue;
                }
                char sender[10] = {0};
                for (int i = 0; i < 6; i++)
                {
                    sender[i] = buffer[i];
                }
                sender[6] = '\0';
                char message[1024] = {0};
                sprintf(message, "%s", buffer + 7);
                printf("Message from %s: %s\n", sender, message);
            }
        }

        if (FD_ISSET(0, &readfds))
        {
            // If there is any message from stdin, send it to the user
            char buffer[1024] = {0};
            fgets(buffer, sizeof(buffer), stdin);
            char message[1024] = {0};
            sprintf(message, "%s", buffer + 7);
            char user_name[7] = {0};
            for (int i = 0; i < 6; i++)
            {
                user_name[i] = buffer[i];
            }
            user_name[6] = '\0';
            struct sockaddr_in user_addr;
            user_addr.sin_family = AF_INET;
            user_addr.sin_port = htons(lookup(user_name).port);
            user_addr.sin_addr.s_addr = inet_addr(lookup(user_name).ip);
            char final_message[2048] = {0};
            sprintf(final_message, "%s:%s", argv[1], message);
            int user_no;
            if (strcmp(user_name, "user_1") == 0)
            {
                user_no = 0;
            }
            else if (strcmp(user_name, "user_2") == 0)
            {
                user_no = 1;
            }
            else if (strcmp(user_name, "user_3") == 0)
            {
                user_no = 2;
            }
            if (connected_fd[user_no] == 0)
            {
                // If no connection is established, establish a new connection
                if ((connected_fd[user_no] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    perror("socket");
                    exit(EXIT_FAILURE);
                }
                if (connect(connected_fd[user_no], (struct sockaddr *)&user_addr, sizeof(user_addr)) < 0)
                {
                    perror("connect");
                    exit(EXIT_FAILURE);
                }
                send(connected_fd[user_no], final_message, strlen(final_message), 0);
            }
            else
            {
                // If connection is already established, send the message
                printf("Sending message to %s\n", user_name);
                send(connected_fd[user_no], final_message, strlen(final_message), 0);
            }
        }
    }
    exit(EXIT_SUCCESS);
}
