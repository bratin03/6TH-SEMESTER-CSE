### Socket

```c
int socket(int domain, int type, int protocol);
```

1. Returns a file descriptor. Lowest file descriptor number possible
2. Domain argument  
a. AF_INET --> IPv4 Internet protocols
3. Type argument  
a. SOCK_STREAM --> Provides sequenced reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.   
b.  SOCK_DGRAM --> Supports datagrams (connectionless unreliable messages of a fixed maximum length).

Can also be used to set flags:
1) SOCK_NONBLOCK
2) SOCK_CLOEXEC
3. ***SOCK_STREAM***: Full-duplex byte streams.  
a. Must be in a connected state.  
b. Ensure data is not lost or duplicated  
c. Connection considered dead if for piece of data for whichc the peer protocol has buffer space cannot be succesfully transmitted. ***SO_KEEPALIVE*** enabled check if other end is still alive.  
d. A SIGPIPE signal is raised if a process sends or receives on a broken stream; this causes naive processes, which do not handle the signal, to exit.

### inet

```c
int inet_aton(const char *cp, struct in_addr *inp);
```

1. inet_aton() converts the Internet host address cp from  the  IPv4  numbers-and-dots  notation  into  binary  form (in network byte order) and stores it in the structure that inp points to.  inet_aton() returns nonzero if  the  address  is valid,  zero  if not.  
forms:  
a)  a.b.c.d --> each part presents a byte  
b) a.b.c --> a =  1 byte, b = 1 byte, c = 2 bytes  
c) a.b --> a = 1 byte, b = 3 byte  
d) a --> a = 1 byte  
Each of a,b,c,d can be decimal,octal (with a leading 0), hexadecmial (with a leading 0X)  
2. Return 1 on success, 0 on failure.

```c
in_addr_t inet_addr(const char *cp);
```
1.  The inet_addr() function converts the Internet host address cp from  IPv4  numbers-and-dots notation into binary data in network byte order.  
2.  If the input is
       invalid, INADDR_NONE (usually -1) is returned.  Use of this function  is  prob‐
       lematic  because -1 is a valid address (255.255.255.255)

```c
in_addr_t inet_network(const char *cp);
```

1.  The  inet_network() function converts cp, a string in IPv4 numbers-and-dots no‐
       tation, into a number in host byte order suitable for use as an  Internet  net‐
       work  address.  On success, the converted address is returned.  If the input is
       invalid, -1 is returned.  

### BYTEORDER

```c
uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);
```

1.  The  htonl() function converts the unsigned integer hostlong from host byte or‐
       der to network byte order.

       The htons() function converts the unsigned short integer  hostshort  from  host
       byte order to network byte order.

       The  ntohl()  function  converts the unsigned integer netlong from network byte
       order to host byte order.

       The ntohs() function converts the unsigned short integer netshort from  network
       byte order to host byte order.

       On  the  i386  the host byte order is Least Significant Byte first, whereas the
       network byte order, as used on the Internet, is Most Significant Byte first.

### BIND
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
```

### SEND

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
```
1. send() only when in connected state.  
2. send() without flags is write
3. send(sockfd, buf, len, flags) = sendto(sockfd, buf, len, flags, NULL, 0)
4. sendto() in connection mode ignores dest_addr and addrlen. Returns error if not actually connected
5. If the message is too long to pass atomically through the underlying  protocol,
       the error EMSGSIZE is returned, and the message is not transmitted.
6.  If the message is too long to pass atomically through the underlying  protocol,
       the error EMSGSIZE is returned, and the message is not transmitted.

7.  No  indication of failure to deliver is implicit in a send().  Locally detected
       errors are indicated by a return value of -1.
8.  When the message does not fit into the send buffer of the socket,  send()  normally  blocks,  unless  the socket has been placed in nonblocking I/O mode.  In
       nonblocking mode it would fail with the error EAGAIN  or  EWOULDBLOCK  in  this
       case.

### RECV
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

1. recv() without flags is read()
2. recv(sockfd, buf, len, flags) = recvfrom(sockfd, buf, len, flags, NULL, NULL)
3. Returns length of message. If supplied buffer less, excess bytes discarded.
4. No message available, blocking waits / Non-blocking returns -1 and errno is EAGAIN or EWOULDBLOCK.
5. Return any data available, up to the
       requested amount, rather than waiting for receipt of the full amount requested.
6. ***MSG_PEEK***: This flag causes the receive operation to return data from the beginning of the receive queue without removing that data from the queue.  Thus, a
              subsequent receive call will return the same data.
7. ***MSG_WAITALL***:              This flag requests that the operation block until the  full  request  is
              satisfied.   However, the call may still return less data than requested
              if a signal is caught, an error or disconnect occurs, or the  next  data
              to be received is of a different type than that returned.  This flag has
              no effect for datagram sockets.
8. ***recvfrom***: If  src_addr  is  not NULL, and the underlying protocol provides the source ad‐
       dress of the message, that source address is placed in the buffer pointed to by
       src_addr.   In this case, addrlen is a value-result argument.  Before the call,
       it should be initialized to the size of the buffer  associated  with  src_addr.
       Upon  return,  addrlen  is updated to contain the actual size of the source ad‐
       dress.  The returned address is truncated if the buffer provided is too  small;
       in  this  case,  addrlen  will  return a value greater than was supplied to the
       call

### CONNECT

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
1. If the socket sockfd is of type SOCK_DGRAM, then addr is the address  to  which
       datagrams  are  sent  by default, and the only address from which datagrams are
       received.  If the socket is of type SOCK_STREAM or  SOCK_SEQPACKET,  this  call
       attempts to make a connection to the socket that is bound to the address speci‐
       fied by addr.
2. Generally, connection-based protocol sockets may  successfully  connect()  only
       once;  connectionless  protocol  sockets  may  use  connect() multiple times to
       change their association.  Connectionless sockets may dissolve the  association
       by connecting to an address with the sa_family member of sockaddr set to AF_UN‐
       SPEC
3. Generally, connection-based protocol sockets may  successfully  connect()  only
       once;  connectionless  protocol  sockets  may  use  connect() multiple times to
       change their association.  Connectionless sockets may dissolve the  association
       by connecting to an address with the sa_family member of sockaddr set to AF_UN‐
       SPEC

### LISTEN

```c
int listen(int sockfd, int backlog);
```

1. listen()  marks  the socket referred to by sockfd as a passive socket, that is,
       as a socket that will be used to accept incoming connection requests using  ac‐
       cept
2. The backlog argument defines the maximum length to which the queue  of  pending
       connections  for  sockfd  may  grow.   If a connection request arrives when the
       queue is full, the client may receive an error with an indication  of  ECONNRE‐
       FUSED  or,  if the underlying protocol supports retransmission, the request may
       be ignored so that a later reattempt at connection succeeds.

### ACCEPT
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

1.  