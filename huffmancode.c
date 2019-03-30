#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct huffnode
{
    char ch; // letter
    unsigned int freq; // binary frequency
    huffnode* left, *right; // left and right child
}huff;

// assuming frequency r in order
// have to change list
//have to change to string

huff * heap[100];
int size=0;

void insert(huff * el) //element
{
    size++;
    heap[size] = el;
    int hsize = size;
    while(heap[hsize/2] -> freq > el -> freq){
        heap[hsize] = heap[hsize/2];
        hsize /= 2;
    }
    heap[hsize] = el;
}

huff * freemin()
{
    huff * min,*last;
    int child, now;
    min = heap[1];
    last = heap[size--];
    for(now = 1; now*2 <= size ;now = child){
         child = now*2;
         if(child != size && heap[child+1]->freq < heap[child] -> freq ){
            child++;
        }
        if(last -> freq > heap[child] -> freq){
            heap[now] = heap[child];
        }
        else
        {
            break;
        }
    }
    heap[now] = last;
    return min;
}

void print(huff *temp, char *code)
{
    if(temp->left==NULL && temp->right==NULL){
        printf("char %c code %s\n",temp->ch,code);
        return;
    }
    int len = strlen(code);
    char lcode[10],rcode[10];
    strcpy(lcode,code);
    strcpy(rcode,code);
    lcode[len] = '0';
    lcode[len+1] = '\0';
    rcode[len] = '1';
    rcode[len+1] = '\0';
    print(temp->left,lcode);
    print(temp->right,rcode);
}

int main()
{
    heap[0] = (huff *)malloc(sizeof(huff));
    heap[0]->freq = 0;
    int n ;
    printf("Enter the no of characters: ");
    scanf("%d",&n);
    printf("Enter the characters and their frequencies: ");
    char ch;
    int freq,i;
    for(i=0;i<n;i++){
        scanf(" %c",&ch);
        scanf("%d",&freq);
        huff * temp = (huff *) malloc(sizeof(huff));
        temp -> ch = ch;
        temp -> freq = freq;
        temp -> left = temp -> right = NULL;
        insert(temp);
    }
    if(n==1){
        printf("char %c code 0\n",ch);
        return 0;
    }
    for(i=0;i<n-1 ;i++){
        huff * left = freemin();
        huff * right = freemin();
        huff * temp = (huff *) malloc(sizeof(huff));
        temp -> ch = 0;
        temp -> left = left;
        temp -> right = right;
        temp -> freq = left->freq + right -> freq;
        insert(temp);
    }
    huff *tree = freemin();
    char code[10];
    code[0] = '\0';
    print(tree,code);
}
