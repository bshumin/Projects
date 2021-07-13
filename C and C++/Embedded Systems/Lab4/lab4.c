#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>

// compile with: gcc -o hm lab4.c -Wall -g

// structs and globals
struct binTree{
  unsigned int    freq;
  unsigned char   letter;
  struct binTree  *left;
  struct binTree  *right;
};

struct bintree *alphabetSort;
struct binTree alphabetUnsorted[256];

typedef struct sortQueue{
  int             queueSize;
  struct binTree  *nodes[256];
} queue;

unsigned int* frequency; //frequency list
queue*        sq; // sorting queue

// global variables for dictionary
unsigned int        *frequency;
unsigned int        totalFreq;
int                 *length;     // length of pattern at initial point
unsigned short int  dictCount;
unsigned int        code[256] = {0};
unsigned int        lenCode[256] = {0};

// functions compression, decompression, binary tree creation
void            buildFrequency (char* inFile); // function to build frequency table from input file
void            setStruct      (); // function for setting values of freqs on compression
struct binTree* buildBinaryTree();
void            fillStruct     (char* inFile); // function for fillling struct freqs on decompression
void            printTable     (); // function to print out current frequency table
void            printTree      (queue* root);
void            traverseBinTree(int   lr,     int   codeToAdd, int len, struct binTree* node);
void            compress       (char* inFile, char* outFile);
void            decompress     (char* inFile, char* outFile);
void            encodeData     (char* inFile, char* outFile);
void            decodeData     (char* inFile, char* outFile);

// helper functions for sorting queue
void            initializeQueue();
int             getLeft        (int initial);
int             getRight       (int initial);
int             getParent      (int initial);
void            addEle         (struct binTree* node);
void            initializeNodes();
void            makeHeap       (int index);
struct binTree* removeMinFreq  ();

//============================================================================//
//=============================MAIN FUNCTION==================================//
//============================================================================//
int main(int argc, char *argv[]) {
  int i;
  // allocate size for sorting queueSize
  sq = (queue*)malloc(sizeof(queue)*256);
  for(i=0;i<256;i++){
    sq->nodes[i] = malloc(sizeof(sq->nodes[i]));
  }

  frequency = (unsigned int*)calloc(1,sizeof(unsigned int)*256);

  // check number of arguments
  if (argc != 4){
    printf("Usage:  ./hm [input filename] [compress/decompress] [output filename]\n");
    printf("NOTE:   -c compress | -d decompress\n");
  	exit(0);
  }

  // decide to either encode or decode the data
  if(strcmp(argv[2],"-c") == 0){ // encode data
    compress(argv[1],argv[3]);
  } else
  if(strcmp(argv[2],"-d") == 0){ // decode data
    decompress(argv[1], argv[3]);
  } else{
    printf("Incorrect flag detected!\n");
    printf("NOTE:   -c compress | -d decompress\n");
  }
}
//============================================================================//

void buildFrequency(char* inFile){
  FILE*         fpt;
  unsigned char val;

  // initialize unsorted struct
  setStruct();

  fpt=fopen(inFile,"rb");
  if (fpt == NULL)
  {
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }

  // set frequencies
  totalFreq = 0;
  while(1){
    fread(&val, 1, sizeof(unsigned char), fpt);
    if(feof(fpt)){
      break;
    }
    alphabetUnsorted[val].freq++;
    frequency[val]++;
    totalFreq++;
  }
  // printf("frequencies found!\n");
  //printTable();
  fclose(fpt);
}

// function for initializing structure in compress
void setStruct(){
  int i;

  for(i=0; i<256; i++){
    alphabetUnsorted[i].freq   = 0;
    alphabetUnsorted[i].letter = i;
    alphabetUnsorted[i].left   = NULL;
    alphabetUnsorted[i].right  = NULL;
  }
  // printf("struct initialized!\n");
}

struct binTree* buildBinaryTree(){
  unsigned char   i;
  int             j;
  struct binTree  *min1, *min2, *newNode;

  //printTable();

  // initialize sorting queueSize
  initializeQueue();

