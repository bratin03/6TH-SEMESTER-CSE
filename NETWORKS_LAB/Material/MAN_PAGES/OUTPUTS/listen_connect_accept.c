// Initially

// Server side
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd, newsockfd;
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;

    int i;
    char buf[100];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20000);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    listen(sockfd, 5);
    while (1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("Accept error\n");
            exit(0);
        }
        strcpy(buf, "Message from server");
        send(newsockfd, buf, strlen(buf) + 1, 0);
        recv(newsockfd, buf, 100, 0);
        printf("%s\n", buf);

        close(newsockfd);
    }
    return 0;
}

// Client side
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    int i;
    char buf[100];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Unable to create socket\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(20000);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                 sizeof(serv_addr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }

    for (i = 0; i < 100; i++)
    {
        buf[i] = '\0';
    }
    recv(sockfd, buf, 100, 0);
    printf("%s\n", buf);

    strcpy(buf, "Message from client");
    send(sockfd, buf, strlen(buf) + 1, 0);

    close(sockfd);
    return 0;
}

// Server output
/*
Message from client
^C
*/
// Client output
/*
Message from server
*/

// Change
// From
listen(sockfd, 5);
// to (listening twice)
listen(sockfd, 5);
if (listen(sockfd, 5) < 0)
{
    int errsv = errno;
    printf("Error Number: %s\n", strerror(errsv));
    perror("Unable to listen\n");
    exit(0);
}

// No error encountered

/*
Two socket can not listen on the same port
Although they can listen on different ports
*/

// Change from
listen(sockfd, 5);
// to
if (listen(sockfd, 5) == -1)
{
    int error = errno;
    printf("Error code: %d\n", error);
    perror("Error listening on socket");
    close(sockfd);
    exit(EXIT_FAILURE);
}
// and
if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
    perror("Cannot create socket\n");
    exit(0);
}
// to
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
    perror("Cannot create socket\n");
    exit(0);
}

// Output
// Error code: 95
// Error listening on socket: Operation not supported

// Change
// From
if ((connect(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr))) < 0)
{
    perror("Unable to connect to server\n");
    exit(0);
}
// To
if ((connect(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr))) < 0)
{
    perror("Unable to connect to server\n");
    exit(0);
}
if ((connect(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr))) < 0)
{
    perror("Unable to connect to server\n");
    exit(0);
}

// Connect twice
// Output
// Unable to connect to server
// : Transport endpoint is already connected
// Error code is 106 (added later)

// Change
// Only run the client
// Output
// Error code: 111
// Unable to connect to server
// : Connection refused

// Now we make the client non-blocking
int flags = fcntl(sockfd, F_GETFL, 0);
if (flags == -1)
{
    perror("fcntl");
    exit(EXIT_FAILURE);
}

if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
{
    perror("fcntl");
    exit(EXIT_FAILURE);
}

// Run only the client
// Output
// Error code: 115
// Unable to connect to server
// : Operation now in progress

// Run both client and server
// Output
// Client side;
// Error code: 115
// Unable to connect to server
// : Operation now in progress

// Server side
// Message from server

//(Note that the message came back)

// Making the server non-blocking didn't change anything

// Change
// Used UDP server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 1024

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int n;
    socklen_t len;
    char buffer[MAXLINE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8181);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("\nServer Running....\n");

    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
    buffer[n] = '\0';
    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}
// client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
	int sockfd;
	struct sockaddr_in serv_addr;

	int i;
	char buf[100];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Unable to create socket\n");
		exit(0);
	}



	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(8181);

	if ((connect(sockfd, (struct sockaddr *)&serv_addr,
				 sizeof(serv_addr))) < 0)
	{
		int error = errno;
		printf("Error code: %d\n", error);
		perror("Unable to connect to server\n");
		exit(0);
	}

	for (i = 0; i < 100; i++)
	{
		buf[i] = '\0';
	}

	strcpy(buf, "Message from client");
	send(sockfd, buf, strlen(buf) + 1, 0);

	close(sockfd);
	return 0;
}

//Output
//Server
// Server Running....
// Message from client

// Client

// UDP Connect

// Genreally we use sendto and recvfrom for UDP
// If we first use the connect function, then we can use send only instead of sendto
// and recv only instead of recvfrom
// Nore: we can listen on a UDP socket