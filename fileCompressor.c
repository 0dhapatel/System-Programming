#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include "filecompressor.h"

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
  
  //if mode.recursive == R , do recursion
  //else do non-recursive calls
  
  return 0;
}

