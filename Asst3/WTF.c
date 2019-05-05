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

int main(int argc, char ** argv)
{
	int port;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
	char response[1024]; 
	const char* ip;
	int portno;
	
	/* checking commandline parameter */
	if (argc < 2)
	{
		printf("usage: %s hostname port text\n", argv[0]);
		return -1;
	}
	
	fd=open("./.configure", O_RDWR|O_CREAT|O_EXCL,0777);
	if(strcmp(argv[1],"configure")){
        	write(fd, argv[2], strlen(argv[2]));
        	write(fd, "\n", 1);
        	write(fd, argv[3], strlen(argv[3]));
        	exit(1);
    	}else if (fd>0){// if configure is not called and file is available
    		char* temp=(char*) malloc(sizeof(char)*100);
    		int size=read(fd,temp,100);
    		char* in=(char*) malloc(sizeof(char)*(size+1));
    		strcpy(in,temp);
    		free(temp);
    		ip=(strtok(in,"\n");
    		portno=atoi(strtok(NULL,"\n");
    		free(in);
    	}else{
    		exit(1);
    	}

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
		fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
		return -4;
	}
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}
	bzero(response,1240);

	/* send text to server */
	if(strcmp(argv[1],"checkout")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"update")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"upgrade")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"commit")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"push")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"create")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"destroy")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"add")){
        	// send server name and file
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		len = strlen(argv[3]);
		write(sock, &len, sizeof(int));
		write(sock, argv[3], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"remove")){
        	// send server name and file
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		len = strlen(argv[3]);
		write(sock, &len, sizeof(int));
		write(sock, argv[3], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"currentversion")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"history")){
        	// send server name
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}else if(strcmp(argv[1],"rollback")){
        	// send server name and version
        	len = strlen(argv[1]);
		write(sock, &len, sizeof(int));
		write(sock, argv[1], len);
		len = strlen(argv[2]);
		write(sock, &len, sizeof(int));
		write(sock, argv[2], len);
		len = strlen(argv[3]);
		write(sock, &len, sizeof(int));
		write(sock, argv[3], len);
		read(sock,response,1024);
		printf("%s\n",response);
    	}
    
	/* close socket */
	close(sock);

	return 0;
}
