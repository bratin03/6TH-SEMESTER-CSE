#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in addr;

    char *ip = "";
    if (inet_aton(ip, &addr.sin_addr) == 0)
    {
        fprintf(stdout, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", inet_ntoa(addr.sin_addr));
    exit(EXIT_SUCCESS);
}
/* Output of the C code:
Invalid address
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in addr;

    char *ip = "0x7.002.12466";
    if (inet_aton(ip, &addr.sin_addr) == 0)
    {
        fprintf(stdout, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", inet_ntoa(addr.sin_addr));
    exit(EXIT_SUCCESS);
}

/* Output of the C code:
7.2.48.178
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *ip = "0x7.02.12466";
    in_addr_t ip2 = inet_addr(ip);
    printf("IPv4 address in network byte order: %08x\n", ip2);

    struct in_addr addr;
    addr.s_addr = ip2;
    printf("IPv4 address in dotted-decimal notation: %s\n", inet_ntoa(addr));
}
/* Output of the C code:
IPv4 address in network byte order: b2300207
IPv4 address in dotted-decimal notation: 7.2.48.178
*/

