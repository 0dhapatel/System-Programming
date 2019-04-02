#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 

// inserts words in string array if not in already 
// adds in frequency array

int inserts(char *word, char** str, int*freq, int num)
{
    int i;
    //printf("%d\n",num);
    //printf("'%s'\n", word);
    //printf("%d\n",strlen(word));
    for(i=0;i<num;i++){
        if(str[i]==NULL){
            str[i]=malloc((strlen(word)+1)*sizeof(char));
            strncpy(str[i],word,(strlen(word)));
            freq[i]=1;
            return 1;
        }
        if(strcmp(word,str[i])==0){
            freq[i]=freq[i]+1; 
            return 1;
        }
    }
    return 0;
}

int tokenizer(char** str, int*freq, int num, char* filename)
{
	struct stat check;
    // grabs from file and inserts it into the string array or adds in the frequency array
	int file=open(filename, O_RDONLY);  ////filename has to be added given
    //printf("%d\n",check.st_size);
    char* myStr;
    if (stat("t.txt",&check)==0){
    	myStr=(char*)malloc((check.st_size)*sizeof(char));
    	//printf("%d\n",check.st_size);
    	read(file,myStr,check.st_size);
    	//printf("%d\n",check.st_size);
    }
    else{
    	printf("error\n");
    }
    close(file);
    //printf("%s\n",myStr);
    char *fill=myStr;
    //int s=check.st_size;
    //printf("%d\n\n",check.st_size);
	int j=0;
	char *ptr;// = strtok(myStr, del);
	while(ptr=strtok_r(fill," \t\n",&fill)){
		//printf("ptr=%d\n", strlen(ptr));
		printf("ptr=%s\n\n", ptr);
		j=inserts(ptr, str, freq, num);
        	if(j==0){
           		num=num+100;
            	str=(char**)realloc(str,(num)*sizeof(char*));
            	freq=(int*)realloc(freq,(num)*sizeof(int));
            	str[num-100]=malloc((strlen(ptr)+1)*sizeof(char));
            	strncpy(str[num-100],ptr,(strlen(ptr)+1));
            	freq[num-100]=1;
    		}
      }
      return num;
}
int main(int argc, char** argv) 
{
    char **str= (char**)malloc(100 * sizeof (char*));
    int *freq= malloc(100 * sizeof(int));
    int num=100;
	tokenizer(str,freq,num,/*filename*/);
	return 0
}
