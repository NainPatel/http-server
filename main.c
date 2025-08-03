
#include"parse.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<signal.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAX 1000
#define PORT 9090

int listend,clientd;
struct sockaddr_in servaddr,clientaddr;


int main(){
	listend=socket(AF_INET,SOCK_STREAM,0);
	if(listend<0){
		fprintf(stderr,"Error while making socket");
		exit(-1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(listend,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
		perror("bind");
		exit(-1);
	}
	if((listen(listend,5))<0){
		fprintf(stderr,"listening of client failed");
		exit(-1);
	}

	socklen_t client_len=sizeof(clientaddr);
	int test=1;

	//this tells kernal that we dont care about child exit status 
	//so it will just clean the child process after termination
	signal(SIGCHLD, SIG_IGN);

	while(test){
		clientd=accept(listend,(struct sockaddr*)&clientaddr,&client_len);
		if(clientd<0)
			continue;

		pid_t pid=fork();
		if(fork()==0){
			close(listend);
			processClientRequest(clientd);
			exit(0);
		}
		else if(pid>0){
			close(clientd);
		}
	}
	close(clientd);
	return 0;
}

