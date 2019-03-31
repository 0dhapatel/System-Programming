#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// inserts words in string array if not in already 
// adds in frequency array

int inserts(char *word, char** str, int*freq, int num)
{
    int i;
    for(i=0;i<num;i++){
        if(str[i]==NULL){
            str[i]=malloc((strlen(word)+1)*sizeof(char));
            strncpy(str[i],word,(strlen(word)+1));
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

int main () 
{
    char myStr[] = "hi my\n   my  name is \t  bob do u now it starts    with b.";
    // grabs from file and inserts it into the string array or adds in the frequency array
    char **str= (char**)malloc(50 * sizeof (char*));
    int *freq= malloc(50 * sizeof(int));
    char *word = strtok (myStr, " \t\n");
    int num=50,j;
    while (word != NULL) {
        j=inserts(word, str, freq, num);
        if(j){
            num=num+50;
            str=(char**)realloc(str,(num)*sizeof(char*));
            freq=(int*)realloc(freq,(num)*sizeof(int));
            str[num-50]=malloc((strlen(word)+1)*sizeof(char));
            strncpy(str[num-50],word,(strlen(word)+1));
            freq[num-50]=1;
        }
        printf ("%s\n", word);
        word = strtok (NULL, " \t\n");
    }
    putchar ('\n');
    printf("\n\n%s\t%d\n",str[1],freq[1]);
    int size = sizeof (freq)/ sizeof (int);
    printf("%d",size);
    return 0;
}
