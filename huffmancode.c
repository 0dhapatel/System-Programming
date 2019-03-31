#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  // Current size of min heap 
  unsigned size;
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
  // current size is 0 
  minnode->size = 0;
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
  if (left < minnode->size
      && minnode->harr[left]->freq < minnode->harr[smallest]->freq)
    {
      smallest = left;
    }
  if (right < minnode->size
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
  minnode->harr[0] = minnode->harr[minnode->size - 1];
  --minnode->size;
  order (minnode, 0);
  return temp;
}

// A  function to insert a new node to Min Heap 
void insnode (heapnode *minnode, huff *minnodeNode)
{
  ++minnode->size;
  int i = minnode->size - 1;
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
  int n = minnode->size - 1;
  int i;
  for (i = (n - 1) / 2; i >= 0; --i)
    order (minnode, i);
}

// A  function to print an array of size n 
void printstr (int str[], int n)
{
  int i;
  for (i = 0; i < n; ++i){
    printf ("%d", str[i]);
  }
  printf ("\n");
}

// Creates a min heap of capacity equal to size and inserts all character of word[] in min heap. Initially size of min heap is equal to cap 
heapnode * makenode (char *word[], int freq[], int size)
{
  int i;
  heapnode *minnode = creminheap (size);
  for (i = 0; i < size; ++i){
    minnode->harr[i] = node (word[i], freq[i]);
  }
  minnode->size = size;
  buinode (minnode);
  return minnode;
}

// Function that builds Huffman tree 
huff * maketree (char *word[], int freq[], int size)
{
  huff *left, *right, *top;
  // Create a min heap of capacity equal to size. Initially, there are modes equal to size. 
  heapnode *minnode = makenode (word, freq, size);
  // Iterate while size of heap doesn't become 1 
  while (!(minnode->size == 1))
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
void printc (huff *root, int str[], int top)
{
  // Assign 0 to left edge and recur 
  if (root->left){
      str[top] = 0;
      printc (root->left, str, top + 1);
    }

  // Assign 1 to right edge and recur 
  if (root->right){
      str[top] = 1;
      printc (root->right, str, top + 1);
    }
  // If this is a leaf node, then it contains one of the input characters, print the character and its code from str[] 
  if (!(root->left) && !(root->right)){
      printf ("%s: ", root->word);
      printstr (str, top);
    }
}

// Driver program to test above functions 
int main ()
{
  char *str[] = { "'ade'", "'b'", "'c'", "'d'", "'e'", "'f'" };
  int freq[] = { 45, 9, 13, 13, 16, 5 };
  int size = sizeof (str) / sizeof (str[0]);
  printf("%d\n\n",size);
  //Making huffman tree
   huff *root = maketree (str, freq, size);
  // Prints out Huffman codes using the Huffman tree built above 
  int words[100], pt = 0; // used 100 randomly
  printc (root, words, pt);
  return 0;
}