  // insert heap nodes with initialized
  initializeNodes();
  j=sq->queueSize;

  for(i=0;i<j-1;i++){
    newNode = malloc(sizeof(struct binTree));

    //find min node values
    min1 = removeMinFreq();
    min2 = removeMinFreq();

    // set min nodes to left and right nodes of new node
    newNode->left  = min1;
    newNode->right = min2;
    //newNode->letter = 0;

    // combine frequencies
    newNode->freq = min1->freq + min2->freq;
    // printf("F:%d L:%d R:%d\n",newNode->freq,newNode->left->freq,newNode->right->freq);

    addEle(newNode);
  }
  return removeMinFreq();
}

// function for filling struct freqs on decompression
void fillStruct(char* inFile){
  FILE *fpt;
  int i;
  setStruct();

  fpt=fopen(inFile,"rb");
  if (fpt == NULL)
  {
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }
  totalFreq = 0;
  for(i=0;i<256;i++){
    fread(&frequency[i],1,sizeof(unsigned int),fpt);
    //printf("freq[%d]:%d\n",i,frequency[i]);
    totalFreq += frequency[i];
    alphabetUnsorted[i].freq   = frequency[i];
    alphabetUnsorted[i].letter = i;
    alphabetUnsorted[i].left   = NULL;
    alphabetUnsorted[i].left   = NULL;
  }
  fclose(fpt);
}

void printTable(){
  int i;
  printf("\n----------------------------\n");
  printf("|char| freq | code |lenCode|\n----------------------------\n");
  for(i=0; i<256; i++){
    printf("| %2x |%6u|%6d|%7d|\n",alphabetUnsorted[i].letter, alphabetUnsorted[i].freq, code[i], lenCode[i]);
    alphabetUnsorted[i].freq   = 0;
    alphabetUnsorted[i].letter = i;
  }
  printf("----------------------------\n\n");
}

void traverseBinTree(int lr, int codeToAdd, int len, struct binTree* node){
  if(lr == 0){ // left traverse
    codeToAdd = codeToAdd<<1; // add 0 to end of code
    len++;
  } else if(lr == 1){ // right traverse
    codeToAdd = (codeToAdd<<1)|1;// add 1 to end of code
    len++;
  }
  //printf("codeToAdd:%d\ncodeLength: %d\n",codeToAdd,len);
  //printf("letter: %x\nfreq:%d\n",node->letter,node->freq);

  if(node->left==NULL && node->right == NULL){
    //printf("HERE\n");
    code[node->letter] = codeToAdd;
    lenCode[node->letter] = len;
  } else{
  //printf("\n");
    if(node->left != NULL){
      traverseBinTree(0, codeToAdd, len, node->left);
    }
    if(node->right != NULL){
      traverseBinTree(1, codeToAdd, len, node->right);
    }
  }
}

//============================================================================//
//==============================COMPRESS======================================//
//============================================================================//
void compress(char* inFile, char* outFile){
  struct binTree* root;
  // printf("compressing\n");

  // build binary tree based on frequencies from file
  buildFrequency(inFile);

  // min heapify the frequencies
  root = buildBinaryTree();

  //printf("root: %d\n",root->freq);

  // traverse the tree to create array of char codes
  traverseBinTree(-1,0,0, root); // leftOrRight = -1 on first run, initial code is empty, length of code starts at 0
  printTable();

  encodeData(inFile, outFile);
}

