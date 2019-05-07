#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include "WTF.h"


int main()
{
	int status = 0;
	char hostbuf[MAXLINE], *IPbuf; 
	char *port = "59";
    struct hostent *host_entry; 
    int hostname; 

    //find the IP address of the computer
    hostname = gethostname(hostbuf, sizeof(hostbuf));
    host_entry = gethostbyname(hostbuf);
    IPbuf = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    //begin the program with the first argument given, which should always "commit" 
    int i = 1;
    //create a new child process
	int pid = fork();
	if(pid < 0) 
	{ 
		write(STDERR, "Error from fork()\n", 18);
		return 0;
	}
	else if (pid == 0)
	{
		//child should exec to the client process with the IP address and a random port number
		printf("CHILD:\tpid: %i\tparent pid: %i\n", getpid(), getppid());
		int ret = execl("/WTF.c", argv[i], IPbuf, port); //using a random port number, 60	
		while( (ret < 0)&&(ret < 1024))
		{ //error, try again with a new port number
			port = (*port + 1);
			ret = execl("/WTF.c", argv[i], IPbuf, port);
		}

		//now that commit was called, we can test for errors
		//test the next args given at argv[2:5], with some arguments to use for testing
		++i;
		char *projectname = "newproj";
		int fp = open("a_file_1.txt", O_RDWR | O_CREAT, 0777);
		while(i < argc)
		{
			if( (strcmp(argv[i], "currentversion") == 0)||(strcmp(argv[i], "create") == 0)||(strcmp(argv[i], "destroy") == 0)||(strcmp(argv[i], "push") == 0)||(strcmp(argv[i], "commit") == 0)||(strcmp(argv[i], "upgrade") == 0)||(strcmp(argv[i], "update") == 0)||((strcmp(argv[i], "checkout") == 0))||(strcmp(argv[i], "history") == 0))
			{
				ret = execl("/WTF.c", argv[i], projectname);
				if(ret < 0) { errno(ret); }
			}
			else
			{
				ret = execl("/WTF.c", argv[i], projectname, fp);
				if(ret < 0) { errno(ret); }
			}
			++i;
		}

	} 
	else
	{
		printf("PARENT:\tpid: %i\tchild pid: %i\n", getpid(), pid);

		while((wait(&status)) > 0)
		{ //while we are waiting for the child to finish, we will run the server to listen for it's requests

			//execute the server program to establish a connection for the client
			execl("WTFserver.c", port);
			while( (ret < 0)&&(ret < 1024))
			{ //error, try again with a new port number
				port = (*port + 1);
				ret = execl("/WTFserver.c", port);
			}
		}

		printf("PARENT:\tChild has ended.\n");	

	}

	return 0; 
}
