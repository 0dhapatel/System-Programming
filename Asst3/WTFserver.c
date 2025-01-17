#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
//#include "WTF.h"

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

/* note to self check if directory is being changed to server_repo properly */

int getver(char *name)
{
 	struct dirent *drct;
    DIR *direc = opendir(name);
	int latest = 0;
	if (direc == NULL)
  {
	printf("project does not exist\n");
	return -1;
  }
	while ((drct = readdir(direc)) != NULL)
        {
  		if (latest < atoi(drct->d_name))
		{
			latest = atoi(drct->d_name);
		}
  	}
	closedir(direc);
	return latest;
}


void currentVersion(char *dirname)
{
	char dire[100];
	snprintf(dire, sizeof(dire), "./.%s/%s", "server_repo", dirname);
        DIR *direc = opendir(dire);
	if (direc == NULL)
        {
		printf("project does not exist\n");
		return;
        }
	printf("%s\n", dirname);
	int num = getver(dire);
	char vernum[50];
	sprintf(vernum, "%d", num);
	char manifest[150];
	snprintf(manifest, sizeof(manifest), "%s/%s/%s", dire, vernum, ".Manifest");

	int fd = open(manifest, O_RDONLY);
	char currchar;
	int pos = 2;
	off_t end = lseek(fd, 0, SEEK_END);
	lseek(fd, pos, SEEK_SET);

	while (pos < end)
	{
		// print version number
		read(fd, &currchar, 1);
		while (currchar != ' ' )
		{
			printf("%c", currchar);
			pos++;
			lseek(fd, pos, SEEK_SET);
			read(fd, &currchar, 1);
		}
		printf(" ");
		pos+=2;
		lseek(fd, pos, SEEK_SET);
		int last = pos;
		int wc = 0;
		// parse and print file name
		while (1)
		{
			read(fd, &currchar, 1);
			if (currchar == ' ')
			{
				lseek(fd, last + 1, SEEK_SET);
				char *filen = malloc(sizeof(char) * wc+1);
				read(fd, filen, wc);
				printf("%s\n", filen);
				// skip over hashed file content
				while (currchar != '\n')
				{
					pos++;
					lseek(fd, pos, SEEK_SET);
					read(fd, &currchar, 1);
				}
				break;
			}
			else if (currchar == '/')
			{
				last = pos;
				wc = 0;
			}
			pos++;
			wc++;
		}
		pos++;
		lseek(fd, pos, SEEK_SET);
	}
	closedir(direc);
}

void rollback(int ver){

        struct stat check;
        int vSize;
        int fd=open(".Version", O_RDONLY);
        if(stat(".Version",&check)==0)
                vSize=check.st_size;
        char *vers=(char*)malloc(sizeof(char)*vSize+1);
        read(fd,vers,vSize);
        *(vers+vSize)='\0';
        close(fd);

        char *token=strtok(vers," \n");
        int old=atoi(token);

	char delete[15];

	char vertype[4];
	strcpy(vertype,"");

	int i;
	for(i=old;i>ver;i--){
		strcpy(delete,"rm -r version");
		sprintf(vertype,"%d",i);
		strcat(delete,vertype);
		system(delete);
	
	}


	sprintf(vertype,"%d",ver);

	
        remove(".Version");

        int fd2=open(".Version", O_CREAT | O_WRONLY, 0777);
        write(fd2,vertype,strlen(vertype));
        write(fd2,"\n",1);
        close(fd2);


        FILE *history=fopen(".History","a");
        fprintf(history,"Rollback:%s\n",vertype);
        fclose(history);
        chdir("..");
        chdir("..");

}


/*.    create and remove files and directories   .*/
// have to pass strings to client

void createdir (char* act)
{
    int dirc;
    char* dirname = "./.server_repo/"; 
	strcat(dirname,act);
    //clrscr(); 
    int check = mkdir(dirname,0777); 
	
    // check if directory is created or not 
	
    if (!check) 
        printf("Directory created\n"); 
    else { 
        printf("Unable to create directory\n"); 
        //exit(1); 
    }  
    if (chdir(dirname) != 0) { //have to add / in string
        	printf("Cannot open directory"); 
    }
	int fd=open(".Version",O_CREAT | O_WRONLY, 0600);
        	if(fd==-1){
                	printf("failed to make version");
        	}
        	write(fd,"0",1);
        	write(fd,"\n",1);
	        close(fd);

                int fd2=open(".History",O_CREAT | O_WRONLY, 0600);
                if(fd==-1){
                        printf("failed to make History");
                }
                write(fd2,"create: 0",9);
                write(fd2,"\n",1);
                close(fd2);
	chdir("..");
		chdir("..");
    
    
}

