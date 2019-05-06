#ifndef WTF_H
#define WTF_H

//read, write
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define MAXLINE 255

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;

#endif
