
#include"parse.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<signal.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<pthread.h>

#define PORT 9090

int listend;
struct sockaddr_in servaddr,clientaddr;

void* Thread(void*);

int main(){
	int clientd;
	struct sockaddr_in clientaddr;

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
	
	//init of SSL
	OPENSSL_init_ssl(0,NULL);
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	//creating ssl context
	SSL_CTX *ctx=SSL_CTX_new(TLS_server_method());
	SSL_CTX_use_certificate_file(ctx,"cert.pem",SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ctx,"key.pem",SSL_FILETYPE_PEM);

	while(test){
		clientd=accept(listend,(struct sockaddr*)&clientaddr,&client_len);
		if(clientd<0)
			continue;

		SSL *ssl=SSL_new(ctx);
		
		if(!ssl){
			close(clientd);
			continue;
		}

		SSL_set_fd(ssl,clientd);
	//TLS handshake
		if(SSL_accept(ssl)<=0){
			ERR_print_errors_fp(stderr);
			SSL_free(ssl);
			close(clientd);
			continue;
		}

		pthread_t thread_id;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		if(pthread_create(&thread_id,&attr,Thread,ssl)){
				perror("pthread_create");
				SSL_free(ssl);
				close(clientd);
				return 1;
		}
	}
	SSL_CTX_free(ctx);
	return 0;
}



void* Thread(void* arg){
	SSL* Thread_clientd=(SSL*)arg;
	processClientRequest(Thread_clientd);
	return NULL;
}
