#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM,0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}


/* Output of the C code:

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Trying to bind again\n");
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
Error Number: 22
Trying to bind again
Error: Invalid argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }

    int fd2;
    fd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (fd2 == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(fd2, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        fprintf(stdout, "Trying to bind again from a different socket\n");
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
Error Number: 98
Trying to bind again from a different socket
Error: Address already in use
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serv_aadr_2;
    serv_aadr_2.sin_family = AF_INET;
    serv_aadr_2.sin_port = htons(10000);
    serv_aadr_2.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr_2, sizeof(serv_aadr_2)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        fprintf(stdout,"Trying to bind to a different port from the same socket\n");
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
Error Number: 22
Trying to bind to a different port from the same socket
Error: Invalid argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = 0;
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "sin_port was initially set to 0, but after bind() it is: ");
    fprintf(stdout, "%d", serv_aadr.sin_port);
    fprintf(stdout, "\nNo error was thrown");
}


/* Output of the C code:
sin_port was initially set to 0, but after bind() it is: 0
No error was thrown
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int fd = dup(1);
    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        fprintf(stdout, "Invalid fd\n");
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}


/* Output of the C code:
Error Number: 88
Invalid fd
Error: Socket operation on non-socket
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{

    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(3, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        fprintf(stdout, "Invalid fd\n");
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}
/* Output of the C code:
Error Number: 9
Invalid fd
Error: Bad file descriptor
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, 0) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

/* Output of the C code:
Error Number: 22
Error: Invalid argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_IPX;
    serv_aadr.sin_port = htons(8181);
    serv_aadr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}
/* Output of the C code:
Error Number: 97
Error: Address family not supported by protocol
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_aadr;

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful\n");
    // Print details od serv_addr
    printf("Server Address: %s\n", inet_ntoa(serv_aadr.sin_addr));
    printf("Server Port: %d\n", ntohs(serv_aadr.sin_port));
    printf("Server Family: %d\n", serv_aadr.sin_family);
    printf("Server Socket: %d\n", fd);
}


/* Output of the C code:
Bind successful
Server Address: 0.0.0.0
Server Port: 0
Server Family: 0
Server Socket: 3
*/

// AF_UNSPEC: 0
// On this using SOCK_STREAM, the bind() function will not throw an error, but the server address, port, and family will be 0.
// Listen also will not throw an error, but the server will not be able to accept any connections.
// Accept will throw an error, as the server is not listening to any port.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_aadr;
    serv_aadr.sin_family = AF_INET;
    serv_aadr.sin_port = htons(8181);
    inet_aton("10.1.1.1", &serv_aadr.sin_addr);
    

    if (bind(fd, (struct sockaddr *)&serv_aadr, sizeof(serv_aadr)) == -1)
    {
        int errornum = errno;
        fprintf(stdout, "Error Number: %d\n", errornum);
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("bind");
        exit(EXIT_FAILURE);
    }
}
/* Output of the C code:
Error Number: 99
Error: Cannot assign requested address
*/
