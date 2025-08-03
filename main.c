
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
	
	//init of SSL
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	//creating ssl context
	SSL_CTX *ctx=SSL_CTX_new(TLS_server_method());
	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);  // Forcing TLS 1.2+
	SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5:!RC4");  // Only strong ciphers
	SSL_CTX_use_certificate_file(ctx,"cert.pem",SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ctx,"Key.pem",SSL_FILETYPE_PEM);
	SSL *ssl=SSL_new(ctx);

	while(test){
		clientd=accept(listend,(struct sockaddr*)&clientaddr,&client_len);
		if(clientd<0)
			continue;

		pid_t pid=fork();
		if(fork()==0){
			close(listend);
			SSL_set_fd(ssl,clientd);
			SSL_accept(ssl);//TLS handshake
			processClientRequest(ssl);
			close(clientd);
			exit(0);
		}
		else if(pid>0){
			close(clientd);
		}
	}
	SSL_CTX_free(ctx);
	close(clientd);
	return 0;
}

