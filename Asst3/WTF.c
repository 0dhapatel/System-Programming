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
#include <openssl/sha.h>
#include <dirent.h>
#include <errno.h>

void sendFile(char *name,int sock){

	int nSize=strlen(name);
	write(sock,&nSize,sizeof(int));
	write(sock,name,nSize);

	int fSize;
	struct stat check;
	int fd=open(name, O_RDONLY);
	if(stat(name,&check)==0)
		fSize=check.st_size;
	char *file=(char*)malloc(fSize+1);
	read(fd,file,fSize);
	*(file+fSize)='\0';
	close(fd);

	write(sock,&fSize,sizeof(int));
	write(sock,file,fSize);
	

}

/*.turn file into to hascode  */
char *hashcode(char *string){

        size_t length = strlen(string);
	char *output=(char*)malloc(sizeof(40));
	strcpy(output,"");
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(string, length, hash);

        int i;
	char code[3];	
        for(i=0;i<SHA_DIGEST_LENGTH;i++){
               sprintf(code,"%02x",hash[i]);
		strcat(output,code);
        }
	return output;
}

/* add(a) or remove(r) file  */

void addOrRemoveFile (char *dirName, char *path, char command)
{

  int fSize;
  struct stat check;
  int fd = open (path, O_RDWR);

  if (stat (path, &check) == 0)
    fSize = check.st_size;
  char *file = (char *) malloc (sizeof (char) * fSize + 1);
  read (fd, file, fSize);
  *(file + fSize) = '\0';
  close (fd);

  chdir (dirName);
  int mSize;
  fd = open (".Manifest", O_RDONLY);
  if (stat (".Manifest", &check) == 0)
    mSize = check.st_size;
  char *man = (char *) malloc (sizeof (char) * mSize + 1);
  read (fd, man, mSize);
  *(man + mSize) = '\0';
  close (fd);

  int arraySize = 0;
  int i;
  for (i = 0; i < mSize - 1; i++)
    {

      if (man[i] == '\n')
	{
	  arraySize++;
	}

    }

  char *version[arraySize];
  char *fileName[arraySize - 1];
  char *Hashcontent[arraySize - 1];

  char *token = strtok (man, " \n");
  char *temp = (char *) malloc (strlen (token) + 1);
  strcpy (temp, token);
  version[0] = temp;
  token = strtok (NULL, " \n");
  int split = 0;
  int index = 0;
  while (token)
    {
      char *temp = (char *) malloc (strlen (token) + 1);
      strcpy (temp, token);

      if (split == 0)
	{
	  version[index + 1] = temp;
	  token = strtok (NULL, " \n");
	  split++;
	}
      else if (split == 1)
	{
	  fileName[index] = temp;
	  token = strtok (NULL, " \n");
	  split++;
	}
      else if (split == 2)
	{
	  Hashcontent[index] = temp;
	  token = strtok (NULL, " \n");
	  split = 0;
	  index++;
	}
    }

  if (command == 'a')
    {
      fd = open (".Manifest", O_CREAT | O_RDWR, 0400);
      write (fd, version[0], strlen (version[0]));
      write (fd, "\n", 1);
      int inManifest = 0;
      int k;

      for (k = 0; k < arraySize - 1; k++)
	{

	  write (fd, version[k + 1], strlen (version[k + 1]));
	  write (fd, " ", 1);

	  write (fd, fileName[k], strlen (fileName[k]));
	  write (fd, " ", 1);
	  if (strcmp (path, fileName[k]) == 0)
	    {
	      inManifest = 1;
	      write (fd, hash (file), 40);
	    }
	  else
	    {
	      write (fd, Hashcontent[k], strlen (Hashcontent[k]));
	    }
	  write (fd, "\n", 1);

	}

      if (inManifest == 0)
	{
	  write (fd, "0", 1);
	  write (fd, " ", 1);
	  write (fd, path, strlen (path));
	  write (fd, " ", 1);
	  write (fd, hash (file), 40);
	  write (fd, "\n", 1);
	}

      write (fd, "\n", 1);
      close (fd);
    }
  if (command == 'r')
    {
      remove (".Manifest");
      fd = open (".Manifest", O_CREAT | O_WRONLY, 0600);
      write (fd, version[0], strlen (version[0]));
      write (fd, "\n", 1);

      int k;
      printf ("%d\n", arraySize);
      for (k = 0; k < arraySize - 1; k++)
	{
	  if (strcmp (fileName[k], path) == 0)
	    {
	      continue;
	    }
	  printf ("%d ", k);
	  write (fd, version[k + 1], strlen (version[k + 1]));
	  write (fd, " ", 1);
	  write (fd, fileName[k], strlen (fileName[k]));
	  write (fd, " ", 1);
	  write (fd, Hashcontent[k], strlen (Hashcontent[k]));
	  write (fd, "\n", 1);

	}
      write (fd, "\n", 1);

      close (fd);

    }

  chdir ("..");

}

