// UDP SOCKETS NOTE
/*
1. memset(&server, 0, sizeof(server)) - clear the server struct. Similar for client.
2. inet_aton("127.0.0.1", &server.sin_addr) - convert IP address to binary form. 0 returned on error
3. sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server, sizeof(server)) - send data to server
4. recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&server, &len) - receive data from server 
Last 2 fields can be set to 0 actually if you don't need them. 
5. sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&client, sizeof(client)) - send data to client
6. recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&client, &len) - receive data from client
//*************************************************************************
Remeber the 0 in both sendto and recvfrom is the flags. It is set to 0 for normal operation.
//*************************************************************************
*/