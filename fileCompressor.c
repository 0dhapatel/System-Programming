#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "filecompressor.h"
#include "huffmannode.c"


/***************************************************************************\
*                                FUNCTIONS                                  *
\***************************************************************************/
void err(int type)
{
	if(type == 0) { write(STDERR, "Cannot perform operation without a .hcz file argument\n", 54); }
	else if(type == 1) { write(STDERR, "Error in flags passed to fileCompressor\n", 40); }
	else if(type == 2) { write(STDERR, "Invalid argument input.\n", 24); }
	else if(type == 3) { write(STDERR, "Must input the flags before the arguments, or too many arguments given.\n", 73); }
	else if(type == 4) { write(STDERR, "Must include the file or directory argument.\n", 46); }
	else if(type == 5) { write(STDERR, "Missing information needed to use fileCompressor.\n", 50); }
	else if(type == 6) { write(STDERR, "Cannot perform the operation without recursion.\n", 48); }
	else if(type == 7) { write(STDERR, "Error opening file\n", 20); }
	else if(type == 8) { write(STDERR, "Error compressing the files\n", 28); }
	else if(type == 9) { write(STDERR, "Error decompressing the files\n", 30); }
}
char* updatePathSize(char *new, char *pathname)
{//update the size of the char *pathname to avoid undefined behavior/segfault
	//strlen gives the amount of used array (not the total size!!)
	int newSize = strlen(new); //amount of bytes we need
	int curSize = strlen(pathname); //amount of bytes we are using in pathname
	int totalSize = (curSize + newSize); //total amount we need to fit in the new pathname

	if(newSize >= PATH_SIZE)
	{//the new name requires more than one path size
		int mult = newSize/PATH_SIZE;
		++mult; //this is the number of pathsizes required to fit the new filename

//////////TODO: test realloc!!??

		pathname = realloc(pathname, curSize + (PATH_SIZE*mult)); //the current amount plus the amount needed to add the file
	}
	else if((totalSize%PATH_SIZE) == 0)
	{//there is no remaining bytes in the pathname for the next allocation, so expand the size
		pathname = realloc(pathname, (totalSize + PATH_SIZE));
	}
	return pathname;
}
char* updatePathname(char *pathname, char *name)
{//add a new directory to the pathname
	//update the path size so we don't have memory issues
	pathname = updatePathSize(name, pathname);
	//check the last value of pathname for correct format
	int pathSize = strlen(pathname);
	if(pathname[pathSize-1] != '/') { strncat(pathname, "/", 1); }
	//add the new filename to the end of pathname, then add a '/' for format
	strncat(pathname, name, strlen(name));
	strncat(pathname, "/", 1);
	return pathname; //return the updated pathname
}
int checkHCZ(char *filename)
{//determine if the file is a .hcz
	int length = strlen(filename);
	if(length < 5) { return ERR; }
	if( (filename[length-1] == 'z')&&(filename[length-2] == 'c')&&(filename[length-3] == 'h')&&(filename[length-4] == '.') ) { return 1; }
	return ERR;
}
struct flags buildMode(int numArgs, char **argv)
{//examine the arguments to determine the mode of operation and check for errors
	//initialize the flag struct with FIRST to indicate initialization
	struct flags mode; 
	mode.operation = FIRST;
	mode.recursive = FIRST;
	int i = 1;