void checkout (char* direcn, int sock)
{
  // check if directory exists
  DIR * dir = opendir (direcn);
  if (dir)
    {
      //Directory exists.
      closedir (dir);
      printf("Project name already exist\n");
      exit(0);
    }
  else if (ENOENT == errno)
    {
      // Directory does not exist. Goes to server to grab project.
      char* response;
      sprintf(response,"%s:%s","checkout",direcn);
      int len=strlen(response);
      write(sock, &len, sizeof(int));
	    write(sock, response, len);
	    char* feedback;
	    char* tar;
	    sprintf(tar,"%s.tgz",direcn);
	    // reads tar file
	    char* buf;
	    read(sock, buf, 10);
      read(sock, feedback, atoi(buf));                                                              
      int fd=open(tar,O_WRONLY|O_CREAT,0777);
      int wr=write(fd,feedback,strlen(feedback));
      char* untar;
      sprintf("tar -xvf %s",tar);
      system(untar);
      close(fd);
    }
  else
    {
      // opendir() failed for some other reason.
      printf("Error occured using opendir()");
      exit(0);
    }
}

void deletedir(char* act)
{
   // check if directory is there or not
  
	
	DIR* dir = opendir("mydir");
	if (dir)
	{
    	/* Directory exists. */
    		closedir(dir);
		 char *dirname=act;
   		char* in;
   		sprintf(in,"rm -r %s",dirname);
   		system(in);
	}
	else
	{
   	// error
	}
   
}

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
    if (chdir(dirname) != 0) { //have to add / in string
        	printf("Cannot open directory"); 
    }
    creat(".Manifest",0777);
    
    
}

void commit(char * dirName){
	struct stat check;
	chdir(dirName);
	int mSize;
	int fd=open(".Manifest", O_RDONLY);
	if(stat(".Manifest",&check)==0)
        	mSize=check.st_size;
	char *man=(char*)malloc(sizeof(char)*mSize+1);
	read(fd,man,mSize);
	*(man+mSize)='\0';
	close(fd);
	chdir("..");

	int arraySize=0;
	int i;
	for(i=0;i<mSize-1;i++){

        	if(man[i]=='\n'){
                	arraySize++;
        	}

	}

	char *version[arraySize];
	char *fileName[arraySize-1];
	char *Hashcontent[arraySize-1];

	char* token = strtok(man, " \n");
	char *temp=(char*)malloc(strlen(token)+1);
	strcpy(temp,token);
	version[0]=temp;
	token=strtok(NULL, " \n");
	int split=0;
	int index=0;
	while (token) {
        	char *temp=(char*)malloc(strlen(token)+1);
        	strcpy(temp,token);

        	if(split==0){
                	version[index+1]=temp;
                	token = strtok(NULL, " \n");
                	split++;
        	}else if(split==1){
                	fileName[index]=temp;
                	token = strtok(NULL, " \n");
                	split++;
        	}else if(split==2){
                	Hashcontent[index]=temp;
                	token = strtok(NULL, " \n");
                	split=0;
                	index++;
        	}
	}
	chdir(dirName);
	remove(".Commit");
	fd=open(".Commit", O_CREAT | O_WRONLY,0600);
	
	int versionNumber=atoi(version[0]);
	versionNumber++;
	
	char newVersion[4];
	strcpy(newVersion,"");
	sprintf(newVersion,"%d",versionNumber);

	write(fd, newVersion,strlen(newVersion));
	write(fd, "\n",1);
	int k;

	for(k=0;k<arraySize-1;k++){
	
		versionNumber=atoi(version[k+1]);
		versionNumber++;	
		strcpy(newVersion,"");
		sprintf(newVersion,"%d",versionNumber);
	
		int fSize;
		char path[50];
		strcpy(path,fileName[k]);
		char name[20];
		strcpy(name,&path[strlen(dirName)+1]);
	
		int fd2=open(name,O_RDWR);

		if(stat(name,&check)==0)
        		fSize=check.st_size;
		char *file=(char*)malloc(sizeof(char)*fSize+1);
		read(fd2,file,fSize);
		*(file+fSize)='\0';
		close(fd2);

                char *hashCode=(char*)malloc(sizeof(char)*41);
                hashCode=hash(file);
        	write(fd,newVersion,strlen(newVersion));
        	write(fd," ",1);

        	write(fd,fileName[k],strlen(fileName[k]));
        	write(fd," ",1);

        	write(fd,hash(file),40);
        	write(fd,"\n",1);
		
	}

	write(fd,"\n",1);
	close(fd);
	chdir("..");
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
        		createdir(argv[2]);
        	}else if(strcmp(argv[1], "destroy")){
        		deletedir(argv[2]);
        	}else if(strcmp(argv[1], "commit")){
			commit(argv[2]);
        		return 0;
        	}else if(strcmp(argv[1], "checkout")){
			checkout(argv[2],sock);
        		return 0;
        	}
    	}else if(argc==4){
        	// send server name and file
        	if(strcmp(argv[1], "rollback"){
        		sprintf(response,"%s:%s:%s",argv[1],argv[2],argv[3]);
        	}else if(strcmp(argv[1],"add")){
        		//adds into manifest
        		addOrRemoveFile (argv[2], argv[3], 'a');
			return 0;
        	}else if(strcmp(argv[1],"remove")){
        		//removes into manifest
        		addOrRemoveFile (argv[2], argv[3], 'r');
			return 0;
        	} 
    	}else{
    		return 0;
    	}

	/* send text to server */
		 
		len = strlen(response);
		write(sock, &len, sizeof(int));
		write(sock, response, len);
		
	/* read server to client */
	// based on what argv[1] is

	/* close socket */
	close(sock);

	return 0;
}
