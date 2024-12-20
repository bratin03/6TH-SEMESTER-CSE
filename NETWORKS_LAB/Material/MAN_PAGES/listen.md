### LISTEN 

```c
LISTEN(2)                      Linux Programmer's Manual                     LISTEN(2)

NAME
       listen - listen for connections on a socket

SYNOPSIS
       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>

       int listen(int sockfd, int backlog);

DESCRIPTION
       listen()  marks  the socket referred to by sockfd as a passive socket, that is,
       as a socket that will be used to accept incoming connection requests using  ac‐
       cept(2).

       The  sockfd  argument  is  a  file  descriptor  that refers to a socket of type
       SOCK_STREAM or SOCK_SEQPACKET.

       The backlog argument defines the maximum length to which the queue  of  pending
       connections  for  sockfd  may  grow.   If a connection request arrives when the
       queue is full, the client may receive an error with an indication  of  ECONNRE‐
       FUSED  or,  if the underlying protocol supports retransmission, the request may
       be ignored so that a later reattempt at connection succeeds.

RETURN VALUE
       On success, zero is returned.  On error, -1 is returned, and errno is  set  ap‐
       propriately.

ERRORS
       EADDRINUSE
              Another socket is already listening on the same port.

       EADDRINUSE
              (Internet  domain sockets) The socket referred to by sockfd had not pre‐
              viously been bound to an address and, upon attempting to bind it  to  an
              ephemeral port, it was determined that all port numbers in the ephemeral
              port  range   are   currently   in   use.    See   the   discussion   of
              /proc/sys/net/ipv4/ip_local_port_range in ip(7).

       EBADF  The argument sockfd is not a valid file descriptor.

       ENOTSOCK
              The file descriptor sockfd does not refer to a socket.

       EOPNOTSUPP
              The socket is not of a type that supports the listen() operation.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, 4.4BSD (listen() first appeared in 4.2BSD).

NOTES
       To accept connections, the following steps are performed:

           1.  A socket is created with socket(2).

           2.  The  socket  is  bound  to a local address using bind(2), so that other
               sockets may be connect(2)ed to it.

           3.  A willingness to accept incoming connections and a queue limit for  in‐
               coming connections are specified with listen().

           4.  Connections are accepted with accept(2).

       POSIX.1  does  not require the inclusion of <sys/types.h>, and this header file
       is not required on Linux.  However, some historical (BSD)  implementations  re‐
       quired this header file, and portable applications are probably wise to include
       it.

       The behavior of the backlog argument on TCP sockets  changed  with  Linux  2.2.
       Now it specifies the queue length for completely established sockets waiting to
       be accepted, instead of the number of incomplete connection requests.  The max‐
       imum   length   of   the   queue  for  incomplete  sockets  can  be  set  using
       /proc/sys/net/ipv4/tcp_max_syn_backlog.  When syncookies are enabled  there  is
       no logical maximum length and this setting is ignored.  See tcp(7) for more in‐
       formation.

       If the backlog argument is greater than the value in  /proc/sys/net/core/somax‐
       conn,  then  it  is silently truncated to that value.  Since Linux 5.4, the de‐
       fault in this file is 4096; in earlier kernels, the default value is  128.   In
       kernels  before  2.4.25, this limit was a hard coded value, SOMAXCONN, with the
       value 128.

EXAMPLE
       See bind(2).

SEE ALSO
       accept(2), bind(2), connect(2), socket(2), socket(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.   A  descrip‐
       tion  of  the project, information about reporting bugs, and the latest version
       of this page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                 2020-02-09                             LISTEN(2)
```