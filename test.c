#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 

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

// A  function allocate a new min heap node with given character and frequency of the character 
huff *node (char *word, unsigned freq)
{
  huff *temp = (huff *) malloc (sizeof (huff));
  temp->left = temp->right = NULL;
  temp->word = word;
  temp->freq = freq;
  return temp;
}

// A  function to create a min heap of given capacity 
heapnode *creminheap (unsigned cap)
{
  heapnode *minnode = (heapnode *) malloc (sizeof (heapnode));
  // current height is 0 
  minnode->height = 0;
  minnode->cap = cap;
  minnode->harr = (huff **) malloc (minnode->cap * sizeof (huff *));
  return minnode;
}

// The standard order function. 
void order (heapnode *minnode, int ptr)
{
  int smallest = ptr;
  int left = 2 * ptr + 1;
  int right = 2 * ptr + 2;
  if (left < minnode->height
      && minnode->harr[left]->freq < minnode->harr[smallest]->freq)
    {
      smallest = left;
    }
  if (right < minnode->height
      && minnode->harr[right]->freq < minnode->harr[smallest]->freq)
    {
      smallest = right;
    }
  if (smallest != ptr)
    {
      huff *temp = minnode->harr[smallest];
      minnode->harr[smallest] = minnode->harr[ptr];
      minnode->harr[ptr] = temp;
      order (minnode, smallest);
    }
}

// A standard function to extract minimum value node from heap 
huff *freemin (heapnode *minnode)
{
  huff *temp = minnode->harr[0];
  minnode->harr[0] = minnode->harr[minnode->height - 1];
  --minnode->height;
  order (minnode, 0);
  return temp;
}

// A  function to insert a new node to Min Heap 
void insnode (heapnode *minnode, huff *minnodeNode)
{
  ++minnode->height;
  int i = minnode->height - 1;
  while (i && minnodeNode->freq < minnode->harr[(i - 1) / 2]->freq)
    {
      minnode->harr[i] = minnode->harr[(i - 1) / 2];
      i = (i - 1) / 2;
    }
  minnode->harr[i] = minnodeNode;
}

// A standard funvtion to build min heap 
void buinode (heapnode *minnode)
{
  int n = minnode->height - 1;
  int i;
  for (i = (n - 1) / 2; i >= 0; --i)
    order (minnode, i);
}

// Creates a min heap of capacity equal to height and inserts all character of word[] in min heap. Initially height of min heap is equal to cap 
heapnode * makenode (char *word[], int freq[], int height)
{
  int i;
  heapnode *minnode = creminheap (height);
  for (i = 0; i < height; ++i){
    minnode->harr[i] = node (word[i], freq[i]);
  }
  minnode->height = height;
  buinode (minnode);
  return minnode;
}

// Function that builds Huffman tree 
huff * maketree (char *word[], int freq[], int height)
{
  huff *left, *right, *top;
  // Create a min heap of capacity equal to height. Initially, there are modes equal to height. 
  heapnode *minnode = makenode (word, freq, height);
  // Iterate while height of heap doesn't become 1 
  while (!(minnode->height == 1))
    {
      // Extract the two minimum freq items from min heap 
      left = freemin (minnode);
      right = freemin (minnode);
      // Create a new internal node with frequency equal to the sum of the two nodes frequencies. 
      // Make the two extracted node as  left and right children of this new node. 
      // Add this NULL to the min heap for internal nodes, not used 
      top = node (NULL, left->freq + right->freq);
      top->left = left;
      top->right = right;
      insnode (minnode, top);
    }
  // The remaining node is the root node and the tree is complete. 
  return freemin (minnode);
}

// Prints huffman codes from the root of Huffman Tree. It uses str[] to store codes 
void printc (huff *root, char *str, int top, int file)
{
  // Assign 0 to left edge and recur 
  if (root->left){
      str[top] = '0';
      printc (root->left, str, top + 1, file);
    }

  // Assign 1 to right edge and recur 
  if (root->right){
      str[top] = '1';
      printc (root->right, str, top + 1, file);
    }
  // If this is a leaf node, then it contains one of the input characters, print the character and its code from str[] 
  if (!(root->left) && !(root->right)){
  		str[top]='\0';
  		write(file, str, top);
    	//printf ("%s", str);
    	write(file, "\t", 1);
  		write(file, root->word, strlen(root->word));
  		write(file, "\n", 1);
      //printf ("\t%s\n", root->word);
    }
}



/*testing*/




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

int main(int argc, char** argv) 
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
    return 0;
    }
    close(file);
    //printf("%s\n",myStr);
    char *fill=myStr;
    //int s=check.st_size;
    //printf("%d\n\n",check.st_size);
    char **str= (char**)malloc(100 * sizeof (char*));
    int *freq= malloc(100 * sizeof(int));
    int num=100,j=0,no=0;
    char *ptr;// = strtok(myStr, del);
	while(ptr=strtok_r(fill," \t\n",&fill))
	{
		//printf("ptr=%d\n", strlen(ptr));
		//printf("ptr=%s\n\n", ptr);
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
      int height=0;
      while(freq[height]>=1){height++;}
  	//printf("%d\n\n",height);
   huff *root = maketree (str, freq, height-1);
  // Prints out Huffman codes using the Huffman tree built above 
  int /*words[100],*/ pt = 0;
  char*words=malloc((height-1)*sizeof(char));// used 100 randomly
  int hfile=open("HuffmanCodebook", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
  printc (root, words, pt, hfile);
  //write(hfile,"hi",3);
  close(hfile);
    return 0;
}
