#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function for printing the hex values of data files

// compilation command: gcc -o readChar readChar.c -g

int main(int argc, char *argv[]){
  FILE            *fpt;
  unsigned char   readin;

  // check number of arguments
  if (argc != 2){
    printf("Usage:      readHex [fileName]\n");
    printf("Usage EX:   readHex [data.txt] > [output.txt]\n");
  	exit(0);
  }

  //data = (unsigned char *)calloc(SIZE,sizeof(unsigned char));

  /* open data for writing */
  //printf("+----- BEGIN HEX READ -----+");
  fpt=fopen(argv[1],"rb");
  if (fpt == NULL)
  {
  	printf("Unable to open %s for reading\n",argv[1]);
  	exit(0);
  }
  int count = 0;
  int line = 0;
  printf("%08x | ",line);
  while(!feof(fpt)){
    fread(&readin,1,sizeof(unsigned char),fpt);
    if(!feof(fpt)){
      printf("%1c",readin);

      count++;
      if(count%16 == 0){
        printf(" | %010d",line);
        line+=16;
        printf("\n%08x | ",line);
      } else if(count%4 == 0){
        printf("  ");
      } else{
        printf(" ");
      }
    }
  }
  while(count%16 != 0){
    printf("- ");
    count++;
  }
  printf("| %010d\n",line);

  //printf("\n+----- END HEX READ -----+\n");
  fclose(fpt);

}
