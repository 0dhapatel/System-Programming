#include <openssl/sha.h> //compiles without importing library on ilab
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "WTF.h"

//scan through all local files in the given project name
//generate a hash for every file 
	//version number at the top
	//newline
	//<file version number><<project name>/path/filename><hash code> (for each file in the project)
char *manifest_hash(char *line)
{
	//given the pathname of the file
	//return the sha256 hash of the contents of the file
	int x;
	char ret[MAXLINE];

    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, line, strlen(line));
    SHA256_Final(digest, &sha256);

    for(x = 0; x < SHA256_DIGEST_LENGTH; x++)
    {
        //printf("%02x\n", digest[x]);
        strcat(ret, digest[x]);
    }
    return ret;
    //sgx_exit(NULL);
}
char *manifest_file(char *pathname)
{
	char hash[MAXLINE], line[MAXLINE], *c, full[MAXLINE];
	int fp = open(pathname, O_RDONLY, 0700);
	if(fp == NULL)
	{
		write(STDERR, "Cannot open file to hash\n", 25);
		return "-1";
	}
	strcat(full, "<");
	
	//get the version number and store in c
	//strcat(full, c);
	
	strcat(full, "><");
	strcat(full, pathname);
	strcat(full, "><");

	while(read(fp, &c, 1) != 0)
	{
		if(strcmp(c, '\n') == 0)
		{
			strcat(line, manifest_hash(hash));
			strncpy(hash, c, 1);
		}
		strcat(hash, c);
	}
	close(fp);
	strcat(full, ">");
	return full;
}
char *manifest_dir(char *pathname)
{
  //open the directory and loop
    //if a directory, open it and recursively call manifest_dir
    //if file, call manifest_file and store the version number, pathname to the file, and hash of the file all in one line
  //return the values
}
void manifest_scan(char *projectname, int version)
{
	//create the manifest file and add the version
	int fp=open(".Manifest", O_RDWR | O_CREAT, 0777);
	write(fp, version, 1);
	void *buf = "\n";
	write(fp, buf, strlen(buf));
	//given the project name, scan through all the files and directories in the project
	struct dirent *pd;
	DIR *PD;
	PD = opendir(projectname);
	if(PD == NULL)
	{
		write(STDERR, "Cannot open the project directory\n", 34);
	//write the .manifest to the socket
		close(fp);
		return;
	}
	struct stat path;
	char pathname[MAXLINE];
	strcat(pathname, projectname);
	while((pd = readdir(PD))!= NULL)
	{
		stat(pd, &path);
		if(S_ISREG(path.st_mode))
		{
			//add the file version number
			//add the projectname/path/filename
			buf = pathname + '/' + path->d_name;
			write(fp, buf, strlen(buf));
			//call the hash function
			//add it to the .manifest
			buf = "/";
			write(fp, buf, strlen(buf));
		}
		else if (S_ISDIR(path.st_mode))
		{
			//update the pathname
			char save[strlen(pathname)];
			strcpy(save, pathname);
			strcat(pathname, "/");
			strcat(pathname, pd->d_name);
			//go through and check for files
			char *newline;
			do
			{
				//call manifest_dir to recursively check directory
				newline = manifest_dir(pathname);
				//add to the manifest
				write(fp, newline, strlen(newline));
			} while(strcmp(newline, "-1") != 0)
			strncpy(pathname, save, strlen(save));
		}
		else 
		{
			//skip this element
		}

	}
	//add a newline to the manifest for each file found, and generate a hash for each file
	//return the .manifest file
}
