//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Function for encoding and decoding RLE compression/decompression

// compilation command: gcc -o rle lab2.c -g

// helper functions
void compress(char* inFile, char* outFile);
void uncompress(char* inFile, char* outFile);
int  seeFuture(FILE* fpt, unsigned char prevChar, FILE* fpt2);

int main(int argc, char *argv[]) {

  // check number of arguments
  if (argc != 4){
    printf("Usage:  rle [input filename] [compress/uncompress] [output filename]\n");
    printf("NOTE:   -c compress | -u uncompress\n");
  	exit(0);
  }

  // decide to either encode or decode the data
  if(strcmp(argv[2],"-c") == 0){ // encode data
    compress(argv[1],argv[3]);
  } else
  if(strcmp(argv[2],"-u") == 0){ // decode data
    uncompress(argv[1], argv[3]);
  } else{
    printf("Incorrect flag detected!\n");
    printf("NOTE:   -c compress | -u uncompress\n");
  }

  return 0;
}

// function for decoding RLE
void compress(char* inFile, char* outFile){
  FILE            *fpt, *fpt2;
  unsigned char   dataA = '1', dataB = '2';
  unsigned char   writeData[2];
  unsigned char   runCount;
  int             rlecount = 0;
  int             valDiffer,i;
  int overFlow=0;


  /* open data for reading */
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

  // basic compression algorithm
  runCount = 1;
  fread(&dataA,1,sizeof(unsigned char),fpt);
  if(feof(fpt)){
    printf("File contains no data!\n");
    exit(0);
  }
  while(1){
    fread(&dataB,1,sizeof(unsigned char),fpt);
    if(feof(fpt)){
      //printf("%c%c\n",runCount,dataA);
      sprintf(writeData,"%c%c",runCount,dataA);
      fwrite(writeData, 1 , sizeof(writeData), fpt2);
      break;
    }
    //printf("%c%c\n",dataA,dataB);
    if(dataA==dataB){
      // rle should be on
      if(runCount==255){
        //printf("%c%c\n",runCount,dataA);
        sprintf(writeData,"%c%c",runCount,dataA);
        fwrite(writeData, 1 , sizeof(writeData), fpt2);
        dataA=dataB;
        runCount=1;
        overFlow=1;
        //printf("overflowON\n");
      } else{
        overFlow=0;
        runCount++;
      }
    }else{ //TODO add code to turn on/off RLE here
      valDiffer = seeFuture(fpt, dataB,fpt2); // get value of differing chars in the future
      //printf("%c%c\n",runCount,dataA);
      if(valDiffer >= 3){
        //printf("HERE\n");
        if(runCount>1 || overFlow==1 || runCount == 255){
          //printf("overflowON2\n");
          sprintf(writeData,"%c%c",runCount,dataA);
          fwrite(writeData, 1 , sizeof(writeData), fpt2);
        }
        overFlow=0;
        //printf("RLEOFF: %d\n", valDiffer);
        //print out valDiffer number of next chars with rle off
        // turn off RLE
        //sprintf(writeData,"00");
        writeData[0] = 0;
        writeData[1] = 0;
        //printf("%s",writeData);
        fwrite(&writeData[0],1,sizeof(unsigned char),fpt2);
        if(rlecount == 0){
          fwrite(&dataA, 1 , sizeof(dataA), fpt2);
          //printf("%c",dataA);
          //printf("HERE");
        }
        fwrite(&dataB, 1 , sizeof(dataB), fpt2);
        //printf("%c",dataB);
        for(i=0;i<valDiffer;i++){
          fread(&dataB,1,sizeof(dataB),fpt);
          //printf("%c",dataB);
          fwrite(&dataB,1,sizeof(dataB),fpt2);
        }
        fwrite(&writeData[0],1,sizeof(unsigned char),fpt2);
        //printf("\n\n");

        fread(&dataA,1,sizeof(dataA),fpt);
        runCount=1;

        if(feof(fpt)){
          break;
        }
      } else{
        sprintf(writeData,"%c%c",runCount,dataA);
        fwrite(writeData, 1 , sizeof(writeData) , fpt2);
        dataA=dataB;
        runCount=1;
        overFlow=0;
      }
    }
    rlecount = 1;
  }
  fclose(fpt);
  fclose(fpt2);

}

void uncompress(char* inFile, char* outFile){
  FILE            *fpt, *fpt2;
  unsigned char   dataA = '1', dataB = '2';
  unsigned char   i;

  /* open data for reading */
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

  while(1){
    fread(&dataA,1,sizeof(dataA),fpt);
    if(feof(fpt)){
      break;
    }
    //check for RLE off
    if(dataA == 0){
      do{
        fread(&dataA,1,sizeof(dataA),fpt);
        //printf("%d",dataA);
        if(dataA != 0){
          fwrite(&dataA, 1 , sizeof(dataA) , fpt2);
        }
      } while(dataA != 0);
      //printf("\n");
    }else{
      fread(&dataB,1,sizeof(dataB),fpt);
      for(i=0;i<dataA;i++){
        //printf("%c",dataB);
        fwrite(&dataB, 1 , sizeof(dataB) , fpt2);
      }
    }


    if(feof(fpt)){
      break;
    }
  }
  fclose(fpt);
  fclose(fpt2);

}

int seeFuture(FILE* fpt, unsigned char prevChar, FILE* fpt2){
  unsigned char temp1, temp2;
  int numDiff=0;

  // save previous location of filePointer
  long fileLoc = ftell(fpt);

  //fread(&temp1,1,sizeof(unsigned char), fpt);
  temp1 = prevChar;
  if(temp1 != 0){
    while(!feof(fpt)){
      fread(&temp2,1,sizeof(unsigned char),fpt);
      if(temp2 == 0){
        break;
      }
      if(temp1 != temp2){
        temp1 = temp2;
        numDiff++;
      } else{
        break;
        //numSame++;
      }
    }
  }
  numDiff--;


  // return to previous position
  fseek(fpt, fileLoc, SEEK_SET);

  return numDiff;
}
