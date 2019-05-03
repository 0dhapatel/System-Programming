#ifndef FILECOMPRESSOR_H
#define FILECOMPRESSOR_H

/***************************************************************************\
*                                 DEFINES                                   *
\***************************************************************************/
//flags
#define B 'b'
#define C 'c'
#define D 'd'
#define R 'R'
//error & init checking
#define FIRST 'f'
#define ERR -1
//file type
#define FILE 20
#define DIRECTORY 21
#define OTHER 22
//read, write
#define STDIN 0
#define STDOUT 1
#define STDERR 2
//values to increase readability 
#define PATH_SIZE 50
#define TOK_SIZE 100
#define BIN_SIZE 50
//macros
#define MIN(x, y) ((x < y) ?x :y) 
#define MAX(x, y) ((x > y) ?x :y)

/***************************************************************************\
*                                 STRUCTS                                   *
\***************************************************************************/
struct flags {
	char recursive; //set if -R is passed
	char operation; //set with the flag(s) passed
	char filename[PATH_SIZE]; //store the given file or directory
	char codebook[PATH_SIZE];
};

struct node {
	char *token; //a word or separating character in the file
	char *binVal; //binary value corresponding to the token
	float freq; //for Huffman Coding
	int height; //for AVL balancing
	struct node *left; //children
	struct node *right;
};

// A Huffman tree node 
typedef struct huffnode
{
  // One of the input characters 
  char *word;
  // Frequency of the character 
  unsigned freq;
  // Left and right child of this node 
  struct huffnode *left, *right;
}huff;

// A Min Heap:  Collection of min heap (or Hufmman tree) nodes 
typedef struct heapnode
{
  // Current height of min heap 
  unsigned height;
  // capacity of min heap 
  unsigned cap;
  // Attay of minnode node pointers 
  huff **harr;
}heapnode;


/***************************************************************************\
*                               FUNCTIONS                                   *
\***************************************************************************/
//fileCompressor.c
struct flags buildMode(int numArgs, char **argv);
//huffmanCoding.c


#endif
