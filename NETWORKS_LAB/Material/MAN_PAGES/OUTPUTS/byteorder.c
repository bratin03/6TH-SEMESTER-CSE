#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int ip = 100;
    printf("ip: %08x\n", ip);

    int ip2 = htonl(ip);
    printf("IPv4 address in network byte order: %08x\n", ip2);

    int ip3 = htons(ip);
    printf("Port number in network byte order: %04x\n", ip3);

    int ip4 = ntohl(ip);
    printf("IPv4 address in host byte order: %08x\n", ip4);

    int ip5 = ntohs(ip);
    printf("Port number in host byte order: %04x\n", ip5);

    return 0;
}

/* Output of the C code:
ip: 00000064
IPv4 address in network byte order: 64000000
Port number in network byte order: 6400
IPv4 address in host byte order: 64000000
Port number in host byte order: 6400
*/
