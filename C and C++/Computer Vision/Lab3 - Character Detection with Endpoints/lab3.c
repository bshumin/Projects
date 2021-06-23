#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]){
  FILE		        *fpt;
  unsigned char	  *image, *template, *norm_msf, *img_cpy, *bin_cpy, *delete;
  char		        header[320], *gt_letter, temp_char;
  int             *gt_data_r, *gt_data_c, count=0, temp1, temp2;
  unsigned char   tempArray[9] = {255, 255, 255, 255, 255, 255, 255, 255, 255};
  int		          ROWS,COLS,BYTES, ROWS_T, COLS_T, BYTES_T, FP=0, TP=0, FN=0, TN=0;
  int             r, c, r2, c2, r3, c3, i, T, j, done;
  int             detectedVal = 0, count2, numEdges=0, CWEdgeChange=0, nesw=0;
  int             branchpts, endpts;


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

  /* read in msf file produced in lab 2 --------------------------------------*/
  if ((fpt=fopen("msf_e.ppm","rb")) == NULL){
    printf("Unable to open msf_e.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255){
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  norm_msf=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(norm_msf,1,COLS*ROWS,fpt);

  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  // calloc the detected
  //detected = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  img_cpy  = (unsigned char *)calloc(ROWS_T*COLS_T,sizeof(unsigned char));
  bin_cpy  = (unsigned char *)calloc(ROWS_T*COLS_T,sizeof(unsigned char));
  delete   = (unsigned char *)calloc(ROWS_T*COLS_T,sizeof(unsigned char));
  /* check ground truth file for TP/FP at user-defined threshold--------------*/
  for(T=160;T<=240;T+=5){
    TP = 0;
    FP = 0;
    TN = 0;
    FN = 0;
    printf("+====== Running at threshold = %d ======+\n", T);
    for(i=0; i<count; i++){
      detectedVal = 0;
      for(r=-(ROWS_T/2); r<=(ROWS_T/2);r++){
        for (c=-(COLS_T/2); c<=(COLS_T/2); c++){
          if(norm_msf[(r+gt_data_r[i])*COLS+(c+gt_data_c[i])] > T){
            detectedVal = 1;
            // immediatley leave for loop
            r=ROWS_T;
            c=COLS_T;
          }
        }
      }
      // handled detected image
      if(detectedVal == 1){
        count2 = 0;
        for(r2=-(ROWS_T/2); r2<=(ROWS_T/2);r2++){
          for (c2=-(COLS_T/2); c2<=(COLS_T/2); c2++){
            img_cpy[count2] = image[(r2+gt_data_r[i])*COLS+(c2+gt_data_c[i])];
            count2++;
          }
        }

        // create binary image out of template
        for(count2=0;count2<COLS_T*ROWS_T;count2++){
          // threshold image at 128
          if(img_cpy[count2] > 128){
            bin_cpy[count2] = 255;
          } else{
            bin_cpy[count2] = 0;
          }
        }

        /*------------------------------------------------------------------*/
        // thin detected template
        /*   -  c  +
         - |t0|t1|t2|
         r |t3|t4|t5|
         + |t6|t7|t8|
        */
        done = 0;
        // continuously thin until cannot thin anymore
        while(done != 1){
          /*reset all reusable values for each letter ------------------------*/
          endpts=0;
          branchpts=0;
          // reset deletion array
          for(j=0;j<COLS_T*ROWS_T;j++){
            delete[j] = 0;
          }
          for(r3=0; r3<ROWS_T;r3++){
            for (c3=0; c3<COLS_T; c3++){
              for(j=0;j<9;j++){
                tempArray[j] = 255;
              }
              numEdges=0;
              CWEdgeChange=0;
              nesw=0;
              if(bin_cpy[r3*COLS_T+c3] == 0){
                tempArray[4] = bin_cpy[r3*COLS_T+c3];
                /* get binary in 3x3 area around point------------------------*/
                if(r3-1 >= 0){
                  tempArray[1] = bin_cpy[(r3-1)*COLS_T+c3];
                  if(c3-1 >= 0){
                    tempArray[0] = bin_cpy[(r3-1)*COLS_T+(c3-1)];
                  }
                  if(c3+1 <= 8){
                    tempArray[2] = bin_cpy[(r3-1)*COLS_T+(c3+1)];
                  }
                }

                if(r3+1 <= 14){
                  tempArray[7] = bin_cpy[(r3+1)*COLS_T+c3];
                  if(c3-1 >= 0){
                    tempArray[6] = bin_cpy[(r3+1)*COLS_T+(c3-1)];
                  }
                  if(c3+1 <= 8){
                    tempArray[8] = bin_cpy[(r3+1)*COLS_T+(c3+1)];
                  }
                }

                if(c3-1 >= 0){
                  tempArray[3] = bin_cpy[r3*COLS_T+(c3-1)];
                }
                if(c3+1 <= 8){
                  tempArray[5] = bin_cpy[r3*COLS_T+(c3+1)];
                }
                /* -----------------------------------------------------------*/

                // determine removal conditions
                // detected edge neighbors
                for(j=0;j<9;j++){
                  if(j!=4){
                    if(tempArray[j] == 0){
                      numEdges++;
                    }
                  }
                }
                //Clockwise number of Edge transitions
                /*   -  c  +
                 - |t0|t1|t2|
                 r |t3|t4|t5|
                 + |t6|t7|t8|
                */
                if(tempArray[0] == 0 && tempArray[1] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[1] == 0 && tempArray[2] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[2] == 0 && tempArray[5] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[5] == 0 && tempArray[8] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[8] == 0 && tempArray[7] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[7] == 0 && tempArray[6] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[6] == 0 && tempArray[3] == 255){
                  CWEdgeChange++;
                }
                if(tempArray[3] == 0 && tempArray[0] == 255){
                  CWEdgeChange++;
                }

                // find branchpts and endpts
                if(CWEdgeChange == 1){
                  endpts++;
                }
                if(CWEdgeChange > 2){
                  branchpts++;
                }

                // check if (N or E) or (W and S) are not edges
                if((tempArray[1]==255||tempArray[5]==255)||(tempArray[3]==255&&tempArray[7]==255)){
                  nesw = 1;
                }

                // check if removal is necessary based on above conditions
                //printf("nesw:%d CW:%d Edge:%d\n",nesw,CWEdgeChange,numEdges);
                if(nesw==1&&CWEdgeChange==1&&numEdges<=6&&numEdges>=2){
                  delete[r3*COLS_T+c3] = 1;
                }
              }
            }
          } // end of thinning
          // thin if needed and break out of loop if not
          done = 1;
          for(j=0;j<COLS_T*ROWS_T;j++){
            if(delete[j]==1){
              bin_cpy[j] = 255;
              done = 0;
            }
          }
        } // end while

        // use branchpts and endpts
        //printf("e: %d b: %d\n", endpts, branchpts);
        if(gt_letter[i] == 'e' && endpts == 1 && branchpts == 1){
          TP++;
        } else {
          FP++;
        }
      // handle if image is not detected
      } else{
        // handle TN FN
        if(gt_letter[i] != 'e'){
          TN++;
        } else{
          FN++;
        }
      } // end of detection check
    } // end of letter read
    printf("TP: %d FP: %d\nTN: %d FN: %d\n",TP, FP, TN, FN);
  } // end of thresholds
  /*--------------------------------------------------------------------------*/

  // /* save MSF image ----------------------------------------------------------*/
  // fpt=fopen("msf_image.ppm","w");
  // fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  // fwrite(norm_msf,COLS*ROWS,1,fpt);
  // fclose(fpt);
  // /*--------------------------------------------------------------------------*/

  /* save file for detected images -------------------------------------------*/
  fpt=fopen("checkBinCpy.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS_T,ROWS_T);
  fwrite(bin_cpy,COLS_T*ROWS_T,1,fpt);
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

}
