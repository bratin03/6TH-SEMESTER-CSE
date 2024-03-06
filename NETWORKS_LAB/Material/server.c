#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int listen_fd, conn_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    // Create socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any available interface
    serv_addr.sin_port = htons(8080);              // Listen on port 8080

    // Bind socket to the server address
    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Set socket to listen for connections
    if (listen(listen_fd, 5) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept connections
    while (1)
    {
        if ((conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
        {
            perror("accept");
            continue;
        }

        // Get the local address and port number of the connected socket
        struct sockaddr_in local_addr;
        socklen_t local_len = sizeof(local_addr);
        if (getsockname(conn_fd, (struct sockaddr *)&local_addr, &local_len) == -1)
        {
            perror("getsockname");
            close(conn_fd);
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        printf("Server is listening on port: %d\n", ntohs(local_addr.sin_port));

        // Handle the connection (read/write data)

        close(conn_fd); // Close the connection socket when done
    }

    close(listen_fd); // Close the listening socket

    return 0;
}