void encodeData(char* inFile, char* outFile){
  FILE         *fpt, *fpt2;
  int           i;
  unsigned char val;      // char = 1 byte
  unsigned char dataBuff = 0; // int  = 1 bytes
  short int     buffSize = 0; // never larger than 8 bits
  unsigned int  data;
  short int     dataLeft;

  fpt  = fopen(inFile,  "rb");
  if (fpt == NULL){
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }

  fpt2 = fopen(outFile, "wb");
  if (fpt2 == NULL){
    printf("Unable to open %s for writing.\n",outFile);
    exit(0);
  }

  // write out frequency data for retrieval on decompress
  for(i=0;i<256;i++){
    fwrite(&frequency[i],1,sizeof(unsigned int),fpt2);
  }
  while(1){
    fread(&val, 1, sizeof(unsigned char), fpt);
    if(feof(fpt)){
      if(buffSize > 0){ // should be unneccessay, but keep in case
        while(buffSize != 8){
          dataBuff = dataBuff<<1;
          buffSize++;
        }
        fwrite(&dataBuff,1,sizeof(unsigned char), fpt2);
        //printf("dataBuff: %02x\n\n", dataBuff);
      }
      break;
    }

    // set data and bits of data required to shift into buffer
    data     = code[val];
    dataLeft = lenCode[val];

    // write out data when buffer is filled
    while(dataLeft > 0){
      dataBuff = (dataBuff<<1) | ((data>>(dataLeft-1)) & 1);
      dataLeft--;
      buffSize++;
      if(buffSize > 7){
        //printf("dataBuff: %02x\n\n", dataBuff);
        fwrite(&dataBuff,1,sizeof(unsigned char), fpt2);
        dataBuff = 0;
        buffSize = 0; // reset buffer values
      }
    }
  }
  fclose(fpt);
  fclose(fpt2);
}
//============================================================================//

//============================================================================//
//==============================DECOMPRESS====================================//
//============================================================================//
void decompress(char* inFile, char* outFile){
  struct binTree* root;

  //printf("decompressing\n");
  fillStruct(inFile);
  root = buildBinaryTree();
  //printf("root: %d\n",root->freq);
  // traverse the tree to create array of char codes
  traverseBinTree(-1,0,0, root); // leftOrRight = -1 on first run, initial code is empty, length of code starts at 0
  // printTable();

  decodeData(inFile,outFile);
}

void decodeData(char* inFile, char* outFile){
  FILE          *fpt, *fpt2;
  int           i, temp;
  unsigned char val      = 0;
  unsigned int  buffData = 0;
  short int     valSize  = 0;
  int           currFreq = 0, found     = 0;
  int           done     = 0, buffSize  = 0;


  fpt  = fopen(inFile,  "rb");
  if (fpt == NULL){
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }

  fpt2 = fopen(outFile, "wb");
  if (fpt2 == NULL){
    printf("Unable to open %s for writing.\n",outFile);
    exit(0);
  }
  // ignore all frequency data
  for(i=0;i<256;i++){
    fread(&temp,1,sizeof(unsigned int),fpt);
  }
  while(!done){
    if(valSize==0){
      fread(&val,1,sizeof(unsigned char),fpt);  //get 8 bits from the compressed file
      //printf("\nchar: %x\n",val);
      if(feof(fpt)){
        //printf("EOF\n");
        break;
      }
      valSize = 8;
    }
    found = 0;
    while(!found && valSize > 0){
      buffData = (buffData<<1)|((val>>7)&1); // shift in highest char bit to buffer
      val <<= 1; // shift out highest bit
      valSize--;
      buffSize++;
      //printf("buffData: %x \nbuffLength: %d\n",buffData, buffSize);
      for(i=0;i<256;i++){
        if(code[i] == buffData && (lenCode[i] == buffSize)){
          //printf("[%x] | C: %x == bD: %x\n",i, code[i], buffData);
          //printf("HERE\n\n");
          found = 1;
          buffData = 0;
          buffSize = 0;
          fwrite(&i,1,sizeof(unsigned char),fpt2);
          //printf("charOUT: %c\n\n",i);
          currFreq++;
          if(currFreq==totalFreq){
            done = 1;
            //printf("totalFreq: %d\n", totalFreq);
            break;
          }
          break;
        }
      }
    }
  }
  fclose(fpt);
  fclose(fpt2);
}

//============================================================================//

