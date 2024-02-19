#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    struct pollfd fds[MAX_CLIENTS + 1]; // +1 for the server socket
    int nfds = 1;                       // Start with 1 for the server socket
    int new_socket, valread;
    int opt = 1;
    int addrlen = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};

    // Create a TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Initialize client sockets to -1
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = -1;
    }

    // Add server socket to the pollfd array
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    printf("Server started. Waiting for connections...\n");

    while (1)
    {
        // Poll for events
        int ret = poll(fds, nfds, 5000);
        printf("Ret: %d\n", ret);
        if (ret == -1)
        {
            perror("Poll error");
            exit(EXIT_FAILURE);
        }

        // Check if server socket has an event
        if (fds[0].revents & POLLIN)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
            {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add new socket to the pollfd array
            fds[nfds].fd = new_socket;
            fds[nfds].events = POLLIN;
            nfds++;

            // Add new socket to the client_sockets array
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == -1)
                {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Check client sockets for events
        for (int i = 1; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int sd = fds[i].fd;
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(sd);
                    fds[i].fd = -1; // Mark the pollfd as closed
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (client_sockets[j] == sd)
                        {
                            client_sockets[j] = -1;
                            break;
                        }
                    }
                }
                else
                {
                    buffer[valread] = '\0';
                    printf("Client %d: %s", sd, buffer);
                }
            }
        }
    }

    return 0;
}
