#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE] = {0};
    const char *message = "Hello from client!";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    // Send message to the server

    // Receive message from the server
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    int addrlen = sizeof(address);
    valread = recvfrom(sock, buffer, MAX_BUFFER_SIZE, MSG_TRUNC, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    buffer[valread] = '\0';
    printf("valread: %d\n", valread);
    printf("Message from server: %s\n", buffer);

    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }

    valread = recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("valread: %d\n", valread);
    printf("Message from server: %s\n", buffer);

    int port = ntohs(address.sin_port);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, ip, INET_ADDRSTRLEN);

    printf("Address: %s\n", ip);
    printf("Port: %d\n", port);

    send(sock, message, strlen(message), 0);
    printf("Message sent to the server\n");

    return 0;
}
