#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
  FILE		        *fpt;
  unsigned char	  *image, *template, *detected, *norm_msf;
  char		        header[320], *gt_letter, temp_char;
  int             *zmc_template, *gt_data_r, *gt_data_c, count=0, temp1, temp2, *msf;
  int		          ROWS,COLS,BYTES, ROWS_T, COLS_T, BYTES_T, FP=0, TP=0, FN=0, TN=0;
  int             r, c, r2, c2, i, sum=0, mean=0, threshold, min_val, max_val, T;

  // ensure correct usage
  if(argc != 2){
    printf("Usage: ./a.out [THRESHOLD]\n");
    exit(0);
  }
  threshold = atoi(argv[1]);
  /* read image --------------------------------------------------------------*/
  if ((fpt=fopen("parenthood.ppm","rb")) == NULL){
    printf("Unable to open parenthood.ppm for reading\n");
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

  /* read template -----------------------------------------------------------*/
  if ((fpt=fopen("parenthood_e_template.ppm","rb")) == NULL){
    printf("Unable to open parenthood_e_template.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS_T,&ROWS_T,&BYTES_T);
  if (strcmp(header,"P5") != 0  ||  BYTES_T != 255){
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  template=(unsigned char *)calloc(ROWS_T*COLS_T,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(template,1,COLS_T*ROWS_T,fpt);

  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* read in ground truth ----------------------------------------------------*/
  if ((fpt=fopen("parenthood_gt.txt","r")) == NULL){
    printf("Unable to open parenthood_gt.txt for reading\n");
    exit(0);
  }
  while(!feof(fpt)){
    fscanf(fpt,"%c %d %d ", &temp_char, &temp1, &temp2);
    count++;
  }

  fclose(fpt);

  // initialize ground truth data values
  gt_data_c  = (int *)calloc(count,sizeof(int));
  gt_data_r  = (int *)calloc(count,sizeof(int));
  gt_letter  = (char *)calloc(count,sizeof(char));

  if ((fpt=fopen("parenthood_gt.txt","r")) == NULL){
    printf("Unable to open parenthood_gt.txt for reading\n");
    exit(0);
  }

  // Read in ground truth values
  count=0;
  while(!feof(fpt)){
    fscanf(fpt,"%s %d %d ", &gt_letter[count], &gt_data_c[count], &gt_data_r[count]);
    count++;
  }

  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  zmc_template  = (int *)calloc(ROWS_T*COLS_T,sizeof(int));
  msf           = (int *)calloc(ROWS*COLS,sizeof(int));
  norm_msf      = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

  /* find mean and zero-mean center the template -----------------------------*/
  // find mean
  sum=0;
  for(r=0;r<=ROWS_T-1;r++){
    for(c=0;c<=COLS_T-1;c++){
      sum += template[r*COLS_T+c];
    }
  }
  mean = sum/(ROWS_T*COLS_T);
  // subtract mean from all pixels in the template
  for(r=0;r<=ROWS_T-1;r++){
    for(c=0;c<=COLS_T-1;c++){
      zmc_template[r*COLS_T+c] = template[r*COLS_T+c] - mean;
      printf("%d\n", zmc_template[r*COLS_T+c]);
    }
  }
  /*--------------------------------------------------------------------------*/

  /* convolve msf image ------------------------------------------------------*/
  sum=0;
  min_val = image[0];
  max_val = image[0];

  /* 2D convolution code from lab 1 but with the addition of convolving with
     the template instead of just the image                                   */
  for (r=7; r<=ROWS-8; r++) {
    for (c=4; c<=COLS-5; c++) {
      sum=0;
      for (r2=-(ROWS_T/2); r2<=(ROWS_T/2); r2++){
        for (c2=-(COLS_T/2); c2<=(COLS_T/2); c2++){
          sum += (image[(r+r2)*COLS+(c+c2)]*zmc_template[(r2+7)*COLS_T+(c2+4)]);
        }
      }
      // determine min and max value for normalization
      if(sum >= max_val){
        max_val = sum;
      }
      if(sum <= min_val){
        min_val = sum;
      }
      msf[r*COLS+c] = sum;
    }
  }
  /*--------------------------------------------------------------------------*/
  printf("min:%d\nmax:%d\n",min_val,max_val);
  /* Normalize MSF -----------------------------------------------------------*/
  for (r=0; r<=ROWS-1; r++) {
    for (c=0; c<=COLS-1; c++) {
      // use the normalization formula to normalize msf to an 8-bit image
      norm_msf[r*COLS+c] = (((msf[r*COLS+c]-min_val)*255)/(max_val-min_val));
    }
  }
  /*--------------------------------------------------------------------------*/
  // calloc the detected
  detected = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

  for(T=160;T<=240;T+=10){
    printf("\n+======== Running program at threshold = %d =========+\n",T);

  /* generate detected image -------------------------------------------------*/
    for(r=0;r<=ROWS-1;r++){
      for(c=0;c<=COLS-1;c++){
        // if any pixel is above the threshold, T, mark as detected (255)
        if(norm_msf[r*COLS+c] > T){
          detected[r*COLS+c] = 255;
        } else{
          detected[r*COLS+c] = 0;
        }
      }
    }
  /*--------------------------------------------------------------------------*/

  /* check ground truth file for TP/FP ---------------------------------------*/
    for(i=0; i<count; i++){
      for(r2=-(ROWS_T/2); r2<=(ROWS_T/2);r2++){
        for (c2=-(COLS_T/2); c2<=(COLS_T/2); c2++){
          /* check the 9x15 area around ground truth locations and determine
             if the detection was a TP, FP, TN, or FN and save these values */
          if(detected[(r2+gt_data_r[i])*COLS+(c2+gt_data_c[i])] == 255){
            if (gt_letter[i] == 'e'){
              TP++;
              // break loop conditions for this run
              r2=(ROWS_T/2)+1;
              c2=(COLS_T/2)+1;
            } else{
              FP++;
              // break loop conditions for this run
              r2=(ROWS_T/2)+1;
              c2=(COLS_T/2)+1;
            }
          } else if(r2==(ROWS_T/2) && c2==(COLS_T/2)){
            if (gt_letter[i] == 'e'){
              FN++;
            } else{
              TN++;
            }
          }
        }
      }
    }
    // print out TP,FP,TN,FN for this run
    printf("TP: %d\nFP: %d\nTN: %d\nFN: %d\n", TP, FP, TN, FN);
    // reset all detection values after each cycle
    TP = 0;
    FP = 0;
    TN = 0;
    FN = 0;
    //free(detected);
    printf("-------------------------------------------------------\n");
  }
  /*--------------------------------------------------------------------------*/

  printf("\n+====== Running at user defined threshold = %d ======+\n", threshold);

  /* generate detected image for user defined threshold-----------------------*/
  for(r=0;r<=ROWS-1;r++){
    for(c=0;c<=COLS-1;c++){
      // if any pixel is above the threshold, T, mark as detected (255)
      if(norm_msf[r*COLS+c] > threshold){
        detected[r*COLS+c] = 255;
      }
    }
  }
  /*--------------------------------------------------------------------------*/

  /* check ground truth file for TP/FP at user-defined threshold--------------*/
  for(i=0; i<count; i++){
    for(r2=-(ROWS_T/2); r2<=(ROWS_T/2);r2++){
      for (c2=-(COLS_T/2); c2<=(COLS_T/2); c2++){
        /* check the 9x15 area around ground truth locations and determine
           if the detection was a TP, FP, TN, or FN and save these values */
        if(detected[(r2+gt_data_r[i])*COLS+(c2+gt_data_c[i])] == 255){
          if (gt_letter[i] == 'e'){
            TP++;
            // break loop conditions for this run
            r2=(ROWS_T/2)+1;
            c2=(COLS_T/2)+1;
          } else{
            FP++;
            // break loop conditions for this run
            r2=(ROWS_T/2)+1;
            c2=(COLS_T/2)+1;
          }
        } else if(r2==(ROWS_T/2) && c2==(COLS_T/2)){
          if (gt_letter[i] == 'e'){
            FN++;
          } else{
            TN++;
          }
        }
      }
    }
  }
  printf("TP: %d\nFP: %d\nTN: %d\nFN: %d\n", TP, FP, TN, FN);
  printf("-------------------------------------------------------\n");
  /*--------------------------------------------------------------------------*/

  /* save MSF image ----------------------------------------------------------*/
  fpt=fopen("msf_image.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(norm_msf,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  /* save file for detected images -------------------------------------------*/
  fpt=fopen("detected.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(detected,COLS*ROWS,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

}
