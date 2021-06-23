#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define WINDOW 7

int main(int argc, char *argv[]){
  FILE		        *fpt;
  unsigned char	  *image, *norm_ext, *origContour, *finalContour;
  char		        header[320];
  int		          ROWS,COLS,BYTES, run, r2, c2;
  int             r, c, i, lenPts=0, temp1, temp2, *colsCont, *rowsCont;
  float           *intEnergy1, *intEnergy2, *extEnergy1, *extEnergy2, avgDist=0,*sumEnergy, sum2, min_val, max_val, min_val2, max_val2, sum;
  float           *norm_int1, *norm_int2, *use_ext;
  int             nextPtx=0, nextPty=0, tempRow, tempCol;
  int             xsobel7x7[49] = {-3,   -2,   -1,   0,   1,  2,  3,
                                   -4,   -3,   -2,   0,   2,  3,  4,
                                   -5,   -4,   -3,   0,   3,  4,  5,
                                   -6,   -5,   -4,   0,   4,  5,  6,
                                   -5,   -4,   -3,   0,   3,  4,  5,
                                   -4,   -3,   -2,   0,   2,  3,  4,
                                   -3,   -2,   -1,   0,   1,  2,  3};

int               ysobel7x7[49] = { 3,    4,    5,   6,   5,  4,  3,
                                    2,    3,    4,   5,   4,  3,  2,
                                    1,    2,    3,   4,   3,  2,  1,
                                    0,    0,    0,   0,   0,  0,  0,
                                   -1,   -2,   -3,  -4,  -3, -2, -1,
                                   -2,   -3,   -4,  -5,  -4, -3, -2,
                                   -3,   -4,   -5,  -6,  -5, -4, -3};

  /* check args --------------------------------------------------------------*/
  if(argc != 3){
    printf("Usage: ./lab5 [image file] [contour list]\n");
    exit(0);
  }
  /*--------------------------------------------------------------------------*/

  /* read image --------------------------------------------------------------*/
  if ((fpt=fopen(argv[1],"rb")) == NULL){
    printf("Unable to open %s for reading\n", argv[1]);
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
  /*--------------------------------------------------------------------------*/

  /* read contour points -----------------------------------------------------*/
  if ((fpt=fopen(argv[2],"r")) == NULL){
    printf("Unable to open %s for reading\n", argv[2]);
    exit(0);
  }

  while(!feof(fpt)){
    fscanf(fpt,"%d %d ", &temp1, &temp2);
    lenPts++;
  }
  fclose(fpt);

  if ((fpt=fopen(argv[2],"r")) == NULL){
    printf("Unable to open %s for reading\n", argv[2]);
    exit(0);
  }
  colsCont = (int*)calloc(lenPts,sizeof(int));
  rowsCont = (int*)calloc(lenPts,sizeof(int));

  for(i=0;i<lenPts;i++){
    fscanf(fpt,"%d %d ", &colsCont[i], &rowsCont[i]);
  }

  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* plot contour points -----------------------------------------------------*/
  origContour=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  for(r=0;r<ROWS;r++){
    for(c=0;c<COLS;c++){
      origContour[r*COLS+c] = image[r*COLS+c];
    }
  }

  for(i=0;i<lenPts;i++){
    for(r=-3;r<=3;r++){
      for(c=-3;c<=3;c++){
        if(r==0||c==0)
          origContour[(rowsCont[i]+r)*COLS+(colsCont[i]+c)] = 0;
      }
    }
  }
  /*--------------------------------------------------------------------------*/

  /* find external energy/ Sobel convolution ---------------------------------*/
  //sum=0;
  extEnergy1  = (float *)calloc(ROWS*COLS,sizeof(float));
  extEnergy2  = (float *)calloc(ROWS*COLS,sizeof(float));
  norm_ext    = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

  //x-axis sobel
  for (r=3; r<ROWS-3; r++) {
    for (c=3; c<COLS-3; c++) {
      sum=0;
      sum2=0;
      for (r2=-3; r2<=3; r2++){
        for (c2=-3; c2<=3; c2++){
          sum  += image[(r+r2)*COLS+(c+c2)]*xsobel7x7[(r2+3)*WINDOW+(c2+3)];
          sum2 += image[(r+r2)*COLS+(c+c2)]*ysobel7x7[(r2+3)*WINDOW+(c2+3)];
        }
      }
      extEnergy1[r*COLS+c] = sum;
      extEnergy2[r*COLS+c] = sum2;
    }
  }

  // get min/max
  for (r=0; r<=ROWS-1; r++) {
    for (c=0; c<=COLS-1; c++) {
      if(r==0 && c == 0){
        min_val = sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2));
        max_val = sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2));
      }

      if(sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2)) < min_val){
        min_val = sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2));
      }
      if(sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2)) > max_val){
        max_val = sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2));
      }
    }
  }

  for (r=0; r<=ROWS-1; r++) {
    for (c=0; c<=COLS-1; c++) {
      // use the normalization formula to normalize msf to an 8-bit image
      norm_ext[r*COLS+c] = (((sqrt(pow(extEnergy1[r*COLS+c],2) + pow(extEnergy2[r*COLS+c],2))-min_val)*255)/(max_val-min_val));
      //printf("%d\n",norm_ext[r*COLS+c]);
    }
  }
  /*--------------------------------------------------------------------------*/
  /*keep contouring until done------------------------------------------------*/

  for(run=0;run<30;run++){
    avgDist = 0;
    // get avg distance for internal energy calc
    for(i=0;i<lenPts;i++){
      // check final point to initial poiunt since the contour connects x=c
      if(i==lenPts-1){
        nextPtx = colsCont[0];
        nextPty = rowsCont[0];
      } else{
        nextPtx = colsCont[i+1];
        nextPty = rowsCont[i+1];
      }

      avgDist += sqrt(pow((colsCont[i]-nextPtx),2) + pow((rowsCont[i]-nextPty),2));
    }

    avgDist /= lenPts;

    for(i=0;i<lenPts;i++){
      intEnergy1 = (float *)calloc(WINDOW*WINDOW,sizeof(float));
      intEnergy2 = (float *)calloc(WINDOW*WINDOW,sizeof(float));
      sumEnergy  = (float *)calloc(WINDOW*WINDOW,sizeof(float));
      norm_int1  = (float *)calloc(WINDOW*WINDOW,sizeof(float));
      norm_int2  = (float *)calloc(WINDOW*WINDOW,sizeof(float));
      use_ext    = (float *)calloc(WINDOW*WINDOW,sizeof(float));

      if(i==lenPts-1){
        nextPtx = colsCont[0];
        nextPty = rowsCont[0];
      } else{
        nextPtx = colsCont[i+1];
        nextPty = rowsCont[i+1];
      }

      //check internal energies
      for(r=-3;r<=3;r++){
        for(c=-3;c<=3;c++){
          intEnergy1[(r+3)*WINDOW+(c+3)] = pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2);
          if(r==-3&&c==-3){
            min_val = pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2);
            max_val = pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2);
          }

          if(pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2) > max_val){
            max_val = pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2);
          }
          if(pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2) < min_val){
            min_val = pow(((colsCont[i]+c)-nextPtx),2)+pow(((rowsCont[i]+r)-nextPty),2);
          }

          intEnergy2[(r+3)*WINDOW+(c+3)] = pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2);
          if(r==-3&&c==-3){
            min_val2 = pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2);
            max_val2 = pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2);
          }

          if(pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2) > max_val2){
            max_val2 = pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2);
          }
          if(pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2) < min_val2){
            min_val2 = pow((avgDist-sqrt(intEnergy1[(r+3)*WINDOW+(c+3)])),2);
          }
        }
      }

      // normalize internal energies
      for (r=0; r<WINDOW; r++) {
        for (c=0; c<WINDOW; c++) {
          norm_int1[r*WINDOW+c] = ((intEnergy1[r*WINDOW+c]-min_val))/(max_val-min_val);
          norm_int2[r*WINDOW+c] = ((intEnergy2[r*WINDOW+c]-min_val2))/(max_val2-min_val2);

          //printf(" %0.2f\n",norm_int1[r*WINDOW+c]);
          //printf("%0.2f\n",norm_int2[r*WINDOW+c]);
        }
      }

      // get external energy for point
      for (r=-3; r<=3; r++) {
        for (c=-3; c<=3; c++) {
          use_ext[(r+3)*WINDOW+(c+3)] = ((float)norm_ext[(rowsCont[i]+r)*COLS+(colsCont[i]+c)]/255);
          use_ext[(r+3)*WINDOW+(c+3)] = sqrt(pow((use_ext[(r+3)*WINDOW+(c+3)]-1),2));
          //printf(" %0.2f\n",use_ext[r*WINDOW+c]);
          //printf(" %0.2f\n",(float)norm_ext[(rowsCont[i]+r)*COLS+(colsCont[i]+c)]);
        }
      }

      tempRow = 0;
      tempCol = 0;
      // sum all energies, find min, and move to that point
      for (r=0; r<WINDOW; r++) {
        for (c=0; c<WINDOW; c++) {
          sumEnergy[r*WINDOW+c] = use_ext[r*WINDOW+c] + norm_int1[r*WINDOW+c] + norm_int2[r*WINDOW+c];
          if(r==0&&c==0){
            min_val = sumEnergy[r*WINDOW+c];
            tempRow = rowsCont[i] + (-3+r);
            tempCol = colsCont[i] + (-3+c);
          }

          if(sumEnergy[r*WINDOW+c] < min_val){
            min_val = sumEnergy[r*WINDOW+c];
            tempRow = rowsCont[i] + (-3+r);
            tempCol = colsCont[i] + (-3+c);
          }
        }
      }

      rowsCont[i] = tempRow;
      colsCont[i] = tempCol;

    }
  }
  /*--------------------------------------------------------------------------*/

  /* plot final contour points -----------------------------------------------*/
  finalContour=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  for(r=0;r<ROWS;r++){
    for(c=0;c<COLS;c++){
      finalContour[r*COLS+c] = image[r*COLS+c];
    }
  }

  for(i=0;i<lenPts;i++){
    for(r=-3;r<=3;r++){
      for(c=-3;c<=3;c++){
        if(r==0||c==0)
          finalContour[(rowsCont[i]+r)*COLS+(colsCont[i]+c)] = 0;
      }
    }
  }
  /*--------------------------------------------------------------------------*/

  /* print out final contour points ------------------------------------------*/
    printf("+------FINAL CONTOUR POINTS------+\n");
    for(i=0;i<lenPts;i++){
      printf("Point %2.0d: %d %d\n",i+1, colsCont[i],rowsCont[i]);
    }
  /*--------------------------------------------------------------------------*/


  /* save image with initial contour points ----------------------------------*/
  fpt=fopen("origContour.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(origContour,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* save image with initial contour points ----------------------------------*/
  fpt=fopen("finalContour.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(finalContour,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* save original image -----------------------------------------------------*/
  fpt=fopen("origImage.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(image,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* save original image -----------------------------------------------------*/
  fpt=fopen("sobelImage.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(norm_ext,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* free all pointers -------------------------------------------------------*/
  free(image);
  free(norm_ext);
  free(origContour);
  free(finalContour);
  free(norm_int1);
  free(norm_int2);
  free(use_ext);
  free(colsCont);
  free(rowsCont);
  free(intEnergy1);
  free(intEnergy2);
  free(extEnergy1);
  free(extEnergy2);
  free(sumEnergy);
  /*--------------------------------------------------------------------------*/
}
