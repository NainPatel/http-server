#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<openssl/ssl.h>
#include<openssl/err.h>

void
processClientRequest(SSL* ssl){
	char req_header[1024];
	char resp_header[521];
	char method[8];
	char path[1000];
	char fullpath[1024];

	memset(req_header,0,sizeof(req_header));
	//reads the request header
	int bytes_read = SSL_read(ssl, req_header, sizeof(req_header) - 1);
	if (bytes_read <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_shutdown(ssl);
		SSL_free(ssl);
		return;
	}
	req_header[bytes_read] = '\0';

	printf("Request:\n%s\n", req_header);

	sscanf(req_header,"%s %s",method,path);
	
	//to make our index page defult
	if(strcmp(path,"/")==0){
		strcpy(path,"/index.html");
	}

	snprintf(fullpath,sizeof(fullpath),"html%s",path);

	int file=open(fullpath,O_RDONLY);
	if(file<0){
		fprintf(stderr, "File not found: %s\n", fullpath);
	}
	//go to end of file(For size)
	off_t size=lseek(file,0,SEEK_END);
	lseek(file,0,SEEK_SET);
	
	snprintf(resp_header,sizeof(resp_header),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %ld\r\n"
		"\r\n",size);

	printf("Response:\n%s\n", resp_header);

	SSL_write(ssl,resp_header,strlen(resp_header));

	char buff[1024];
	int n;
	while((n=read(file,buff,sizeof(buff)))>0){
		SSL_write(ssl,buff,n);
	}


	close(file);
	SSL_shutdown(ssl);
	SSL_free(ssl);
}

