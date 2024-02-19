#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

const int PORT = 20000;
const unsigned int BUF_SIZE = 64;

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct pollfd poll_set[1];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("ERROR! Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    int n;
    socklen_t len;
    int attempts = 0;

    for (attempts = 0; attempts < 5; attempts++)
    {
        char *buffer = (char *)malloc(BUF_SIZE * sizeof(char));
        if (buffer == NULL)
        {
            perror("ERROR! Memory allocation failed.\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        char *hello = "time request";
        sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

        poll_set[0].fd = sockfd;
        poll_set[0].events = POLLIN;
        int poll_result;
        int bytes_received = sizeof(serv_addr);
        socklen_t servlen;

        if ((poll_result = poll(poll_set, 1, 3000)) < 0)
        {
            perror("POLL ERROR!\n");
            free(buffer);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (poll_result == 0)
        {
            free(buffer);
            continue;
        }

        if (poll_set[0].revents & POLLIN)
        {
            if ((bytes_received = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&serv_addr, &servlen)) < 0)
            {
                perror("ERROR! recvfrom failed.\n");
                free(buffer);
                close(sockfd);
                exit(EXIT_FAILURE);
            }

            buffer[bytes_received] = '\0';
            printf("%s\n", buffer);
            free(buffer);
            break;
        }
        free(buffer);
    }

    if (attempts == 5)
    {
        printf("Timelimit Excceded! Timed Out\n");
    }
    close(sockfd);
    return 0;
}