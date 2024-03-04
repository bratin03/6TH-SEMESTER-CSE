## Listen
- Syntax:
```cpp
int listen(int sockfd, int backlog);
```
- Headers
```cpp
#include <sys/socket.h>
#include <sys/types.h>
```
- Description
    - Listen marks the socket as passive socket, used to accept incoming connections. <font color="red">I tried to send a message from client without connecting when the server was listening. The client code exited with exit code 141. I tried sending to a server without connecting the server as well as the receiver failed. Basicallly if I call send or recv on a not connected socket, recv will throw an error stating "Transport endpoint is not connected" and send exits with exit code 141 </font>
    <font color="magenta">**You can not send or recv on a TCP socket without connecting.**</font>
    - The sockfd is a socket of type SOCK_STREAM or SOCK_SEQPACKET.<font color="red">I tried to use SOCK_DGRAM on server and it failed</font>
    - <font color="red">It worked without bind call but the client has no way to connect to the server</font>
    - About the backlog argument, it is the maximum length to which the queue of pending connections for sockfd may grow. If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED or, if the underlying protocol supports retransmission, the request may be ignored so that a later reattempt at connection succeeds.
- Return Value
    - On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
- Errors
1. EADDRINUSE
    - Another socket is already listening on the same port.
    - This can not be simulated as bind will fail if the port is already in use.
    - <font color="red">I called bind twice on same sockfd but didn't get any error.</font>
    ```cpp
    if (listen(sockfd, 5) == -1)
    {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1)
    {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    ```
    - <font color="red">I called listen twice on different sockfd with different port and it worked but the send recv behaviour was unexpected.</font>

- In case you dont have anything like accept after listen the code will execute and exit. It will not wait.

## Connect
- Syntax:
```cpp
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
- Headers
```cpp
#include <sys/socket.h> 
#include <sys/types.h>
```
- Description
    - Connects sockfd to addr. The addrlen argument specifies the size of addr.
    - <font color="red">If the socket sockfd is of type SOCK_DGRAM then the connect function will not establish a connection but only set the address of the peer to which the socket sockfd will send datagrams and the only datagrams from the specified address will be received.</font>
    - <font color="red">If the socket sockfd is of type SOCK_STREAM then the connect function will establish a connection to the peer specified by addr.</font>
    - <font color="red"> Generally connection-based protocol sockets may successfully connect() only once; connectionless protocol sockets may use connect() multiple times to change their association.</font>
    - <font color="red">I called connect twice on same sockfd and it failed with error "Transport endpoint is already connected" for TCP. For UDP I can call connect multiple times. After the success of connect call for UDP, I can send using the send call without specifying the address.</font>

- Return Value
    - On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
- Errors:
1. For nonblocking sockets, connect() returns -1 with the error EINPROGRESS if the connection cannot be established immediately. <font color="red">I tried to connect to a server which was not listening and it returned ECONNREFUSED. Also, I tried to connect to a server which was running and accepting connections and it still returned error although a message was received by the server.</font>

## Accept
- Syntax:
```cpp
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- Headers
```cpp
#include <sys/socket.h>
#include <sys/types.h>
```
- Description
    - The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET). It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket. The newly created socket is not in the listening state. The original socket sockfd is unaffected by this call.
    - The addr argument is a pointer to a sockaddr structure. This structure is filled in with the address of the peer socket, as known to the communications layer. The addrlen argument is a value-result argument: it should initially contain the size of the structure pointed to by addr, on return it will contain the actual size of the peer address.
    - The addrlen argument is a value-result argument: it should initially contain the size of the structure pointed to by addr, on return it will contain the actual size of the peer address. The returned address is truncated if the buffer provided is too small; in this case, addrlen will return a value greater than was supplied to the call.
    - <font color="red">I tried to call accept on a socket which was not listening and it returned error "Invalid argument".</font>
    - If no pending connections are present on the queue, and the socket is not marked as nonblocking, accept() blocks the caller until a connection is present. If the socket is marked nonblocking and no pending connections are present on the queue, accept() fails with the error EAGAIN or EWOULDBLOCK.

- Return Value
    - On success, these system calls return a nonnegative integer that is a file descriptor for the accepted socket. On error, -1 is returned, and errno is set appropriately.
- Errors
1. EAGAIN
    - The socket is marked nonblocking and no connections are present to be accepted.
2. EWOULDBLOCK
    - The socket is marked nonblocking and no connections are present to be accepted.
3. EBADF
    - The descriptor is invalid.

## FCNTL
- Syntax:
```cpp
int fcntl(int fd, int cmd, ... /* arg */ );
```
- Headers
```cpp
#include <fcntl.h>
```
- Usage
```cpp
int flags = fcntl(sockfd, F_GETFL, 0);
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
```

## SELECT
- Syntax:
```cpp
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
void FD_ZERO(fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_CLR(int fd, fd_set *set);
int FD_ISSET(int fd, fd_set *set);
```

- Headers
```cpp
#include <sys/select.h>
```
- Description
1. Monitor multiple file descriptors.
2. Select can monitor file descriptors numbers that are less than FD_SETSIZE which is 1024.
3. The file descriptors are watched to see if they are ready for reading, are ready for writing, or have an exceptional condition (a signal or an error condition).