//============================================================================//
//==============================TREE HELPER FUNCS=============================//
//============================================================================//
void initializeQueue(){
  sq->queueSize = 0; // initialize sorting queue size (always starts at size 0)
}
int getLeft(int initial){
  return (initial*2)+1;
}
int getRight(int initial){
  return (initial*2)+2;
}
int getParent(int initial){
  return (initial-1)/2;
}
void addEle(struct binTree* node){
  int i;
  i=sq->queueSize;
  sq->queueSize++;
  //printf("QS:%d\n",sq->queueSize);

  while((sq->nodes[getParent(i)]->freq > node->freq) && (i>0)) {
    sq->nodes[i]->freq = sq->nodes[getParent(i)]->freq;
    sq->nodes[i]->letter = sq->nodes[getParent(i)]->letter;
    sq->nodes[i]->left = sq->nodes[getParent(i)]->left;
    sq->nodes[i]->right = sq->nodes[getParent(i)]->right;
    i=getParent(i);
  }
  sq->nodes[i]->freq = node->freq;
  sq->nodes[i]->letter = node->letter;
  sq->nodes[i]->left = node->left;
  sq->nodes[i]->right = node->right;

}

void initializeNodes(){
  int i;

  for(i=0;i<256;i++){
    if(frequency[i] != 0){
      //printf("%d\n",frequency[i]);
      //alphabetUnsorted[i].freq = frequency[i];
      struct binTree node = {frequency[i], (char)i, NULL, NULL};
      //printf("Freq:%d\n", node.freq);
      //addEle(&alphabetUnsorted[i]);
      addEle(&node);
    }
  }
  // printf("PRINTING HEAP:\n");
  // for(i=0;i<sq->queueSize;i++){
  //   printf("[%d] %d ",i, sq->nodes[i]->freq);
  // }
  // printf("\n");
}
void makeHeap(int index){
  int            left, right, min;
  struct binTree *node;
  node = (struct binTree*)malloc(sizeof(struct binTree));

  left  = getLeft (index);
  right = getRight(index);

  // get min of parent and children
  if(sq->nodes[left]->freq < sq->nodes[index]->freq && left < sq->queueSize){ // check left
    min = left;
  } else{
    min = index;
  }
  if(sq->nodes[right]->freq < sq->nodes[min]->freq && right < sq->queueSize){
    min = right;
  }
  // make heap again if parent must be resorted
  if(min != index){
    node->freq   = sq->nodes[index]->freq;
    node->letter = sq->nodes[index]->letter;
    node->left   = sq->nodes[index]->left;
    node->right  = sq->nodes[index]->right;

    sq->nodes[index]->freq   = sq->nodes[min]->freq;
    sq->nodes[index]->letter = sq->nodes[min]->letter;
    sq->nodes[index]->left   = sq->nodes[min]->left;
    sq->nodes[index]->right   = sq->nodes[min]->right;
    sq->nodes[index]->freq   = sq->nodes[min]->freq;

    sq->nodes[min]->freq     = node->freq;
    sq->nodes[min]->letter   = node->letter;
    sq->nodes[min]->left     = node->left;
    sq->nodes[min]->right    = node->right;

    makeHeap(min);
  }
  // printf("PRINTING HEAP:\n");
  // for(int i=0;i<sq->queueSize;i++){
  //   printf("[%d] %d ",i, sq->nodes[i]->freq);
  // }
  // printf("\n");
}
struct binTree* removeMinFreq() {
  struct binTree* node;
  node = (struct binTree*)malloc(sizeof(struct binTree));

  // ensure queueSize is valid
  if(sq->queueSize <= 0){
    printf("something went wrong! queue size might have underflowed.\n");
    exit(0);
  }
  // remove head node
  node->freq   = sq->nodes[0]->freq;
  node->letter = sq->nodes[0]->letter;
  node->left   = sq->nodes[0]->left;
  node->right  = sq->nodes[0]->right;

  // set the end value to the head value to preserve rest of queue
  sq->nodes[0]->freq   = sq->nodes[sq->queueSize-1]->freq;
  sq->nodes[0]->letter = sq->nodes[sq->queueSize-1]->letter;
  sq->nodes[0]->left   = sq->nodes[sq->queueSize-1]->left;
  sq->nodes[0]->right  = sq->nodes[sq->queueSize-1]->right;

  sq->queueSize = sq->queueSize-1;
  makeHeap(0);
  return node;
}
//============================================================================//