	do {//loop through argv and check the elements to determine initalize mode
		if(mode.operation == ERR) { break; }
		if(strcmp(argv[i],"-R") == 0)
		{ 
			//-R is specified more than once
			if(mode.recursive == R)
			{ 
				err(1); 
				mode.operation = ERR;
				break;
			}
			mode.recursive = R; 
		}
		else if(strcmp(argv[i],"-b") == 0)
		{ //if it is the first time adding a flag, we can just add the flag; otherwise add the combination
			if(i == (numArgs-1))
			{//at the end of the flags w/o an argument
				err(5);
				mode.operation = ERR;
				break;
			}
			if(mode.operation == FIRST) { mode.operation = B; }
			else
			{
				err(1); 
				mode.operation = ERR;
				break;
			}
		}
		else if(strcmp(argv[i],"-c") == 0)
		{
			//check format of the arguments; one of them needs to be .hcz
			int check1 = checkHCZ(argv[numArgs-1]);
			int check2 = checkHCZ(argv[numArgs-2]);
			if( (check1 == 1)||(check2 == 1) ) 
			{ 
				printf("hmm\n");
				err(2); 
				mode.operation = ERR;
				break;
			}
			else 
			{//add the filename and codebook to mode
				if(check1 == 1) 
				{
					int fileSize = strlen(argv[numArgs-1]);
					strncpy(mode.codebook, argv[numArgs-1], fileSize);
					fileSize = strlen(argv[numArgs-2]);
					strncpy(mode.filename, argv[numArgs-2], fileSize);
				}
				else
				{
					int fileSize = strlen(argv[numArgs-1]);
					strncpy(mode.filename, argv[numArgs-1], fileSize);
					fileSize = strlen(argv[numArgs-2]);
					strncpy(mode.codebook, argv[numArgs-2], fileSize);
				}
			}
			if(mode.operation == FIRST) { mode.operation = C; }
			else
			{
				err(1); 
				mode.operation = ERR;
				break;
			}
		}
		else if(strcmp(argv[i],"-d") == 0)
		{
			int check1 = checkHCZ(argv[numArgs-1]);
			int check2 = checkHCZ(argv[numArgs-2]);
			if( (check1 != 1)&&(check2 != 1) ) 
			{ 
				err(0); 
				mode.operation = ERR;
				break;
			}
			else if( (check1 == 1)&&(check2 == 1) ) 
			{ 
				err(2);
				mode.operation = ERR; 
				break;
			}
			else 
			{//set the filename and codebook in mode
				if(check1 == 1) 
				{//initialize the filename and codebook
					int fileSize = strlen(argv[numArgs-1]);
					strncpy(mode.filename, argv[numArgs-1], fileSize);
					fileSize = strlen(argv[numArgs-2]);
					strncpy(mode.codebook, argv[numArgs-2], fileSize);
				}
				else
				{
					int fileSize = strlen(argv[numArgs-1]);
					strncpy(mode.codebook, argv[numArgs-1], fileSize);
					fileSize = strlen(argv[numArgs-2]);
					strncpy(mode.filename, argv[numArgs-2], fileSize);
				}
			}
			if(mode.operation == FIRST) { mode.operation = D; }
			else
			{
				err(1); 
				mode.operation = ERR;
				break;
			}
		}
		else 
		{ //a flag is not found, check the arguments for errors
			//too many args--B only needs 1 argument, -c and -d need 2 arguments. therefore max remainder is 3 (2 + 1 for ./a.out)
			if( ((numArgs - i) > 3) || ( ((numArgs - i) == 3) && (mode.operation == B) )) 
			{ 
				err(3); 
				mode.operation = ERR;
				break;
			}
			//not enough arguments 
			else if( (numArgs - i) <= 0) 
			{ 
				err(4);
				mode.operation = ERR;
				break;
			}
			//B is the only flag that takes 1 argument (1 + 1 for ./a.out)
			else if(((numArgs-i) == 1)&&(mode.operation == B) ) 
			{ 
				int filesize = strlen(argv[i]);
				strncpy(mode.filename, argv[i], filesize);
				break;
			}
			//whatever path/file is given should begin with "./" ; its okay if == 0
			else if(strcmp(argv[i], "./") < 0) 
			{ 
				err(2); 
				mode.operation = ERR;
				break;
			}
		}
		++i;
	} while(i <= (numArgs-1));
	return mode;
}
int checkDir(char *pathname, char *filename)
{//return to indicate that the file type of filename is a directory or not
	//create a temp char array to call update path without changing the original pathname
	char newPath[strlen(pathname)+PATH_SIZE];
	//if filename is given to use, we need to add it to the temp path; otherwise, pathname is the path we need to check
	if(filename != NULL) { strcpy(newPath, updatePathname(pathname, filename)); }
	else { strcpy(newPath, pathname); }

	//call stat on the pathname and return the result of S_ISDIR()
	struct stat check;
	int result = stat(newPath, &check);
	if(result < 0)
	{
		return result;
	}
	if(S_ISDIR(check.st_mode)) { return DIRECTORY; }
	return OTHER;
}
int checkFile(char *filename)
{
	struct stat check;
	int checkie = stat(filename, &check);
	if(checkie < 0)
	{
		int errLen = strlen(strerror(errno));
		write(STDERR, "%s\n", errLen+1);
		return ERR;
	}

	return(S_ISREG(check.st_mode));
}
char* findLast(char *pathname)
{//return the last token in the pathname
	char *second = pathname;
	char *last = strtok(pathname, "/");
	while(last != NULL)
	{
		second = last;
		last = strtok(NULL, "/");
	}
	return second; //will return the original pathname if there is <= 1 token
}
struct node *newNode(char *token, char *binVal)
{//(o^^)o ~ initialize the new node ~ o(^^o)
	struct node *new = malloc(sizeof(struct node));
	new->token = malloc(TOK_SIZE);
	if(token == 0) { new->token = NULL; }
	else { strncpy(new->token, token, strlen(token)); }
	new->binVal = malloc(BIN_SIZE);
	if(binVal == 0) { new->binVal = NULL; }
	else { strncpy(new->binVal, binVal, strlen(binVal)); }
	new->freq = 0;
	//leaf node: height is -1, null ptr children
	new->height = -1; //we don't need this for BST.c
	new->left = NULL;
	new->right = NULL;
	return new;
}
char *searchWord(struct node *root, char *word)
{
	if( (root == NULL)||(strcmp(word,"") == 0) ) { return NULL; }

	if(strcmp(root->token, word) == 0) { return root->binVal; }

	char *retVal;

	//check the left
	if(root->left) 
	{ 
		retVal = searchWord(root->left, word); 
		if(retVal != NULL) { return retVal; }
	}
	//check the right
	if(root->right)
	{
		retVal = searchWord(root->right, word);
		if(retVal != NULL) { return retVal; }
	}

	return NULL;

}
struct node *insertGivenToks(struct node *root, struct node *new)
{//for compressing and decompressing, build a tree from the given codebook. no need to balance the tree bc the encoding was already determined
	int i = 0;
	int pathSize = strlen(new->binVal);
	char cur = new->binVal[i];
	
