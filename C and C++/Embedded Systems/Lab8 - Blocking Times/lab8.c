#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// compile command: gcc -o schedule lab8.c -Wall -g -lm

// number of tasks
#define TASKS 8
//Overhead value 156 ns
#define OH .153  // convert microseconds to milliseconds


int main(void){
  int i,j, k, l;
  //period for each priority (ms)
  float period[TASKS]   = {10.56, 40.96, 61.44, 100.00, 165.00, 285.00, 350.00, 700.00};
  //runTime for each priority (ms)
  float runTime[TASKS]  = { 1.30,  4.70, 9.00,  23.00,  38.30,  10.00,   3.00,   2.00};
  //total Max Blocking for each Priority (ms)
  float maxBlock[TASKS] = { 3.30,  3.30,  9.30,   9.20,   5.20,   5.20,   2.00,   0.00};

  // summation segment of the RMA theorem
  float sum = 0;
  // l*Tk value for the theorem comparison
  float comp = 0;
  // implement RMA theorem from slides
  for(i=1;i<=TASKS;i++){
    for(k=1;k<=i;k++){
      for(l=1;l<=floor(period[i-1]/period[k-1]);l++) {
        sum=0;
        for(j=1;j<=i-1;j++){
          sum += (runTime[j-1] + OH)*ceil(l*period[k-1]/period[j-1]);
        }
        // add run time and total maxBlocking
        sum += runTime[i-1] + maxBlock[i-1];
        // set comparison value
        comp = l*period[k-1];

        //print statement if passes comparison
        if(sum <= comp){
          printf("THEOREM PASSES FOR %d | (%d,%d)\n", i, k, l);
        }

      }
    }
  }
  return 1;
}
