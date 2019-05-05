#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv)
{
	int port;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
	
	/* checking commandline parameter */
	if (argc < 2)
	{
		printf("usage: %s hostname port text\n", argv[0]);
		return -1;
	}
	
	if(strcmp(argv[1],"configure")){
        	fd=open("./.configure", O_RDWR|O_CREAT|O_EXCL,0777);
        	ssize_t wr1 = write(fd, argv[2], strlen(argv[2]));
        	ssize_t wr2 = write(fd, argv[3], strlen(argv[3]));
        	
    	}else if (){// if configure is not called
    		exit(1);
    	}

	/* obtain port number */
	if (sscanf(/*argv[2] port no */, "%d", &port) <= 0)
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
	host = gethostbyname(/*argv[1] host name*/);
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

	/* send text to server */
	if(strcmp(argv[1],"checkout")){
        	// send server name
    	}else if(strcmp(argv[1],"update")){
        	// send server name
    	}else if(strcmp(argv[1],"upgrade")){
        	// send server name
    	}else if(strcmp(argv[1],"commit")){
        	// send server name
    	}else if(strcmp(argv[1],"push")){
        	// send server name
    	}else if(strcmp(argv[1],"create")){
        	// send server name
    	}else if(strcmp(argv[1],"destroy")){
        	// send server name
    	}else if(strcmp(argv[1],"add")){
        	// send server name and file
    	}else if(strcmp(argv[1],"remove")){
        	// send server name and file
    	}else if(strcmp(argv[1],"currentversion")){
        	// send server name
    	}else if(strcmp(argv[1],"history")){
        	// send server name
    	}else if(strcmp(argv[1],"rollback")){
        	// send server name and version
    	}
    
    /*. how to send */
	len = strlen(argv[3]);
	write(sock, &len, sizeof(int));
	write(sock, argv[3], len);

	/* close socket */
	close(sock);

	return 0;
}