void deletedir(char* act) //have to lock
{
   // check if directory is there or not
   chdir(".server_repo");
   char *dirname=act;
   char* in;
   sprintf(in,"rm -r %s",dirname);
   system(in);
   chdir("..");
   
}

void checkout (char* direcn, int sock)
{
    DIR * dir = opendir (direcn);
  if (dir)
    {
      //Directory exists. Send over to the client
      // gets the most resent version of project
      int ver=getver(direcn);
      chdir(".server_repo");
      chdir(direcn);
      char* tar;
      sprintf(tar,"tar cfz %s.tgz %d",direcn, ver);
      system(tar);
      char* act;
      sprintf(act,"%s.tgz");
      int fd=open(act,O_CREAT|O_RDONLY,0777);
      
      int fSize;
	struct stat check;
	if(stat(act,&check)==0)
		fSize=check.st_size;
	char *file;
	read(fd,file,fSize);
	close(fd);
	write(sock,&fSize,sizeof(int));
	write(sock,file,fSize);
      char* untar;
      sprintf(untar,"tar xfz %s.tgz %d",direcn, ver);
      system(untar);
      chdir("..");
      chdir("..");
    }
  else if (ENOENT == errno)
    {
      // Directory does not exist. Goes to server to grab project.
      write(sock,"No project in server",20);
    }
  else
    {
      // opendir() failed for some other reason.
      write(sock,"Server Error",12);
      exit(0);
    }
}

/**/

void history(char* direc, int sock){
	struct stat st;
	char *act;
	sprintf(act,"./.server_por/%s/.History",direc);
	stat(act, &st);
	int size = st.st_size;
	//printf("%d\n",size);
	char *in;
	sprintf(in,"%d",size);
	write(sock,in,20);
	int fd= open(act,O_RDONLY,0777);
	char* buf;
	int re=read(fd,buf,size);
	write(sock,buf,size);
}

