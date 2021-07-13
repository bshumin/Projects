#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 270  // change as needed

// program for making a test file for RLE compression
// compilation command: gcc -Wall -o makeTest makeTest.c -g

int main(int argc, char *argv[]){
  FILE            *fpt;
  int             i;
  unsigned char   data1[] = {'A'};
  unsigned char   data2[] = {'B'};

  // check number of arguments
  if (argc != 3){
    printf("Usage:      makeTest [same/alternate/half] [outputfile.txt]\n");
    printf("Usage EX:   makeTest [-s/-a/-h] [data.txt]\n");
  	exit(0);
  }

  //data = (unsigned char *)calloc(SIZE,sizeof(unsigned char));

  /* open data for writing */
  fpt=fopen(argv[2],"w");
  if (fpt == NULL)
  {
  	printf("Unable to open %s for reading\n",argv[2]);
  	exit(0);
  }

  // decide to either encode or decode the data
  if(strcmp(argv[1],"-s") == 0){ // print same data
    for(i=0;i<SIZE;i++){
      fwrite(data1, sizeof(char), sizeof(data1), fpt);
    }
  } else
  if(strcmp(argv[1],"-a") == 0){ // print alternating data
    for(i=0;i<SIZE;i++){
      if(i%2){
        fwrite(data2,sizeof(char),sizeof(data2), fpt);
      } else{
        fwrite(data1,sizeof(char),sizeof(data1), fpt);
      }
    }
  } else
  if(strcmp(argv[1],"-h") == 0){ // print half/half data
    for(i=0;i<SIZE;i++){
      if(i<(SIZE/2)){
        fwrite(data1,sizeof(char),sizeof(data1), fpt);
      } else{
        fwrite(data2,sizeof(char),sizeof(data2), fpt);
      }
    }
  } else{
    printf("Incorrect flag detected!\n");
    printf("Usage:      makeTest [same/alternate] [outputfile.txt]\n");
    printf("Usage EX:   makeTest [-s/-a] [data.txt]\n");
    exit(0);
  }

  fclose(fpt);
}
