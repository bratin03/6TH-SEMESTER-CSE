#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAX_CLIENT 100
#define MAX_CONC 10
#define PORT 8180
#define BUFFER_SIZE 1024

int max(int a,int b)
{
	if(a>b)
		return a;
	return b;
}

int readUntilEndLine(int sockfd,char *buffer)
{
	char c;
	int i=0;
	for(;;)
	{
		if(recv(sockfd,&c,1,0)==0)
		{
			return 0;
		}
		if(i==BUFFER_SIZE)
		{
			printf("Buffer Overflow");
			exit(EXIT_FAILURE);
		}
		if(c=='\n')
		{
			buffer[i++]='\0';
			return i;
		}
		buffer[i++]=c;
	}
}

int main()
{
	int sockfd,newsockfd;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	int fdesc[MAX_CLIENT];
	for(int i=0;i<MAX_CLIENT;i++)
	{
		fdesc[i]=-1;
	}
	
	struct sockaddr_in serv_addr,cli_addr;
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(PORT);
	
	if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	
	printf("Server Running on Port: %d\n",PORT);
		
	if(listen(sockfd,MAX_CONC)<0)
	{
		perror("listen");
	}
	
	fd_set readfds;

	for(;;)
	{
		FD_ZERO(&readfds);
		int max_fd=sockfd;
		
		FD_SET(sockfd,&readfds);
		
		for(int i=0;i<MAX_CLIENT;i++)
		{
			if(fdesc[i]!=-1)
			{
				FD_SET(fdesc[i],&readfds);
				max_fd=max(fdesc[i],max_fd);
			}
		}
		
		struct timeval t;
		t.tv_sec=3;
		t.tv_usec=50;
		
		int activity;
		activity=select(max_fd+1,&readfds,NULL,NULL,&t);
		
		if(activity<0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
		
		if(activity==0)
		{
			printf("Timeout!\n");
			continue;
		}
		
		if(FD_ISSET(sockfd,&readfds))
		{
			socklen_t clilen;
			clilen=sizeof(cli_addr);
			newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
			if(newsockfd<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			for(int i=0;i<MAX_CLIENT;i++)
			{
				if(fdesc[i]==-1)
				{
					fdesc[i]=newsockfd;
					break;
				}
			}
		}
		
		for(int i=0;i<MAX_CLIENT;i++)
		{
			if(FD_ISSET(fdesc[i],&readfds))
			{
				char buffer[BUFFER_SIZE];
				int n=readUntilEndLine(fdesc[i],buffer);
				if(n==0)
				{
					close(fdesc[i]);
					fdesc[i]=-1;
					continue;
				}
				printf("Message From %d: %s\n",fdesc[i],buffer);
			}
		}
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
			
}