void * process(void * ptr) // takes in from client in order to do as commanded
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
		/*printf("%d.%d.%d.%d: %s\n",
			(int)((addr      ) & 0xff),
			(int)((addr >>  8) & 0xff),
			(int)((addr >> 16) & 0xff),
			(int)((addr >> 24) & 0xff),
			buffer);*/
			
		char temp[1024];
		strcpy(temp,buffer);
		free(buffer);
		
		//printf("temp: %s\n",temp);
		char* command = strtok(temp,":");
		//printf("%s\n",command);
		
		//printf("command: %s\n",command);
		//printf("%d\n",strlen(command));
		
		int le1, le2, le3, ver;
		char* direcn; 
		char* filen;
		char* serv="./.server_repo/";
		
		/*printf("%d\n",le);
		printf("%s\n",temp);*/
		
		
		if(strcmp(command,"checkout")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			checkout(direcn,conn->sock);
    		}else if(strcmp(command,"update")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
        		chdir("./.server_repo");
        		chdir(direcn);
        		int fSize;
	struct stat check;
	int fd=open(".Manifest", O_RDONLY);
	if(stat(".Manifest",&check)==0)
		fSize=check.st_size;
	char *file=(char*)malloc(fSize+1);
	read(fd,file,fSize);
	*(file+fSize)='\0';
	close(fd);

	write(conn->sock,&fSize,sizeof(int));
	write(conn->sock,file,fSize);
        		chdir("..");
        		chdir("..");
        		
    		}else if(strcmp(command,"upgrade")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
        		
        		
        		int length;
		read(conn->sock,&length,sizeof(int));
		char *path=(char*)malloc((length+1)*sizeof(char));
		read(conn->sock,path,length);
			
		chdir(".server_repo");
		struct stat check;
		int size;
		int fd=open(path, O_RDONLY,0600);
		if(stat(path,&check)==0)
			size=check.st_size;
		char *file=(char*)malloc(sizeof(char)*size+1);
		read(fd,file,size);
		*(file+size)='\0';
		close(fd);	

		write(conn->sock,&size,sizeof(int));
		write(conn->sock,file,size);	

		chdir("..");
        		
        		
        		
    		}else if(strcmp(command,"push")==0){ // everytime creates new version
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
        		struct stat check;
	int vSize;
        		
        		DIR * dir = opendir (direcn);
  if (dir)
    {
      //Directory exists.
      closedir (dir);
      chdir("./.server");
      chdir(direcn);
      int ver=getver(direcn)+1;
      char* feedback;
	    char* tar;
	    sprintf(tar,"%s.tgz",direcn);
	    // reads tar file
	    char* buf;
	    read(conn->sock, buf, 10);
      read(conn->sock, feedback, atoi(buf));                                                              
      int fd=open(tar,O_WRONLY|O_CREAT,0777);
      int wr=write(fd,feedback,strlen(feedback));
      char* untar;
      sprintf("tar -xvf %s.tgz %d",tar,ver);
      system(untar);
      close(fd);
      
       fd=open(".Version", O_RDONLY);
	if(stat(".Version",&check)==0)
		 vSize=check.st_size;
	char *vers=(char*)malloc(sizeof(char)*vSize+1);
	read(fd,vers,vSize);
	*(vers+vSize)='\0';
	close(fd);

	
      char *newVersion;
	sprintf(newVersion,"%d",ver);

	remove(".Version");

	int fd2=open(".Version", O_CREAT | O_WRONLY, 0600);
	write(fd2,newVersion,strlen(newVersion));
	write(fd2,"\n",1);
	close(fd2);

	FILE *history=fopen(".History","a");
	fprintf(history,"push:%s\n",newVersion);
	fclose(history);

      chdir("..");
      chdir("..");
    }
  else if (ENOENT == errno)
    {
      // Directory does not exist. Goes to client to grab project.
      createdir(direcn);
      chdir("./.server");
      chdir(direcn);
	    char* feedback;
	    char* tar;
	    sprintf(tar,"%s.tgz",direcn);
	    // reads tar file
	    char* buf;
	    read(conn->sock, buf, 10);
      read(conn->sock, feedback, atoi(buf));                                                              
      int fd=open(tar,O_WRONLY|O_CREAT,0777);
      int wr=write(fd,feedback,strlen(feedback));
      char* untar;
      sprintf("tar -xvf %s.tgz 1",tar);
      system(untar);
      close(fd);
      fd=open(".Version", O_RDONLY);
	if(stat(".Version",&check)==0)
		vSize=check.st_size;
	char *vers=(char*)malloc(sizeof(char)*vSize+1);
	read(fd,vers,vSize);
	*(vers+vSize)='\0';
	close(fd);

	
      char *newVersion;
	sprintf(newVersion,"%d",ver);

	remove(".Version");

	int fd2=open(".Version", O_CREAT | O_WRONLY, 0600);
	write(fd2,newVersion,strlen(newVersion));
	write(fd2,"\n",1);
	close(fd2);

	FILE *history=fopen(".History","a");
	fprintf(history,"push:%s\n",newVersion);
	fclose(history);
	chdir("..");
      chdir("..");
    }
  else
    {
      // opendir() failed for some other reason.
      printf("Error occured using opendir()");
      exit(0);
    }
        		
        		
        		
    		}else if(strcmp(command,"create")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			createdir(direcn);
    		}else if(strcmp(command,"destroy")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			deletedir(direcn);
    		}else if(strcmp(command,"currentversion")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			currentVersion(direcn);
    		}else if(strcmp(command,"history")==0){
        	// send  name to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			history(direcn,conn->sock);
    		}else if(strcmp(command,"rollback")==0){
        	// send  name and version to method
        		direcn=strtok(NULL,":");
        		//printf("direc: %s\n", direcn);
			strcat(serv,direcn);
			chdir(serv);
        		ver=atoi(strtok(NULL,":"));
        		//printf("verzion: %s\n", filen);
			rollback(ver);
			
    		}else{
    			printf("does not go through\n");
    		}
		
		
		
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}

int main(int argc, char ** argv)
{
	
   	if (mkdir(".server_repo",0777) !=0) {
        	//printf("Directory not created\n"); 
    	}
    	
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	pthread_t thread;

	/* check for command line arguments */
	if (argc < 2)
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
	if (listen(sock, 1000) < 0)
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
		// try to get more words passed through
		// control c should close sock
	}
	
	return 0;
}
