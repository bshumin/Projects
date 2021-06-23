
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//#define ROWS	128
//#define COLS	128

#define THRESH 125
#define SQR(x) ((x)*(x))
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define WINDOW 5

/*
**	This routine converts the data in an Odetics range image into 3D
**	cartesian coordinate data.  The range image is 8-bit, and comes
**	already separated from the intensity image.
*/
int ROWS, COLS;
double averagex, averagey, averagez;

void RegionGrow(unsigned char *image,	unsigned char *labels, int r,int c, int paint_over_label, int new_label, int *indices, int *count, double SurfaceNorm[3][128*128]);

int main(int argc,char *argv[]) {
  int	           BYTES,r,c,r2,c2, i;
  char		       header[320];
  double	       cp[7];
  double	       xangle,yangle,dist;
  double	       ScanDirectionFlag,SlantCorrection;
  unsigned char	 RangeImage[128*128], ThreshImage[128*128], *labels;
  double		     P[3][128*128], SurfaceNorm[3][128*128] = {0};
  int            ImageTypeFlag;
  char	         Filename[160],Outfile[160];
  FILE	         *fpt;
  double         ax, ay, az;
  double         bx, by, bz;
  double         cx, cy, cz;
  int            x,  a,  b;
  int            masked, RegionNum=20, RegionSize, *indices, amtRegion=0;



  printf("Enter range image file name:");
  scanf("%s",Filename);
  if ((fpt=fopen(Filename,"rb")) == NULL)
    {
    printf("Couldn't open %s\n",Filename);
    exit(0);
    }

  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255){
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  header[0]=fgetc(fpt);	/* read white-space character that separates header */

  fread(RangeImage,1,128*128,fpt);
  fclose(fpt);

  labels  = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  indices = (int *)calloc(ROWS*COLS,sizeof(int));

  printf("Up(-1), Down(1) or Neither(0)? ");
  scanf("%d",&ImageTypeFlag);


  cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
  cp[1]=32.0;		/* scan time per single pixel in microseconds */
  cp[2]=(COLS/2)-0.5;		/* middle value of columns */
  cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
  cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
  cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
  cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

  cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
  cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

  switch(ImageTypeFlag)
    {
    case -1:		/* Odetics image -- scan direction upward */
      ScanDirectionFlag=-1;
      break;
    case 1:		/* Odetics image -- scan direction downward */
      ScanDirectionFlag=1;
      break;
    default:		/* in case we want to do this on synthetic model */
      ScanDirectionFlag=0;
      break;
    }

  	/* start with semi-spherical coordinates from laser-range-finder: */
  	/*			(r,c,RangeImage[r*COLS+c])		  */
  	/* convert those to axis-independant spherical coordinates:	  */
  	/*			(xangle,yangle,dist)			  */
  	/* then convert the spherical coordinates to cartesian:           */
  	/*			(P => X[] Y[] Z[])			  */

  if (ImageTypeFlag != 3)
    {
    for (r=0; r<ROWS; r++)
      {
      for (c=0; c<COLS; c++)
        {
        SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
        xangle=cp[0]*cp[1]*((double)c-cp[2]);
        yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
  	SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
        dist=(double)RangeImage[r*COLS+c]+cp[6];
        P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
  	+(tan(yangle)*tan(yangle))));
        P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
        P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
        }
      }
    }

  // for(r=0;r<128*128;r++){
  //   printf("P[0][%d]: %lf\n", r, P[0][r]);
  //   printf("P[1][%d]: %lf\n", r, P[1][r]);
  //   printf("P[2][%d]: %lf\n", r, P[2][r]);
  //   printf("\n");
  // }

  for(r=0;r<ROWS;r++){
    for(c=0;c<COLS;c++){
      if(RangeImage[r*COLS+c] < THRESH){
        ThreshImage[r*COLS+c] = 0;
      }
      else {
        ThreshImage[r*COLS+c] = 255;
      }
    }
  }

  //int    count=0;

  // find surface norms
  for(r=3;r<ROWS-3;r++){
    for(c=3;c<COLS-3;c++){
      x = r*COLS+c;
      b = (r+3)*COLS+c;
      a = r*COLS+(c+3);

      ax = P[0][a] - P[0][x];
      ay = P[1][a] - P[1][x];
      az = P[2][a] - P[2][x];

      bx = P[0][b] - P[0][x];
      by = P[1][b] - P[1][x];
      bz = P[2][b] - P[2][x];

      cx = ay*bz-az*by;
      cy = az*bx-ax*bz;
      cz = ax*by-ay*bx;

      SurfaceNorm[0][r*COLS+c] = cx;
      SurfaceNorm[1][r*COLS+c] = cy;
      SurfaceNorm[2][r*COLS+c] = cz;
      //count++;
      // if(count%50 == 0){
      //   printf("A: (%0.2lf, %0.2lf, %0.2lf) B: (%0.2lf, %0.2lf, %0.2lf) \n", ax, ay, az, bx, by, bz);
      //   printf("Cross: (%0.2lf, %0.2lf, %0.2lf)\n", cx, cy, cz);
      //   printf("-----------------------------------------------\n\n");
      // }
    }
  }
  printf("\n\n");
  for(r=2;r<ROWS-2;r++){
    for(c=2;c<COLS-2;c++){
      masked = 0;
      for(r2=-(WINDOW/2);r2<=(WINDOW/2);r2++){
        for(c2=-(WINDOW/2);c2<=(WINDOW/2);c2++){
          // check if any pixels are masked in the range of 5x5
          if(ThreshImage[(r+r2)*COLS+(c+c2)] != 0){
            masked = 1;
          }
          // check if any labels already exist in range of 5x5
          if(labels[r*COLS+c] != 0){
            masked = 1;
          }
        }
      }
      if(masked == 0){
        RegionGrow(RangeImage, labels, r, c, 0, RegionNum, indices, &RegionSize, SurfaceNorm);
        RegionNum += 20;
      }
      if (RegionSize < 20) {	/* erase region (relabel pixels back to 0) */
        for (i=0; i<RegionSize; i++)
          labels[indices[i]]=0;
      } else{
        if(masked==0){
          printf("+++Region %d+++\n", ++amtRegion);
          printf("Pixels in region: %d\nAvgX: %+4.3lf AvgY: %+4.3lf AvgZ: %+4.3lf\n\n",RegionSize, averagex, averagey, averagez);
        }
      }
    }
  }


  sprintf(Outfile,"%s.coords",Filename);
  fpt=fopen(Outfile,"w");
  fwrite(P[0],8,128*128,fpt);
  fwrite(P[1],8,128*128,fpt);
  fwrite(P[2],8,128*128,fpt);
  fclose(fpt);

  fpt=fopen("threshImage.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(ThreshImage,COLS*ROWS,1,fpt);
  fclose(fpt);

  fpt=fopen("testRegion.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(labels,COLS*ROWS,1,fpt);
  fclose(fpt);

}

void RegionGrow(unsigned char *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count,		/* output:  count of pixels painted */
    double SurfaceNorm[3][128*128])
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
//int	average,total;	/* average and total intensity in growing region */
double totalx, totaly, totalz;
double dotP, distA, distB, angle;

*count=0;
if (labels[r*COLS+c] != paint_over_label)
  return;
labels[r*COLS+c]=new_label;
//average=total=(int)image[r*COLS+c];

averagex = totalx = SurfaceNorm[0][r*COLS+c];
averagey = totaly = SurfaceNorm[1][r*COLS+c];
averagez = totalz = SurfaceNorm[2][r*COLS+c];

if (indices != NULL)
  indices[0]=r*COLS+c;
queue[0]=r*COLS+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */
(*count)=1;
while (qt != qh)
  {
  if ((*count)%1 == 0)	/* recalculate average after each pixel joins */
    {
    //average=total/(*count);
    averagex = totalx/(*count);
    averagey = totaly/(*count);
    averagez = totalz/(*count);

    // printf("new avg=%d\n",average);
    }
  for (r2=-1; r2<=1; r2++)
    for (c2=-1; c2<=1; c2++)
      {
      if (r2 == 0  &&  c2 == 0)
        continue;

      if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  || (queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
        continue;

      if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
        continue;

      if(image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] > THRESH)
        continue;

		/* test criteria to join region */
      dotP = (averagex*SurfaceNorm[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]
           + averagey*SurfaceNorm[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]
           + averagez*SurfaceNorm[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]);

      distA = sqrt(pow(averagex,2) + pow(averagey, 2) + pow(averagez, 2));
      distB = sqrt(pow(SurfaceNorm[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2],2) +
              pow(SurfaceNorm[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2],2) +
              pow(SurfaceNorm[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2],2));


      // if(dotP/(distA*distB){
      //
      // }
      angle = acos(dotP/(distA*distB));

      //printf("angle: %lf\ndotP: %lf\ndistA: %lf \ndistB: %lf\n\n",angle, dotP, distA, distB);

      if (sqrt(SQR(angle)) > .8){
        continue;
      }

      labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;

      if (indices != NULL){
        indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      }

      totalx += SurfaceNorm[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
      totaly += SurfaceNorm[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
      totalz += SurfaceNorm[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];

      (*count)++;
      queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  qt=(qt+1)%MAX_QUEUE;
  }
}
