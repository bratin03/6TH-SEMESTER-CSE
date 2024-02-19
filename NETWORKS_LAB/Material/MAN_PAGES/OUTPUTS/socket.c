#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

int main()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stderr, "Error: %s\n", errstr);
        perror("socket");
        exit(1);
    }
    printf("%d\n", fd);
}

/* Output of the C code:
3
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

int main()
{
    int fd;
    fd = socket(AF_INET, -1, 0);
    if (fd == -1)
    {
        int errornum = errno;
        const char *errstr = strerror(errornum);
        fprintf(stdout, "Error: %s\n", errstr);
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("%d\n", fd);
}

/* Output of the C code:
Error: Invalid argument
*/

