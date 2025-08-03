#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

void
processClientRequest(int clientd){
	char req_header[1024];
	char resp_header[521];
	char method[8];
	char path[1000];
	char fullpath[1024];
	//reads the request header
	read(clientd,req_header,sizeof(req_header));

	sscanf(req_header,"%s %s",method,path);
	
	//to make our index page defult
	if(strcmp(path,"/")==0){
		strcpy(path,"/index.html");
	}

	snprintf(fullpath,sizeof(fullpath),"html%s",path);

	int fd=open(fullpath,O_RDONLY);
	//go to end of file(For size)
	off_t size=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	snprintf(resp_header,sizeof(resp_header),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %ld\r\n"
		"\r\n",size);
	write(clientd,resp_header,strlen(resp_header));
	char buff[1024];
	int n;
	while((n=read(fd,buff,sizeof(buff)))>0){
		write(clientd,buff,n);
	}

	close(clientd);
}


