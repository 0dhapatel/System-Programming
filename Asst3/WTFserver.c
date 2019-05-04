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
    clrscr(); 
    check = mkdir(dirname); 
    // check if directory is created or not 
    if (!check) 
        printf("Directory created\n"); 
    else { 
        printf("Unable to create directory\n"); 
        exit(1); 
    } 
    getch(); 
    system("dir/p"); 
    getch(); 
}

void deletedir(char* act)
{
    int cheack;
   char *dirname=act;
   clrscr(); 
   gets(dirname);
   system("dir/p");
   cheack = rmdir(dirname);
   if (!cheack){
      printf("Directory deleted\n");
   }
   else{   
    printf("Unable to remove directory\n"); 
    getch();
    exit(1);
   }
  getch();
}

void addfile(char* act, char* dir)
{
    if (chdir(dir) != 0) { //have to add / in string
        printf("Cannot open directory"); 
    }
    //have to check through manifest
    if(creat(act)!=0){
        printf("can not create file");
    }
}

void removefile(char* act, char* dir)
{
    if (chdir(dir) != 0) { //have to add / in string
        printf("Cannot open directory"); 
    }
    
    if (remove(act) == 0){ 
      printf("Deleted successfully"); 
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
    int sockfd, newsockfd, portno, in;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        error("Cannot open Socket");
    }
    
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno= atoi(argv[1]);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr_.s_addr=INADDR_ANY;
    serv_addr.port=htons(portno);
    
    if(blind(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr))<0){
        error("Binding Failed");
    }
    
    listen(sockfd,5);
    clilen=sizeof(cli_addr);
    
    newsockfd=accept(sockfd,(struct sockaddr*) &cli_addr,&clilen);
    
    if(newsockfd<0){
        error("Error on accept");
    }
    while(1){
        bzero(buffer,256);
        in=read(newsockfd,buffer,255);
        if(in<0){
            error("Error on reading");
        }
        printf("Client: %s\n", buffer);
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        
        in=write (newsockfd,buffer,strlen(buffer));
        if(in<0){
            error("Error on writing");
        }
        int i=strncmp(SIGINT,buffer,2);
        if(i==0){
            break;
        }
    }
    
    close(newsockfd);
    close(sockfd);
    return 0;
}