	if( (root->token == NULL)||(pathSize == 0) ) { return new; }
	//the token is uninitialized, we are at a dummy variable?
	else if(cur == '\0') { return NULL; }
	//base case:
	else if( (root->token == new->token)&&(root->binVal) ) { return root; } //no need to insert twice :)  / avoid initialization error (root->binVal) is false for dummy nodes
	
	do { //traverse the tree depending on the value at binVal, add the new node in the correct place
		if(cur == '0')
		{//go to the left, either adding the new node or adding a dummy node
			if( (pathSize-1) == i)
			{ 
				root->left = new; 
				return root;
			}
			else if(root->left == NULL)
			{
				struct node *dummy = newNode("dummy", 0);
				root->left = dummy;
				root = root->left;
			}
			else { root = root->left; }
		}
		else if(cur == '1')
		{//go to the right, either add the new node or a dummy node
			if( (pathSize-1) == i)
			{
				root->right = new;
				return root;
			}
			else if(root->right == NULL)
			{
				struct node *dummy = newNode("dummy", 0);
				root->right = dummy;
				root = root->right;
			}
			else { root = root->right; }
		}
		else
		{
			return NULL;
		}
		++i;
		cur = new->binVal[i];
	}while( (pathSize > i)&&(cur != '\0') );
	//if we get here, something was worng
	return NULL;
	return root;
}
int compress(char *filename, char *codebook)
{
	//open the codebook
	int codebookFD = open(codebook, O_RDONLY, 0);
	if(codebook < 0)
	{
		printf("Error opening codebook %s : %s\n", codebook, strerror(errno));
		return 0;
	}

	/**struct stat check;
	int checkie = stat(codebook, &check);
	if(S_ISREG(checkie)) { printf("its a file \n"); }
	else { printf("sisreg determined it is not a file\n"); }**/

	off_t codebookSize = lseek(codebookFD, SEEK_CUR, SEEK_END);
	if(codebookSize < 0)
	{
		printf("error gettig size of codebook from lseek: %s", strerror(errno));
		return 0;
	}
	else if(codebookSize == 0)
	{
		printf("Given an invalid codebook %s\n", codebook);
		return 0;
	}

	off_t reset = lseek(codebookFD, 0, SEEK_SET);
	if(reset < 0)
	{
		printf("error reseting the offset of %s: %s\n", codebook, strerror(errno));
		return 0;
	}
	struct node *tree;
	tree = newNode("root", 0);
	struct node *root;
	root = tree;
	//read the codebook
	char token[TOK_SIZE] = "\0";
	char binVal[BIN_SIZE] = "\0";
	size_t checkReadSize;
	int i = 0;

	while(i <= codebookSize)
	{ //add each token of the codebook to the tree
		char cur;
		int retVal = read(codebookFD, &cur, 1);
		if(retVal < 0) { printf("error reading!\n"); }
		else if(retVal == 0) 
		{ 
			break; 
		} //EOF
		if( ((cur == '/')&&(i < 2))||(cur == '\t') )
		{ 
			++i;
			continue; 
		}
		else if( (cur == '0')&&(token[0] == '\0') )
		{
			strcat(binVal, "0");
		}
		else if( (cur == '1')&&(token[0] == '\0') )
		{
			strcat(binVal, "1");
		}
		else if(cur == '\n')
		{
			if(token[0] == '\0') 
			{ 
				++i;
				continue; 
			}
			struct node *new;
			new = newNode(token, binVal);
			tree = insertGivenToks(tree, new);
			tree = root;
			strncpy(token, "", 1);
			strncpy(binVal, "", 1);
		}
		else
		{
			if(token[0] == '\0')
			{
				strcpy(token, ((char[2]) { (char) cur, '\0'}) );
			}
			else
			{
				char buff[2];
				strcpy(buff, ((char[2]) { (char) cur, '\0' }) );
				strcat(token, buff);
			}
		}
		++i;
	}

	close(codebookFD);

	if(checkReadSize == -1)
	{
		printf("error reading codebook %s: %s\n", codebook, strerror(errno));
		return 0;
	}
	else if(i != (codebookSize-1) )
	{
		printf("could not read the entire codebook %s\n", codebook);
		return 0;
	}
	//create a return file
	int retFile = open("compression_result.hcz.txt", O_RDWR|O_CREAT|O_APPEND, 00666);
	if(retFile < 0)
	{
		printf("error creating the result file: %s\n", strerror(errno));
		return 0;
	}

	//open the given file
	/**
	checkie = stat(filename, &check);
	if(checkie < 0) { printf("error: %s\n", strerror(errno)); }

	if(S_ISREG(checkie)) { printf("its a file \n"); }
	else { printf("sisreg determined it is not a file\n"); } **/

	int fileFD = open(filename, O_RDONLY);
	if(fileFD < 0)
	{
		printf("error opening the file %s: %s\n", filename, strerror(errno));
		return 0;
	}

	codebookSize = lseek(fileFD, SEEK_CUR, SEEK_END);
	if(codebookSize < 0)
	{
		printf("error gettig size of codebook from lseek: %s", strerror(errno));
		return 0;
	}
	else if(codebookSize == 0)
	{
		printf("Given an invalid codebook %s\n", codebook);
		return 0;
	}

	reset = lseek(fileFD, 0, SEEK_SET);
	if(reset < 0)
	{
		printf("error reseting the offset of %s: %s\n", codebook, strerror(errno));
		return 0;
	}

	char current;
	i = 0;
	strncpy(token, "", 1);

	while(i <= codebookSize)
	{
		//read each token (deliminers are specified, check the assignemnt description)
		int retV = read(fileFD, &current, 1);
		if(token[0] == '\0') { strcpy(token, ((char[2]) { (char) current, '\0'})); }
		else 
		{
			char bufff[2];
			strcpy(bufff, ((char[2]) { (char) current, '\0'}));
			strcat(token, bufff);
		}
		//find the value of the token 
		char *retB = searchWord(tree, token);
		if(retB == NULL) 
		{ 
			++i;
			continue; 
		}
		else
		{ //add the value of the token to the return file
			write(retFile, retB, strlen(retB));
			strncpy(token, "", 1);
		}
		++i;
		retB = NULL;
	}
	
	close(fileFD);
	close(retFile);
	return 1;
}
struct node *traverse(struct node *root, char binVal)
{
	if(root == NULL) { return NULL; }
	if(binVal == '\0') { return root; }
	else
	{
		if(binVal == '0') { return root->left; }
		else { return root->right; }
	}
}
char *searchBin(struct node *root, char *binVal)
{
	if(root == NULL) { return NULL; }
	else if(binVal[0] == '\0') { return NULL; }

	struct node *ret;
	ret = root;
	int length = strlen(binVal);
	for(int i = 0; i < length; i++)
	{
		ret = traverse(ret, binVal[i]);
	}
	if( ret == NULL) { return "-1"; }
	else if( (strcmp(ret->token, "dummy") == 0)||(strcmp(ret->token, "root")== 0) ) { return "-1"; }
	return ret->token;
}
int decompress(char *filename, char *codebook)
{
	int codebookFD = open(codebook, O_RDONLY, 0);
	if(codebookFD < 0) { return 0; }

	off_t codebookSize = lseek(codebookFD, SEEK_CUR, SEEK_END);
	if(codebookSize < 0)
	{
		printf("error gettig size of codebook from lseek: %s", strerror(errno));
		return 0;
	}
	else if(codebookSize == 0)
	{
		printf("Given an invalid codebook %s\n", codebook);
		return 0;
	}

	off_t reset = lseek(codebookFD, 0, SEEK_SET);
	if(reset < 0)
	{
		printf("error reseting the offset of %s: %s\n", codebook, strerror(errno));
		return 0;
	}

	struct node *tree;
	tree = newNode("root", 0);
	struct node *root;
	root = tree;
	//read the codebook
	char token[TOK_SIZE] = "\0";
	char binVal[BIN_SIZE] = "\0";
	size_t checkReadSize;
	int i = 0;

	while(i <= codebookSize)
	{ //add each token of the codebook to the tree
		char cur;
		int retVal = read(codebookFD, &cur, 1);
		if(retVal < 0) { printf("error reading!\n"); }
		else if(retVal == 0) 
		{ 
			break; 
		} //EOF
		if( ((cur == '/')&&(i < 2))||(cur == '\t') )
		{ 
			++i;
			continue; 
		}
		else if( (cur == '0')&&(token[0] == '\0') )
		{
			strcat(binVal, "0");
		}
		else if( (cur == '1')&&(token[0] == '\0') )
		{
			strcat(binVal, "1");
		}
		else if(cur == '\n')
		{
			if(token[0] == '\0') 
			{ 
				++i;
				continue; 
			}
			struct node *new;
			new = newNode(token, binVal);
			tree = insertGivenToks(tree, new);
			tree = root;
			strncpy(token, "", 1);
			strncpy(binVal, "", 1);
		}
		else
		{
			if(token[0] == '\0')
			{
				strcpy(token, ((char[2]) { (char) cur, '\0'}) );
			}
			else
			{
				char buff[2];
				strcpy(buff, ((char[2]) { (char) cur, '\0' }) );
				strcat(token, buff);
			}
		}
		++i;
	}

	close(codebookFD);

	if(checkReadSize == -1)
	{
		printf("error reading codebook %s: %s\n", codebook, strerror(errno));
		return 0;
	}
	else if(i != (codebookSize-1) )
	{
		printf("could not read the entire codebook %s\n", codebook);
		return 0;
	}
	//create a return file
	int retFile = open("decompression_result.txt", O_RDWR|O_CREAT|O_APPEND, 00666);
	if(retFile < 0)
	{
		printf("error creating the result file: %s\n", strerror(errno));
		return 0;
	}

	int fileFD = open(filename, O_RDONLY);
	if(fileFD < 0)
	{
		printf("error opening the file %s: %s\n", filename, strerror(errno));
		return 0;
	}

	codebookSize = lseek(fileFD, SEEK_CUR, SEEK_END);
	if(codebookSize < 0)
	{
		printf("error gettig size of codebook from lseek: %s", strerror(errno));
		return 0;
	}
	else if(codebookSize == 0)
	{
		printf("Given an invalid codebook %s\n", codebook);
		return 0;
	}

	reset = lseek(fileFD, 0, SEEK_SET);
	if(reset < 0)
	{
		printf("error reseting the offset of %s: %s\n", codebook, strerror(errno));
		return 0;
	}

	i = 0;
	strncpy(binVal, "", 1);
	char current;
	char *retB;

	while(i <= codebookSize)
	{
		int retVal = read(fileFD, &current, 1);
		if(binVal[0] == '\0') { strcpy(binVal, ((char[2]) { (char) current, '\0'})); }
		else 
		{
			char bufff[2];
			strcpy(bufff, ((char[2]) { (char) current, '\0'}));
			strcat(binVal, bufff);
		}

		retB = searchBin(tree, binVal);
		if(strcmp(retB, "-1") == 0) 
		{
			++i;
			continue;
		}
		else
		{
			write(retFile, retB, strlen(retB));
			strncpy(binVal, "", 1);
		}
		++i;
	}

	close(fileFD);
	close(retFile);
	return 1;
}	

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
int num=1000;
int tokenizer(char** str, int*freq, char* filename)
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
           		num=num+1000;
            	str=(char**)realloc(str,(num)*sizeof(char*));
            	freq=(int*)realloc(freq,(num)*sizeof(int));
            	str[num-1000]=malloc((strlen(ptr)+1)*sizeof(char));
            	strncpy(str[num-1000],ptr,(strlen(ptr)+1));
            	freq[num-1000]=1;
    		}
      }
}

