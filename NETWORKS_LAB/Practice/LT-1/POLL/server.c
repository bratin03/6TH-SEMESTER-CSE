#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fds[MAX_CLIENTS], max_clients = 0;
    struct sockaddr_in server_addr, client_addr;
    struct pollfd fds[MAX_CLIENTS + 1]; // +1 for server_fd
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // Add server_fd to pollfd structure
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        // Initialize events for connected clients
        for (int i = 1; i <= max_clients; ++i) {
            fds[i].fd = client_fds[i - 1];
            fds[i].events = POLLIN;
        }

        // Perform poll
        int ret;
        if ((ret=poll(fds, max_clients + 1, 3000)) == -1) {
            perror("Poll error");
            exit(EXIT_FAILURE);
        }
        
        if(ret==0)
        {
        	printf("Timeout\n");
        }

        // Check for incoming connections on server socket
        if (fds[0].revents & POLLIN) {
            int client_fd, client_len = sizeof(client_addr);

            // Accept incoming connection
            if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len)) == -1) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection accepted: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add new client to array
            if (max_clients < MAX_CLIENTS) {
                client_fds[max_clients++] = client_fd;
            } else {
                fprintf(stderr, "Maximum number of clients reached\n");
                close(client_fd);
            }
        }

        // Check for incoming data on client sockets
        for (int i = 0; i < max_clients; ++i) {
            if (fds[i + 1].revents & POLLIN) {
                int read_size;
                if ((read_size = read(client_fds[i], buffer, BUFFER_SIZE)) == 0) {
                    // Connection closed
                    printf("Client disconnected: %d\n", client_fds[i]);
                    close(client_fds[i]);
                    // Remove client from array
                    for (int j = i; j < max_clients - 1; ++j) {
                        client_fds[j] = client_fds[j + 1];
                    }
                    --max_clients;
                } else {
                    // Echo received data back to client
                    buffer[read_size] = '\0';
                    printf("Received data from client %d: %s\n", client_fds[i], buffer);
                    write(client_fds[i], buffer, strlen(buffer));
                }
            }
        }
    }

    return 0;
}

