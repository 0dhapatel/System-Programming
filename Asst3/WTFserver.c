#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char* msg)
{
    perror(msg);
    exit(1);
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

int main(int argc, char**argv)
{
    if(argc<2){
        fprintf(stderr, "Port No not provideed\n");
        exit(1);
    }
    
    
    int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
     return 0; 
    
    close(newsockfd);
    close(sockfd);
    return 0;
}
