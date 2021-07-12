/*
    Brandon Shumin
    Project 4
    ECE 2220
    March 10th, 2019
    Program Description: This program reads in data from 2 text files and
    stores them to structures. Then, these structures are used to convolve the
    data. Then, the convolved data is printed to an output file given in the
    execution line of the program. After compiling the program should be run as
    follows:

    ./prog4 xDataFile.txt hDataFile.txt OutputFile.txt
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

// Structure defined in program specifications to store data from infile
struct TData
{ int MinIndex, MaxIndex, Points;
  float *pValue;
} Input[2], Output;

// Function to read in file data to relevant structure
void fileRead(struct TData *inputData, char *fileOpen);

// Function to reflect the values in the index of x
void reflect(struct TData *xData);

// Function to convolve data from infiles
void convolve(struct TData xData, struct TData hData, struct TData *outData);

// Function to write file data to output file
void fileWrite(struct TData *outData, char *fileOut);

int main(int argc, char *argv[]) {

  // read in file data from x data and h data
  fileRead(&Input[0], argv[1]); // xdata
  fileRead(&Input[1], argv[2]); // hdata

  // reflext x data
  reflect(&Input[0]);

  // convolve data
  convolve(Input[0], Input[1], &Output);

  // print data to output file
  fileWrite(&Output, argv[3]);

  return 0;
}

void fileRead(struct TData *inputData, char *fileOpen) {
  int count, minInd, lastInd;
  FILE *infile;
  float *dataArr, trash;

  // open infile and error if file is unreadable, empty, or undefined
  infile = fopen(fileOpen, "r");
  if (infile == NULL) {
    fprintf(stderr, "ERROR: Cannot read designated file, exitting.\n");
    exit(-1); //Terminate program
  }

  // get length of file input and allocate memory based on length
  count = 0;
  while(!feof(infile)) {
    count++;
    fscanf(infile, " %i %f ", &lastInd, &trash);
  }

  //set file stream back to file beginning
  rewind(infile);

  // data allocation
  inputData -> pValue = malloc((count+1) * sizeof(float));
  dataArr = malloc((count+1) * sizeof(float));

  // read in data
  count = 0;
  while(!feof(infile)) {
    fscanf(infile, " %i %f ", &lastInd, &dataArr[count]);
    // set min index at first value point
    if (count == 0) {
      minInd = lastInd;
    }
    count++;
  }

  // store data points values, min and max, and number of data points (count)
  inputData -> pValue = dataArr;
  inputData -> Points = count;
  inputData -> MinIndex = minInd;
  inputData -> MaxIndex = lastInd;
  // close file
  fclose(infile);
}

void reflect(struct TData *xData) {
  int sizeDat = 0, i = 0, tempDat = 0;

  // Define the size of the data index
  sizeDat = xData -> Points;

  // Reflect data until halfway point is reached
  while((sizeDat - i) > (sizeDat/2)) {
    tempDat = xData -> pValue[i];
    xData -> pValue[i] = xData -> pValue[sizeDat-i-1];
    xData -> pValue[sizeDat-i-1] = tempDat;
    i++;
  }

  // Reverse and negate max and min indexes for xData to reflect max and min
  tempDat = xData -> MaxIndex;
  xData -> MaxIndex = -(xData -> MinIndex);
  xData -> MinIndex = -tempDat;
}

void convolve(struct TData xData, struct TData hData, struct TData *outData) {
  int m = 0, n = 0, sizeDat = 0, count = 0, numZero = 0;
  float sum = 0, epsilon = 0.000001;

  // Define size of output data (zero values on x and h data ends reduce size)
  (xData.pValue[0] < epsilon||hData.pValue[0] < epsilon) ? numZero++ : numZero;
  (xData.pValue[xData.Points - 1] < epsilon || hData.pValue[hData.Points - 1]
    < epsilon) ? numZero++ : numZero;

  // size is sum of each data set's index minus the number of zero indexes
  sizeDat = xData.Points + hData.Points - numZero;
  outData->Points = sizeDat;
  outData->pValue = malloc((sizeDat)*sizeof(float));
  outData->MinIndex = hData.MinIndex;
  outData->MaxIndex = hData.MinIndex + sizeDat;
  // Define lowest value of n to begin convolution
  n = hData.MinIndex;
  // Convolve h and x data from n < minX + minH to n > maxX + maxH
  count = 0;
  while(count <= sizeDat) {
    // set sum back to zero on each pass
    sum = 0;

    // define where the convolution index should start from (minimum index of h)
      m = hData.MinIndex;

    // depending on the position of the x data over the h data, perform
    // convolution based going to either the hData max or the xData max
    if (xData.MaxIndex > hData.MaxIndex) {
      for( ;m <= hData.MaxIndex; m++) {
        sum += (hData.pValue[m-hData.MinIndex] * xData.pValue[n-m]);
      }
    } else{
      for( ;m <= xData.MaxIndex; m++) {
        sum += (hData.pValue[m-hData.MinIndex] * xData.pValue[n-m]);
      }
    }

    // increment count and xData indices for loops in while
    count++;
    xData.MaxIndex++;
    xData.MinIndex++;

    // store outvalues based on convolved summation
    outData->pValue[sizeDat-count] = sum;
    n++;
  }
}

void fileWrite(struct TData *outData, char *fileOut) {
  FILE *outfile;
  int i;

  // open user designated file for printing output
  outfile = fopen(fileOut, "w");
  if (outfile == NULL) {
    fprintf(stderr, "ERROR: Cannot open designated file for printing output.");
    exit(-1);
  }
  //  print convolved data to outfile
  for(i = 0; i <= outData->Points; i++) {
    fprintf(outfile, "%d %.1f\n", (outData->MinIndex), outData->pValue[i]);
    (outData->MinIndex)++;
  }
  fclose(outfile);
}
