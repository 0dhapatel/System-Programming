#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <unistd.h>

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

void * process(void * ptr)
{
	char * buffer;
	int len;
	connection_t * conn;
	long addr = 0;

	if (!ptr) pthread_exit(0); 
	conn = (connection_t *)ptr;

	/* read length of message */
	read(conn->sock, &len, sizeof(int));
	if (len > 0)
	{
		addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
		buffer = (char *)malloc((len+1)*sizeof(char));
		buffer[len] = 0;

		/* read message */
		read(conn->sock, buffer, len);

		/* print message */
		printf("%d.%d.%d.%d: %s\n",
			(int)((addr      ) & 0xff),
			(int)((addr >>  8) & 0xff),
			(int)((addr >> 16) & 0xff),
			(int)((addr >> 24) & 0xff),
			buffer);
		free(buffer);
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}
/*.    create and remove files and directories   .*/
// have to pass strings to client

void createdir (char* act)
{
    int dirc;
    char* dirname = act; 
    //clrscr(); 
    int check = mkdir(dirname,0777); 
    // check if directory is created or not 
    if (!check) 
        printf("Directory created\n"); 
    else { 
        printf("Unable to create directory\n"); 
        //exit(1); 
    }  
    //getch(); 
    //system("dir/p"); 
    //getch(); 
    
}

void addfile( char* dir, char* act)
{
    if (chdir(dir) != 0) { 
        printf("Cannot open directory"); 
    }
    //have to check through manifest
    int fp=open("Manifest.txt", O_RDWR | O_CREAT, 0777);
    while(){// check if file is created or not
        
    }
    //read();
    close(fp);
    if(creat(act,0777)<0){
        printf("Error Creating file");
    }
}

void deletedir(char* act)
{
    int cheack;
   char *dirname=act;
   cheack = rmdir(dirname);
   if (!cheack){
      printf("Directory deleted\n");
   }
   else{   
    printf("Unable to remove directory\n"); 
    exit(1);
   }
}

void removefile( char* dir, char* act) //removes when nothing in the folder
{
    if (chdir(dir) != 0) { //have to add / in string
        printf("Cannot open directory"); 
    }
    if (remove(act) == 0){ 
      printf("Deleted successfully");
      // go to manifest and state how the file is removed
   }else{
      printf("Unable to delete the file");
   }
    
}


/**/

int main(int argc, char ** argv)
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	pthread_t thread;

	/* check for command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return -1;
	}

	/* obtain port number */
	if (sscanf(argv[1], "%d", &port) <= 0)
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

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		return -4;
	}

	/* listen on port */
	if (listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);
	
	while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}
	
	return 0;
}
