#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // ICMP protocol
                                                          /*
                                                          For tcp use IPPROTO_TCP
                                                          */

    /*
        Binding raw socket - Works at IP Layer
        But at IP layer, there is no notion of port number
    */

    /*
        IPPROTO_RAW - Raw IP packets - Kernel will not fill any header
    */

    int on = 1;
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

    // sendto( buff() )
    // Will have to add all header upto IP layer

    /*
    Fill the IP header
    struct iphdr
    {
        unsigned int ihl:4;
        unsigned int version:4;
        unsigned char tos;
        unsigned short tot_len;
        unsigned short id;
        unsigned short frag_off;
        unsigned char ttl;
        unsigned char protocol;
        unsigned short check;
        unsigned int saddr;
        unsigned int daddr;
    };
    struct tcphdr
    {
        unsigned short source;
        unsigned short dest;
        unsigned int seq;
        unsigned int ack_seq;
        unsigned short res1:4;
        unsigned short doff:4;
        unsigned short fin:1;
        unsigned short syn:1;
        unsigned short rst:1;
        unsigned short psh:1;
        unsigned short ack:1;
        unsigned short urg:1;
        unsigned short res2:2;
        unsigned short window;
        unsigned short check;
        unsigned short urg_ptr;
    };
    struct icmphdr
    {
        unsigned char type;
        unsigned char code;
        unsigned short checksum;
        unsigned short id;
        unsigned short sequence;
    };
    */

    /*
        IPPROTO_ICMP - ICMP packets - Kernel will fill the IP header after taking the ICMP message
    */

   /*
   Receiving a packet

   Kernel generally never sends tcp and udp packets to raw socket

   Capture packet at ethernet layer

   ICMP and IGMP packets are sent to raw socket

   Kernel will send to raw socket only if the packet header is not known

   What happens if we put 0 in socket() call?

   Protocol field 0 means anything received at IP layer will be sent to this socket

   If marked with 0, it filters out the header and sends the data to the socket

   For SOCK_RAW, if we use 0 as protocol, it will receive all packets reveived at IP layer on this device
   
   Even if we use IPPROTO_ICMP, it will receive all ICMP packets received at IP layer on this device

   For TCP, UDP we can distinguish using the port number
   */

}