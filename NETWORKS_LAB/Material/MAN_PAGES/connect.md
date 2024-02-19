### CONNECT

```c
CONNECT(2)                     Linux Programmer's Manual                    CONNECT(2)

NAME
       connect - initiate a connection on a socket

SYNOPSIS
       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>

       int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);

DESCRIPTION
       The  connect() system call connects the socket referred to by the file descrip‐
       tor sockfd to the address specified by addr.  The  addrlen  argument  specifies
       the  size  of addr.  The format of the address in addr is determined by the ad‐
       dress space of the socket sockfd; see socket(2) for further details.

       If the socket sockfd is of type SOCK_DGRAM, then addr is the address  to  which
       datagrams  are  sent  by default, and the only address from which datagrams are
       received.  If the socket is of type SOCK_STREAM or  SOCK_SEQPACKET,  this  call
       attempts to make a connection to the socket that is bound to the address speci‐
       fied by addr.

       Generally, connection-based protocol sockets may  successfully  connect()  only
       once;  connectionless  protocol  sockets  may  use  connect() multiple times to
       change their association.  Connectionless sockets may dissolve the  association
       by connecting to an address with the sa_family member of sockaddr set to AF_UN‐
       SPEC (supported on Linux since kernel 2.2).

RETURN VALUE
       If the connection or binding succeeds, zero is returned.  On error, -1  is  re‐
       turned, and errno is set appropriately.

ERRORS
       The  following  are general socket errors only.  There may be other domain-spe‐
       cific error codes.

       EACCES For UNIX domain sockets, which are identified by pathname: Write permis‐
              sion  is  denied  on the socket file, or search permission is denied for
              one of the directories in  the  path  prefix.   (See  also  path_resolu‐
              tion(7).)

       EACCES, EPERM
              The  user  tried  to  connect  to a broadcast address without having the
              socket broadcast flag enabled or the connection request  failed  because
              of a local firewall rule.

       EADDRINUSE
              Local address is already in use.

       EADDRNOTAVAIL
              (Internet  domain sockets) The socket referred to by sockfd had not pre‐
              viously been bound to an address and, upon attempting to bind it  to  an
              ephemeral port, it was determined that all port numbers in the ephemeral
              port  range   are   currently   in   use.    See   the   discussion   of
              /proc/sys/net/ipv4/ip_local_port_range in ip(7).

       EAFNOSUPPORT
              The passed address didn't have the correct address family in its sa_fam‐
              ily field.

       EAGAIN For nonblocking UNIX domain sockets, the socket is nonblocking, and  the
              connection  cannot be completed immediately.  For other socket families,
              there are insufficient entries in the routing cache.

       EALREADY
              The socket is nonblocking and a previous connection attempt has not  yet
              been completed.

       EBADF  sockfd is not a valid open file descriptor.

       ECONNREFUSED
              A  connect() on a stream socket found no one listening on the remote ad‐
              dress.

       EFAULT The socket structure address is outside the user's address space.

       EINPROGRESS
              The socket is nonblocking and the connection cannot be completed immedi‐
              ately.   (UNIX domain sockets failed with EAGAIN instead.)  It is possi‐
              ble to select(2) or poll(2) for completion by selecting the  socket  for
              writing.   After  select(2)  indicates writability, use getsockopt(2) to
              read the SO_ERROR option at level SOL_SOCKET to determine  whether  con‐
              nect()  completed  successfully  (SO_ERROR  is  zero)  or unsuccessfully
              (SO_ERROR is one of the usual error codes listed  here,  explaining  the
              reason for the failure).

       EINTR  The  system  call  was interrupted by a signal that was caught; see sig‐
              nal(7).

       EISCONN
              The socket is already connected.

       ENETUNREACH
              Network is unreachable.

       ENOTSOCK
              The file descriptor sockfd does not refer to a socket.

       EPROTOTYPE
              The socket type does not support the requested communications  protocol.
              This  error  can occur, for example, on an attempt to connect a UNIX do‐
              main datagram socket to a stream socket.

       ETIMEDOUT
              Timeout while attempting connection.  The server may be too busy to  ac‐
              cept  new connections.  Note that for IP sockets the timeout may be very
              long when syncookies are enabled on the server.

CONFORMING TO
       POSIX.1-2001, POSIX.1-2008, SVr4, 4.4BSD, (connect() first appeared in 4.2BSD).

NOTES
       POSIX.1 does not require the inclusion of <sys/types.h>, and this  header  file
       is  not  required on Linux.  However, some historical (BSD) implementations re‐
       quired this header file, and portable applications are probably wise to include
       it.

       For background on the socklen_t type, see accept(2).

       If  connect() fails, consider the state of the socket as unspecified.  Portable
       applications should close the socket and create a new one for reconnecting.

EXAMPLE
       An example of the use of connect() is shown in getaddrinfo(3).

SEE ALSO
       accept(2), bind(2), getsockname(2), listen(2), socket(2), path_resolution(7)

COLOPHON
       This page is part of release 5.05 of the Linux man-pages project.   A  descrip‐
       tion  of  the project, information about reporting bugs, and the latest version
       of this page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                 2019-03-06                            CONNECT(2)
```