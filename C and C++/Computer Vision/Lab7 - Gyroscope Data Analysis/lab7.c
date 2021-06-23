#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define WINDOW 7
#define RTOD 57.22958 // convert radian to degrees
#define MTOMM 1000
#define GRAV 9.8

int main(int argc, char *argv[]){
  FILE   *fpt;
  double temp, *time, *accx, *accy, *accz, *pitch, *roll, *yaw;
  double win_time[WINDOW], win_accx[WINDOW], win_accy[WINDOW], win_accz[WINDOW], win_pitch[WINDOW], win_roll[WINDOW], win_yaw[WINDOW];
  double sampleMeanx = 0, sampleMeany = 0, sampleMeanz = 0, *sampleVarx, *sampleVary, *sampleVarz;
  double sampleMeanPitch = 0, sampleMeanRoll = 0, sampleMeanYaw = 0, *sampleVarPitch, *sampleVarRoll, *sampleVarYaw;
  double thresholdGyro = .01, thresholdAcc = .005, timediff = 0;
  double avgVelx = 0, avgVely = 0, avgVelz = 0, totalYaw = 0, totalPitch = 0, totalRoll = 0;
  double totalx = 0, totaly = 0, totalz = 0, timeSample = 0, continueMotion = 0;
  double  startTime = 0, endTime = 0;
  int    count=0, i, j, move=0;
  char   tempstr[20];


  /* check args --------------------------------------------------------------*/
  if(argc != 2){
    printf("Usage: ./lab6 [data file]\n");
    exit(0);
  }
  /*--------------------------------------------------------------------------*/

  /* read in data file -------------------------------------------------------*/
  if ((fpt=fopen(argv[1],"r")) == NULL){
    printf("Unable to open %s for reading\n", argv[1]);
    exit(0);
  }
  fscanf(fpt,"%s %s %s %s %s %s %s ",tempstr,tempstr,tempstr,tempstr,tempstr,tempstr,tempstr);
  // get size of data
  while(!feof(fpt)){
    fscanf(fpt,"%lf %lf %lf %lf %lf %lf %lf ",&temp,&temp,&temp,&temp,&temp,&temp,&temp);
    count++;
  }
  time  = (double *)calloc(count,sizeof(double));

  accx  = (double *)calloc(count,sizeof(double));
  accy  = (double *)calloc(count,sizeof(double));
  accz  = (double *)calloc(count,sizeof(double));

  pitch = (double *)calloc(count,sizeof(double));
  roll  = (double *)calloc(count,sizeof(double));
  yaw   = (double *)calloc(count,sizeof(double));


  sampleVarPitch = (double *)calloc(count/WINDOW,sizeof(double));
  sampleVarRoll  = (double *)calloc(count/WINDOW,sizeof(double));
  sampleVarYaw   = (double *)calloc(count/WINDOW,sizeof(double));
  sampleVarx     = (double *)calloc(count/WINDOW,sizeof(double));
  sampleVary     = (double *)calloc(count/WINDOW,sizeof(double));
  sampleVarz     = (double *)calloc(count/WINDOW,sizeof(double));

  fclose(fpt);

  if ((fpt=fopen(argv[1],"r")) == NULL){
    printf("Unable to open %s for reading\n", argv[1]);
    exit(0);
  }
  fscanf(fpt,"%s %s %s %s %s %s %s ",tempstr,tempstr,tempstr,tempstr,tempstr,tempstr,tempstr);
  // get size of data
  for(i=0;i<count;i++){
    fscanf(fpt,"%lf %lf %lf %lf %lf %lf %lf ", &time[i], &accx[i], &accy[i], &accz[i], &pitch[i], &roll[i], &yaw[i]);
  }
  fclose(fpt);
  /*--------------------------------------------------------------------------*/

  // calculate time steps
  timediff   = time[1] - time[0];
  timeSample = time[5] - time[0];

  /* check variance thresholds at window -------------------------------------*/
  printf("Time of Motion\tX\t\tY\t\tZ\t\tpitch\t\troll\t\tyaw\n");
  for(i=0;i<count;i++){

    // on complete window, get variances
    if(i%WINDOW == 0 && i != 0){

      // reset variables for window calculations
      sampleMeanx     = 0;
      sampleMeany     = 0;
      sampleMeanz     = 0;

      sampleMeanPitch = 0;
      sampleMeanRoll  = 0;
      sampleMeanYaw   = 0;

      sampleVarx    [move] = 0;
      sampleVary    [move] = 0;
      sampleVarz    [move] = 0;

      sampleVarPitch[move] = 0;
      sampleVarRoll [move] = 0;
      sampleVarYaw  [move] = 0;

      // sum means
      for(j=0;j<WINDOW;j++){
        sampleMeanx     += win_accx [j];
        sampleMeany     += win_accy [j];
        sampleMeanz     += win_accz [j];

        sampleMeanPitch += win_pitch[j];
        sampleMeanRoll  += win_roll [j];
        sampleMeanYaw   += win_yaw  [j];

      }
      // take mean of sample data
      sampleMeanx     /= WINDOW;
      sampleMeany     /= WINDOW;
      sampleMeanz     /= WINDOW;

      sampleMeanPitch /= WINDOW;
      sampleMeanRoll  /= WINDOW;
      sampleMeanYaw   /= WINDOW;


      // get variance of samples
      for(j=0;j<WINDOW;j++){
        sampleVarx    [move] += pow(win_accx [j] - sampleMeanx,     2);
        sampleVary    [move] += pow(win_accy [j] - sampleMeany,     2);
        sampleVarz    [move] += pow(win_accz [j] - sampleMeanz,     2);

        sampleVarPitch[move] += pow(win_pitch[j] - sampleMeanPitch, 2);
        sampleVarRoll [move] += pow(win_roll [j] - sampleMeanRoll,  2);
        sampleVarYaw  [move] += pow(win_yaw  [j] - sampleMeanYaw,   2);
      }

      // average
      sampleVarx    [move] /= WINDOW-1;
      sampleVary    [move] /= WINDOW-1;
      sampleVarz    [move] /= WINDOW-1;

      sampleVarPitch[move] /= WINDOW-1;
      sampleVarRoll [move] /= WINDOW-1;
      sampleVarYaw  [move] /= WINDOW-1;

      //reset variables
      avgVelx   = 0;
      avgVely   = 0;
      avgVelz   = 0;



      if(sampleVarx[move] > thresholdAcc || sampleVary[move] > thresholdAcc || sampleVarz[move] > thresholdAcc ||
         sampleVarPitch[move] > thresholdGyro || sampleVarRoll[move] > thresholdGyro || sampleVarYaw[move] > thresholdGyro){
           if(continueMotion==0){
             startTime = win_time[0];
           }
           // check if next variance is also included in region
           continueMotion = 1;
           for(j=0;j<WINDOW;j++){
             // integrate gyro data into radians
             win_pitch[j] *= timediff;
             win_roll [j] *= timediff;
             win_yaw  [j] *= timediff;

             // integrate acceleration data into velocities and convert to meters
             win_accx [j] *= GRAV*timediff;
             win_accy [j] *= GRAV*timediff;
             win_accz [j] *= GRAV*timediff;

           }
           // total radians
           for(j=0;j<WINDOW;j++){
             totalPitch += win_pitch[j];
             totalRoll  += win_roll [j];
             totalYaw   += win_yaw  [j];
           }

           // determine average velocities
           avgVelx = (win_accx[WINDOW-1]-win_accx[0])/2;
           avgVely = (win_accy[WINDOW-1]-win_accy[0])/2;
           avgVelz = (win_accz[WINDOW-1]-win_accz[0])/2;

           totalx += (avgVelx * timeSample);
           totaly += (avgVely * timeSample);
           totalz += (avgVelz * timeSample);

      } else if(continueMotion == 1){
        continueMotion=0;
        endTime = win_time[WINDOW-1];
        move++;

        printf("%4.2lfs-%4.2lfs\t%+lf\t%+lf\t%+lf\t%+lf\t%+lf\t%+lf\n",startTime,endTime, totalx*MTOMM, totaly*MTOMM, totalz*MTOMM, totalPitch*RTOD, totalRoll*RTOD, totalYaw*RTOD);

        // rads
        totalPitch = 0;
        totalRoll  = 0;
        totalYaw   = 0;
        // distance
        totalx     = 0;
        totaly     = 0;
        totalz     = 0;
      }
    }
    // set window array values
    win_time [i%WINDOW] = time [i];

    win_accx [i%WINDOW] = accx [i];
    win_accy [i%WINDOW] = accy [i];
    win_accz [i%WINDOW] = accz [i];

    win_pitch[i%WINDOW] = pitch[i];
    win_roll [i%WINDOW] = roll [i];
    win_yaw  [i%WINDOW] = yaw  [i];

  }
  /*--------------------------------------------------------------------------*/

  /*free all pointers --------------------------------------------------------*/
  free(time);
  free(accx);
  free(accy);
  free(accz);
  free(pitch);
  free(roll);
  free(yaw);
  free(sampleVarx);
  free(sampleVary);
  free(sampleVarz);
  free(sampleVarPitch);
  free(sampleVarRoll);
  free(sampleVarYaw);
  /*--------------------------------------------------------------------------*/
}
