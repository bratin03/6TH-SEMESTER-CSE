#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8181

int main()
{
	int sockfd;
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("sockfd");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(PORT);
	
	if(inet_aton("127.0.0.1",&serv_addr.sin_addr)==0)
	{
		printf("Error in ip-conversion\n");
	}
	
	char buffer[BUFFER_SIZE];
	strcpy(buffer,"WOW");
	sendto(sockfd,(const char*)buffer,strlen(buffer),0,(const struct sockaddr*)&serv_addr,sizeof(serv_addr));
	printf("Message Sent: %s\n",buffer);
	int n = recvfrom(sockfd,(char*)buffer,BUFFER_SIZE-1,0,0,0);
	buffer[n]='\0';
	printf("Message Received: %s\n",buffer);
	close(sockfd);
	exit(EXIT_SUCCESS);
}
