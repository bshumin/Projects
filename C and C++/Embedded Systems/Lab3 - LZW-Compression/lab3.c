#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// compile with: gcc -o lzw lab3.c

// functions compression and decompression/dictionary creation
void  buildAlphabet();
void  printDict    ();
void  compress     (char* inFile, char* outFile);
void  decompress   (char* inFile, char* outFile);
int   seeFuture    (FILE* fpt, unsigned char prevChar, FILE* fpt2);
int   checkDict    (unsigned char* checkP, int size);
void  addToDict    (unsigned char* addP, int size);
int   findPattern  (unsigned short int findCode);
// global variables for dictionary
unsigned char       **dictionary;
int                 *length;     // length of pattern at initial point
unsigned short int  dictCount;
unsigned short int  *code;

int main(int argc, char *argv[]) {

  // check number of arguments
  if (argc != 4){
    printf("Usage:  lzw [input filename] [compress/uncompress] [output filename]\n");
    printf("NOTE:   -c compress | -d decompress\n");
  	exit(0);
  }

  buildAlphabet();

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
  return 0;
}

// function to build alphabet from values of 0-255
void buildAlphabet(){
  int i;

  dictionary = (unsigned char**)    calloc(256,sizeof(unsigned char*));
  length     = (int*)               calloc(256,sizeof(int));
  code       = (unsigned short int*)calloc(256,sizeof(unsigned short int));


  for(i=0;i<256;i++){
    dictionary[i]    = (unsigned char*)calloc(1,sizeof(unsigned char));
    dictionary[i][0] = (unsigned char)i;
    length[i]        = 1;
    code[i]          = i;
    dictCount++;
  }
}

void printDict(){
  int  i,j;

  printf("| code| dc | length |\n");
  printf("| ----------------- |\n");
  for(i=0;i<dictCount;i++){
    printf("| %3hu | ", code[i]);
    for(j=0;j<length[i];j++){
      printf("%2x", dictionary[i][j]);
    }
    printf(" | ");
    printf("%3d |\n", length[i]);
  }
}

void compress(char* inFile, char* outFile){
  FILE          *fpt, *fpt2;
  unsigned char c, *p, *pAc;
  int           sizeP  = 0;
  int           inDict = 0;
  int           i;
  int           foundCode;

  // open read and write files
  fpt=fopen(inFile,"rb");
  if (fpt == NULL)
  {
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }
  fpt2=fopen(outFile,"wb");
  if (fpt2 == NULL)
  {
    printf("Unable to open %s for writing.\n",outFile);
    exit(0);
  }

  // initialize pointer for dict compares
  p   = (unsigned char*)calloc(1, sizeof(unsigned char));
  pAc = (unsigned char*)calloc(1, sizeof(unsigned char));
  while(1){
    fread(&c,1,sizeof(unsigned char),fpt);
    if(feof(fpt)){
      foundCode = checkDict(p, sizeP);
      fwrite(&foundCode, 1 , sizeof(unsigned short int), fpt2);
      break;
    }
    if(sizeP==0){
      pAc[0] = c;
      sizeP  = 1;
    } else{
      pAc = (unsigned char*)realloc(pAc,sizeof(unsigned char) * (++sizeP));
      pAc[sizeP-1] = c;
    }

    inDict = checkDict(pAc, sizeP);
    if(inDict!=-1){ // p+c in dictionary already
      p  = (unsigned char*)realloc(p,sizeof(unsigned char)*(sizeP));
      for(i=0;i<sizeP;i++){
        p[i] = pAc[i];
      }
    } else{ // p+c not in dictionary
      foundCode = checkDict(p, sizeP-1);
      fwrite(&foundCode, 1 , sizeof(unsigned short int), fpt2);
      addToDict(pAc,sizeP);
      free(p);
      free(pAc);
      p      = (unsigned char*)calloc(1, sizeof(unsigned char));
      pAc    = (unsigned char*)calloc(1, sizeof(unsigned char));
      p[0]   = c;
      pAc[0] = c;
      sizeP  = 1;
    }
  }
  fclose(fpt);
  fclose(fpt2);
}

