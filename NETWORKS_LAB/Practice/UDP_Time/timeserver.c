#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

const int PORT = 20000;
const unsigned int BUF_SIZE = 64;

int main()
{
    int sockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("ERROR! Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if ((bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        perror("ERROR! Binding Failed.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is Running on PORT: %d\nWaiting for incoming connections.\n", PORT);
    int bytes_received = 0;
    char *buffer = (char *)malloc(sizeof(char *) * BUF_SIZE);
    if (buffer == NULL)
    {
        perror("ERROR! Memory allocation failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        clilen = sizeof(cli_addr);
        if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&cli_addr, &clilen)) < 0)
        {
            perror("ERROR! recvfrom failed\n");
            free(buffer);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);

        time_t t = time(NULL);
        struct tm *tm = localtime(&t);

        strcpy(buffer, asctime(tm));
        sendto(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&cli_addr, clilen);
    }
    free(buffer);
    close(sockfd);

    return 0;
}