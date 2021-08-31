
	/*
	** This program reads bridge.ppm, a 512 x 512 PPM image.
	** It smooths it using 3 different convolutional methods.
	** The program also times the average execution time of the
  ** methods over 10 runs.
	**
	** To compile, must link using -lrt  (man clock_gettime() function).
  ** Compile command: gcc lab1.c -Wall -lrt
	*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// define how many runs of the program should occur for averaging run-time
#ifndef RUN
#define  RUN 10
#endif

int main(void){
  FILE		        *fpt;
  unsigned char	  *image;
  unsigned char	  *smoothed1, *smoothed2, *smoothed3;
  int             *tempSum;
  char		        header[320];
  int		          ROWS,COLS,BYTES;
  int		          r,c,r2,c2,sum,run,avg1=0,avg2=0,avg3=0;
  struct timespec	tp1,tp2;

  /* read image */
  if ((fpt=fopen("bridge.ppm","rb")) == NULL){
    printf("Unable to open bridge.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255){
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

  	/* allocate memory for smoothed versions of image */
  smoothed1  = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  smoothed2  = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  smoothed3  = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  tempSum    = (int *)calloc(ROWS*COLS,sizeof(int));
  //truncation = (float *)calloc(ROWS*COLS,sizeof(float));
  /* run the convolutions/smoothing operations 10 times (defined as RUN)
     for averaging */
  for(run=0;run<RUN;run++){
    // formatting for outputting the run number with averages displayed below
    printf("\nRun %d: --------------------------------------------------",run+1);
    if(run < 9){
      printf("-\n");
    } else {
      printf("\n");
    }
    /* start of method 1 -------------------------------------------------------*/
    // query timer for method 1
    clock_gettime(CLOCK_REALTIME,&tp1);
    //printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

    /* method 1, 2D convolution with 7x7 area */
    for (r=3; r<=ROWS-4; r++) {
      for (c=3; c<=COLS-4; c++) {
        sum=0;
        for (r2=-3; r2<=3; r2++){
          for (c2=-3; c2<=3; c2++){
            sum+=image[(r+r2)*COLS+(c+c2)];
          }
          smoothed1[r*COLS+c]=sum/49;
        }
      }
    }
    /* query timer for method 1*/
    clock_gettime(CLOCK_REALTIME,&tp2);
    //printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

    /* report how long it took to smooth */
    // account for if the time is inaacurate due to the second changing
    if(tp2.tv_nsec-tp1.tv_nsec >= 0) {
      printf("2D convolution run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec)/1e9);
      avg1+=tp2.tv_nsec-tp1.tv_nsec;
    } else {
      printf("2D convolution run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec+1e9)/1e9);
      avg1+=tp2.tv_nsec-tp1.tv_nsec+1e9;
    }
    /*end of method 1-----------------------------------------------------------*/

    /*start of method 2---------------------------------------------------------*/
    // query timer for method 2
    clock_gettime(CLOCK_REALTIME,&tp1);

    /* method 2, separable filters of 7x1 and 1x7 */

    //7x1 filter
    for(r=3; r<=ROWS-4; r++) {
      for(c=0; c<=COLS-1; c++){
        sum=0;
        for(r2=-3; r2<=3; r2++){
          sum+=image[(r+r2)*COLS+c];
        }
        tempSum[r*COLS+c]=sum;
      }
    }

    //1x7 filter
    for(r=3;r<=ROWS-4;r++){
      for(c=3;c<=COLS-4;c++){
        sum=0;
        for(c2=-3;c2<=3;c2++){
          sum+=tempSum[r*COLS+(c+c2)];
        }
        smoothed2[r*COLS+c]=sum/49;
      }
    }
    /* query timer for method 2*/
    clock_gettime(CLOCK_REALTIME,&tp2);

    /* report how long it took to smooth */
    // account for if the time is inaacurate due to the second changing
    if(tp2.tv_nsec-tp1.tv_nsec >= 0) {
      printf("Separable filters run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec)/1e9);
      avg2+=tp2.tv_nsec-tp1.tv_nsec;
    } else {
      printf("Separable filters run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec+1e9)/1e9);
      avg2+=tp2.tv_nsec-tp1.tv_nsec+1e9;
    }
    /*end of method 2-----------------------------------------------------------*/

    /*start of method 3---------------------------------------------------------*/
    // query timer for method 3
    clock_gettime(CLOCK_REALTIME,&tp1);

    /* method 3, sliding windows with separable filters */

    //7x1 filter
    for(r=3; r<=ROWS-4; r++) {
      for(c=0; c<=COLS-1; c++){
        sum=0;
        for(r2=-3; r2<=3; r2++){
          sum+=image[(r+r2)*COLS+c];
        }
        tempSum[r*COLS+c]=sum;
      }
    }

    //1x7 filter
    for(r=3;r<=ROWS-4;r++){
      for(c=3;c<=COLS-4;c++){
        if(c==3){
          sum=0;
          for(c2=-3;c2<=3;c2++){
            sum+=tempSum[r*COLS+(c+c2)];
          }
          smoothed3[r*COLS+c]=sum/49;
        } else {
          // sliding window is implemented here
          sum=sum-tempSum[r*COLS+c-4]+tempSum[r*COLS+c+3];
          smoothed3[r*COLS+c]=sum/49;
        }
      }
    }
    /* query timer for method 3*/
    clock_gettime(CLOCK_REALTIME,&tp2);

    /* report how long it took to smooth */
    // account for if the time is inaacurate due to the second changing
    if(tp2.tv_nsec-tp1.tv_nsec >= 0) {
      printf("Sliding window with separable filters run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec)/1e9);
      avg3+=tp2.tv_nsec-tp1.tv_nsec;
    } else {
      printf("Sliding window with separable filters run time: %fs\n",(tp2.tv_nsec-tp1.tv_nsec+1e9)/1e9);
      avg3+=tp2.tv_nsec-tp1.tv_nsec+1e9;
    }
    /*end of method 3-----------------------------------------------------------*/

    // formatting for showing end of run
    printf("----------------------------------------------------------\n");
  }
  // formatting for printing a=out averages of run
  printf("\n+=================== Average Runtimes ===================+\n");
  printf("Average for 2D convolution run-time: %fs\n",(double) (avg1/RUN)/1e9);
  printf("Average for separable filters run-time: %fs\n",(double) (avg2/RUN)/1e9);
  printf("Average for sliding window run-time: %fs\n",(double) (avg3/RUN)/1e9);
  printf("+========================================================+\n");

  /* write out 2DConvolution image to see result */
  fpt=fopen("2DConvolution.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed1,COLS*ROWS,1,fpt);
  fclose(fpt);

  /* write out SeparableFilters image to see result */
  fpt=fopen("SeparableFilters.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed2,COLS*ROWS,1,fpt);
  fclose(fpt);

  /* write out SlidingWindow image to see result */
  fpt=fopen("SlidingWindow.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed3,COLS*ROWS,1,fpt);
  fclose(fpt);
}