void decompress(char* inFile, char* outFile){
  FILE                *fpt, *fpt2;
  unsigned char       *x, y, *xAy;
  unsigned short int  c, p;
  int                 i, inDict;

  // open read and write files
  fpt=fopen(inFile,"rb");
  if (fpt == NULL)
  {
    printf("Unable to open %s for reading.\n",inFile);
    exit(0);
  }
  fpt2=fopen(outFile,"wb");
  if (fpt2 == NULL)
  {
    printf("Unable to open %s for writing.\n",outFile);
    exit(0);
  }
  fread(&c,1,sizeof(unsigned short int),fpt);
  x   = (unsigned char*)calloc(1, sizeof(unsigned char));
  xAy = (unsigned char*)calloc(length[c], sizeof(unsigned char));
  for(i=0;i<length[c];i++){
    xAy[i] = dictionary[c][i];
  }

  fwrite(xAy, 1, sizeof(unsigned char)*length[c], fpt2);
  while(1){
    p = c;
    fread(&c,1,sizeof(unsigned short int),fpt);
    if(feof(fpt)){
      break;
    }
    //printf("\nCode is %hu\n",c);

    inDict = findPattern(c);

    if(inDict > -1){ // if c in dictionary
      fwrite(dictionary[c], 1 , sizeof(unsigned char)*length[c], fpt2);
      x  = (unsigned char*)realloc(x,sizeof(unsigned char)*length[p]);
      for(i=0;i<length[p];i++){
        x[i] = dictionary[p][i];
      }

      y = dictionary[c][0];

      xAy  = (unsigned char*)realloc(xAy,sizeof(unsigned char)*length[p]+1);
      for(i=0;i<length[p];i++){
        xAy[i] = x[i];
      }

      xAy[length[p]] = y;
      addToDict(xAy,length[p]+1);
    }else{ // if c not in dictionary
      x  = (unsigned char*)realloc(x,sizeof(unsigned char)*length[p]);
      for(i=0;i<length[p];i++){
        x[i] = dictionary[p][i];
      }

      y = dictionary[p][0];
      xAy  = (unsigned char*)realloc(xAy,sizeof(unsigned char)*length[p]+1);
      for(i=0;i<length[p];i++){
        xAy[i] = x[i];
      }
      xAy[length[p]] = y;


      fwrite(xAy, 1 , sizeof(unsigned char)*length[p]+1, fpt2);
      addToDict(xAy,length[p]+1);
    }

  }


  fclose(fpt);
  fclose(fpt2);
}

int checkDict(unsigned char* checkP, int size){
  int i;

  // check if value p+c is in dictionary already
  for(i=0;i<dictCount;i++){
    // if in dictionary, retun 1
    if(size == length[i]){
      if(memcmp(checkP,dictionary[i], sizeof(unsigned char)*size)==0){
        return i;
      }
    }
  }

  return -1;
}

void addToDict (unsigned char* addP, int size){
  int i;
  code                  = (unsigned short int*)realloc(code,sizeof(unsigned short int)*(dictCount+1));
  dictionary            = (unsigned char**)realloc(dictionary,sizeof(unsigned char*) * (dictCount+1));
  length                = (int*)realloc(length,sizeof(int)*(dictCount+1));
  dictionary[dictCount] = (unsigned char*)calloc(size,sizeof(unsigned char));

  for(i=0;i<size;i++){
    dictionary[dictCount][i]   = addP[i];
  }
  length[dictCount] = size;
  code[dictCount]   = dictCount;

  dictCount++;
}

int findPattern(unsigned short int findCode){
  unsigned short int i;

  // check if the unsigned short int is in the dictionary already
  for(i=0;i<dictCount;i++){
    if(findCode == code[i]){
      return i;
    }
  }
  // if not found, return -1
  return -1;

}
