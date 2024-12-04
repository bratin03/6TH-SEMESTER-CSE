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
	
	struct sockaddr_in serv_addr,cli_addr;
	memset(&serv_addr,0,sizeof(serv_addr));
	memset(&cli_addr,0,sizeof(cli_addr));	
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(8181);
	
	if(bind(sockfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	printf("Server running on port: %d\n",PORT);
	
	while(1)
	{
		socklen_t len=sizeof(cli_addr);
		char buffer[BUFFER_SIZE];
		int n=recvfrom(sockfd,(char*)buffer,BUFFER_SIZE-1,0,(struct sockaddr *)&cli_addr,&len);
		char *ip=inet_ntoa(cli_addr.sin_addr);
		int port_num=ntohs(cli_addr.sin_port);
		printf("ip: %s port: %d\n",ip,port_num);
		buffer[n]='\0';
		printf("Message Received: %s\n",buffer);
		sendto(sockfd,(char*)buffer,strlen(buffer),0,(struct sockaddr*)&cli_addr,sizeof(cli_addr));				
		printf("Message Sent: %s\n",buffer);
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
}
