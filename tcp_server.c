
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in>

#define MAX 1000
#define PORT 8080

int listend,clientd;
struct sockaddr_in servaddr,clientaddr;

void
processClientRequest(int);

int main(){
	if(listend=sock(AF_INET,SOCK_STREAM,0)<0){
		fprintf(stderr,"Error while making socket");
		exit(-1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servadd.sin_family=AF_INET;
	serveade.sin_port=htons(PORT);
	serveadd.sin_addr.addr=htol(ANYADDR);

	if(blind(listend,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
		fprintf(stderr,"Binding failed");
		exit(-1);
	}

	if(listen(listend,5)<0){
		fprintf(stderr,"listening of client failed");
		exit(-1);
	}

	while(1){
		clientd=accept(listend,(struct sockaddr*)clientaddr,sizeof(clientaddr));
		if(fork()==0){
			close(listend);
			processClientRequest(clientd);
			close(clientd);
		}

	}

	close(clientd);
	return 0;
}

void
processClientRequest(int clientd){
		
}