/***************************************************************************\
*                                    MAIN                                   *
\***************************************************************************/
int main(int argc, char **argv)
{
	if(argc < 3)
	{ 
		err(5);
		return 0;
	}

	struct flags mode = buildMode(argc, argv);
	
	if(mode.operation == ERR) { return 0; }
	else if( (mode.operation == B)&&(strcmp(mode.filename, "") == 0) )
	{ //move the filename into the correct field of mode
		int filesize = strlen(argv[argc-1]);
		strncpy(mode.filename, argv[argc-1], filesize);
	}
	
  	int retVal;
	if(mode.recursive == R)
	{


	}
	else
	{
		if(mode.operation == C)
		{
			retVal = compress(mode.filename, mode.codebook);
			if(retVal == 0) 
			{ 
				err(8);
				return 0;
			}
		}
		else if(mode.operation == D)
		{
			retVal = decompress(mode.filename, mode.codebook);
			if(retVal == 0) 
			{ 
				err(9);
				return 0;
			}
		}
		else //mode.operation == B ; non recursive build 
		{
			char **str= (char**)malloc(100 * sizeof (char*));
    			int *freq= malloc(100 * sizeof(int));
    			int num=100;
    			num=tokenizer(str,freq,mode.filename);
    			int height=0;
      			while(freq[height]>=1){height++;}
   			huff *root = maketree (str, freq, height-1);
  			// Prints out Huffman codes using the Huffman tree built above 
  			int pt = 0;
  			char*words=malloc((height-1)*sizeof(char));
  			int hfile=open("HuffmanCodebook", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
  			printc (root, words, pt, hfile);
  			close(hfile);
		}
	}
  
	return 0;
}

