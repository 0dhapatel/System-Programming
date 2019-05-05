#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void removefile(){ //removes from client manifest
}

void addfile(){ //adds from client manifest
}

int main(int argc, char ** argv)
{
	int port;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
	char response[1024]; 
	const char* ip;
	const char* portno;
	
	/* checking commandline parameter */
	if (argc < 2)
	{
		printf("usage: %s hostname port text\n", argv[0]);
		return -1;
	}
	
	int fd=open("./.configure",O_RDONLY,0777);
	if(strcmp(argv[1],"configure")==0){
		//int hi=creat("configure.txt",0777);
		fd=open("./.configure", O_WRONLY|O_CREAT,0777);
        	ssize_t w1=write(fd, argv[2], strlen(argv[2]));
        	ssize_t w2=write(fd, "\n", 1);
        	ssize_t w3=write(fd, argv[3], strlen(argv[3]));
        	printf("config\n");
        	close(fd);
        	//return 0;
    	}else if (fd<0){   // if configure is not called and file is not available
    		return 0;
    	}
	fd=open("./.configure", O_RDONLY|O_CREAT,0777);
    	char temp[100];
    	int size=read(fd,temp,100);
    	char* in=(char*) malloc(sizeof(char)*(size+1));
    	strcpy(in,temp);
    	//free(temp);
    	ip=(strtok(in,"\n"));
    	printf("%s\n",ip);
    	portno=strtok(NULL,"\n");
    	printf("%s\n",portno);
	/* obtain port number */
	if (sscanf(portno, "%d", &port) <= 0)
	{
		fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
		return -2;
	}

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname(ip);
	if (!host)
	{
		fprintf(stderr, "%s: error: unknown host %s\n", argv[0], ip);
		return -4;
	}
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}
	free(in);
	close(fd);
	
	bzero(response,1024);
	if(strcmp(argv[1],"sendfile")==0){
        	// send server file
        	sprintf(response,"%s:%s",argv[1],argv[2]);
    	}else if(argc==3){
        	// send server name
        	sprintf(response,"%s:%s",argv[1],argv[2]);
        	if(strcmp(argv[1], "create")){
        		
        	}else if(strcmp(argv[1], "destroy")){
        		
        	}
    	}else if(argc==4){
        	// send server name and file
        	if(strcmp(argv[1], "rollback"){
        		sprintf(response,"%s:%s:%s",argv[1],argv[2],argv[3]);
        	}else if(strcmp(argv[1],"add")){
        		//adds into manifest
        	}else if(strcmp(argv[1],"remove")){
        		//removes into manifest
        	} 
    	}else{
    		return 0;
    	}

	/* send text to server */
		 
		len = strlen(response);
		write(sock, &len, sizeof(int));
		write(sock, response, len);
		
	/* read server to client */
	

	/* close socket */
	close(sock);

	return 0;
}
