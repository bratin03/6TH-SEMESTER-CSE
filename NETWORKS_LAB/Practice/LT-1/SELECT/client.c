#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<sys/types.h>

#define IP "127.0.0.1"
#define PORT 8180
#define BUFFER_SIZE 1024


int main(int argc,char *argv[])
{	
	pid_t id=getpid();
	srand(time(NULL)+id);
	
	int sockfd;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE];
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(PORT);
	if(inet_pton(AF_INET,IP,&serv_addr.sin_addr)<=0)
	{
		perror("inet_pton");
		exit(EXIT_FAILURE);
	}
	
	if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		perror("connect");
	}
	
	int num=5+rand()%10;
	printf("I will send %d times\n",num);
	
	for(int i=0;i<num;i++)
	{
		int time_s=1+rand()%5;
		sleep(time_s);
		sprintf(buffer,"My %d message\n",i+1);
		send(sockfd,buffer,strlen(buffer),0);
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
	
}
