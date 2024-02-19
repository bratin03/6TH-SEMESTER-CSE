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

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_sockets[MAX_CLIENTS];
    fd_set readfds;
    struct sockaddr_in server_addr, client_addr;
    int max_sd, sd, activity, new_socket, valread;
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

    // Initialize client sockets to 0
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

    printf("Server started. Waiting for connections...\n");

    while (1)
    {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add server socket to the set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        printf("Server socket %d added to the set\n", server_fd);

        // Add client sockets to the set
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
                printf("Client socket %d added to the set\n", sd);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // Wait for activity on any of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        printf("Activity: %d\n", activity);
        printf("Timeout: %ld\n", timeout.tv_sec);
        printf("Timeout: %ld\n", timeout.tv_usec);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("Select error");
        }

        // Check server socket for activity
        if (FD_ISSET(server_fd, &readfds))
        {
            printf("Server socket %d is ready\n", server_fd);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
            {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add new socket to array of client sockets
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Check client sockets for activity
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds))
            {
                printf("Client socket %d is ready\n", sd);
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    /*

                    When a client disconnects from the server, it typically results in the corresponding socket becoming ready for reading, which select() detects and reports. 
                    This behavior occurs because when a client disconnects, the server receives an "end-of-file" indication on the socket associated with that client. 
                    This indication means that there is no more data to be read from that socket, so the socket is marked as ready for reading by select().

                    Here's a simplified sequence of events:

                    The client sends a disconnect signal or closes its socket.
                    The server receives an "end-of-file" indication when it attempts to read from the socket associated with that client.
                    When the server calls select(), it detects that the socket associated with the disconnected client is ready for reading.
                    The server can then handle the disconnection by closing the socket, removing it from the list of active sockets, and performing any necessary cleanup.
                    So, in summary, select() returns when there is activity on the sockets being monitored, including when a client disconnects, which causes its socket to become ready for reading.
                    */

                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(sd);
                    client_sockets[i] = 0;
                }
                else
                {
                    printf("Received: %s\n", buffer);
                }
            }
        }
    }

    return 0;
}
