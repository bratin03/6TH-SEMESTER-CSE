#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};
    const char *message = "Hello from server!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Send message to the client
    // while (1)
    // {
    if (send(new_socket, message, strlen(message), 0) < 0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Message sent to the client\n");
    // }
    const char *message2 = "Hello from server 2!";
    if (send(new_socket, message2, strlen(message2), 0) < 0)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Message sent to the client\n");
    // Receive message from the client
    int valread = recv(new_socket, buffer, MAX_BUFFER_SIZE, 0);
    printf("Message from client: %s\n", buffer);
    return 0;
}
