/*
    Brandon Shumin
    Project 6
    ECE 2220
    April 12th, 2019
    Program Description: This program takes in a .bmp image, 3 integer values
    all between -200 and 200, and an output image file name. The program then
    adds the 3 integers to each pixel in the image in RGB format to alter the
    shading of the original image. Then, another image is created by using a
    pre-defined matrix as an "edge detector" and creating an edge altered image.
    The two created images are output as the given output file name followed by
    "(shade).bmp" and "(edge).bmp" respectively.

*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct HEADER
{  unsigned short int Type;                 /* Magic identifier       */
   unsigned int Size;                       /* File size in bytes     */
   unsigned short int Reserved1, Reserved2;
   unsigned int Offset;                     /* Offset to data (in B)  */
};                                          /* -- 14 Bytes --         */

struct INFOHEADER
{  unsigned int Size;                       /* Header size in bytes    */
   int Width, Height;                       /* Width / Height of image */
   unsigned short int Planes;               /* Number of colour planes */
   unsigned short int Bits;                 /* Bits per pixel          */
   unsigned int Compression;                /* Compression type        */
   unsigned int ImageSize;                  /* Image size in bytes     */
   int xResolution, yResolution;            /* Pixels per meter        */
   unsigned int Colors;                     /* Number of colors        */
   unsigned int ImportantColors;            /* Important colors        */
};                                          /* -- 40 Bytes --          */

struct PIXEL {
  unsigned char Red, Green, Blue;
};

// function to make sure input parameters follow requirements
void checkInput(int argc, char *argv[6]);

// read in image data from an input file
struct PIXEL** readIn(struct HEADER* Header, struct INFOHEADER* InfoHeader,
  struct PIXEL** Pixel, char* fileName);

// create a shaded image based on the 3 given integer arguments
struct PIXEL** shadeImage(struct PIXEL** pixelShade, struct INFOHEADER* InfoHeader,
  int red, int green, int blue);

// create a edge image based on a pre-defined matrix used as an edge detector
struct PIXEL** edgeImage(struct PIXEL** pixelEdge, struct PIXEL** Pixel,
  struct INFOHEADER* InfoHeader);

// output the two created images based on the designated output file name
void outImage(struct HEADER* Header, struct INFOHEADER* InfoHeader,
  struct PIXEL** pixelShade, struct PIXEL** pixelEdge, char* fileName);

int main(int argc, char *argv[]){
  int numArg3 = atoi(argv[3]), numArg4 = atoi(argv[4]), numArg5 = atoi(argv[5]);
  int i, j;
  struct HEADER Header;
  struct INFOHEADER InfoHeader;
  struct PIXEL** Pixel = NULL;
  struct PIXEL** pixelShade = NULL;
  struct PIXEL** pixelEdge = NULL;

  // check if inputs are correct
  checkInput(argc, argv);

  // read in data to Pixel dounle pointer and return double pointer
  Pixel = readIn(&Header, &InfoHeader, Pixel, argv[1]);

  //allocate space for pixel shade pointer
  pixelShade = (struct PIXEL**)calloc(InfoHeader.Height, sizeof(struct PIXEL*));
  for (i = 0; i < InfoHeader.Height; i++) {
    pixelShade[i] = (struct PIXEL*)calloc(InfoHeader.Width, sizeof(struct PIXEL));
  }

  //allocate space for pixel edge pointer
  pixelEdge = (struct PIXEL**)calloc(InfoHeader.Height, sizeof(struct PIXEL*));
  for (i = 0; i < InfoHeader.Height; i++) {
    pixelEdge[i] = (struct PIXEL*)calloc(InfoHeader.Width, sizeof(struct PIXEL));
  }

  // copy Pixel pointer to the shade and edge pointers
  for(i=0;i<InfoHeader.Height;i++){
    for(j=0;j<InfoHeader.Width;j++){
      pixelShade[i][j].Red = Pixel[i][j].Red;
      pixelShade[i][j].Green = Pixel[i][j].Green;
      pixelShade[i][j].Blue = Pixel[i][j].Blue;
      pixelEdge[i][j].Red = Pixel[i][j].Red;
      pixelEdge[i][j].Green = Pixel[i][j].Green;
      pixelEdge[i][j].Blue = Pixel[i][j].Blue;
    }
  }

  // create pointer with the shade effect applied
  pixelShade = shadeImage(pixelShade, &InfoHeader, numArg3, numArg4, numArg5);

  // create pointer with the shade effect applied
  pixelEdge = edgeImage(pixelEdge, Pixel, &InfoHeader);

  // output altered images
  outImage(&Header, &InfoHeader, pixelShade, pixelEdge, argv[2]);

  return 0;
}

void checkInput(int argc, char *argv[6]) {
  int numArg3 = atoi(argv[3]), numArg4 = atoi(argv[4]), numArg5 = atoi(argv[5]);
  //check of the program was given the correct number of arguments, if not error
  if (argc != 6) {
    fprintf(stderr, "ERROR: Incorrect number of arguments.\n");
    exit(-1);
  }

  // if input and output file names are the same, error and print error message
  if(strcmp(argv[1], argv[2])==0) {
    fprintf(stderr, "ERROR: Input and output file names must be different.\n");
    exit(-1);
  }

  // check if any of the input argument values is outside the range -200 to 200
  if(numArg3 > 200 || numArg3 < -200 || numArg4 > 200 || numArg4 < -200 ||
     numArg5 > 200 || numArg5 < -200) {
       fprintf(stderr,"ERROR: Numeric value outside range of -200 to 200.\n");
       exit(-1);
  }
}

struct PIXEL** readIn(struct HEADER* Header, struct INFOHEADER* InfoHeader,
  struct PIXEL** Pixel, char* fileName) {
  FILE* inFile;
  int i, j;

  //open file for read in, check if file location is valid
  inFile = fopen(fileName, "r");
  if (inFile == NULL) {
    fprintf(stderr, "ERROR: Cannot read designated file, exitting.\n");
    exit(-1); //Terminate program
  }

  // Read in all data in Header of file
  fread(&Header->Type, sizeof(short int), 1, inFile);
  fread(&Header->Size, sizeof(int), 1, inFile);
  fread(&Header->Reserved1, sizeof(short int), 1, inFile);
  fread(&Header->Reserved2, sizeof(short int), 1, inFile);
  fread(&Header->Offset, sizeof(int), 1, inFile);

  // Read in all data in info header of file
  fread(&InfoHeader->Size, sizeof(int), 1, inFile);
  fread(&InfoHeader->Width, sizeof(int), 1, inFile);
  fread(&InfoHeader->Height, sizeof(int), 1, inFile);
  fread(&InfoHeader->Planes, sizeof(short int), 1, inFile);
  fread(&InfoHeader->Bits, sizeof(short int), 1, inFile);
  fread(&InfoHeader->Compression, sizeof(int), 1, inFile);
  fread(&InfoHeader->ImageSize, sizeof(int), 1, inFile);
  fread(&InfoHeader->xResolution, sizeof(int), 1, inFile);
  fread(&InfoHeader->yResolution, sizeof(int), 1, inFile);
  fread(&InfoHeader->Colors, sizeof(int), 1, inFile);
  fread(&InfoHeader->ImportantColors, sizeof(int), 1, inFile);

  // allocate space for Pixel struct
  Pixel = (struct PIXEL**)calloc(InfoHeader->Height, sizeof(struct PIXEL*));
  for (i = 0; i < InfoHeader->Height; i++) {
    Pixel[i] = (struct PIXEL*)calloc(InfoHeader->Width, sizeof(struct PIXEL));
  }

  // Read in Pixel values for pixel struct
  for(i=0;i<InfoHeader->Height;i++){
    for(j=0;j<InfoHeader->Width;j++){
      fread(&Pixel[i][j], sizeof(struct PIXEL),1,inFile);
    }
  }

  // Close file
  fclose(inFile);

  // return value for read in image
  return Pixel;
}

void shadeImage(struct PIXEL** pixelShade, struct INFOHEADER* InfoHeader,
  int red, int green, int blue){
  int i,j;

  //Shade Pixels in the whole image
  for(i = 0; i < InfoHeader->Height;i++){
    for(j = 0; j < InfoHeader->Width;j++) {
      // Shade the red pixel value, making sure it's between 0 and 255
      if(pixelShade[i][j].Red + red > 255) {
        pixelShade[i][j].Red = 255;
      } else if( pixelShade[i][j].Red < 0) {
        pixelShade[i][j].Red = 0;
      } else {
        pixelShade[i][j].Red += red;
      }

      // Shade the green pixel value, making sure it's between 0 and 255
      if(pixelShade[i][j].Green + green > 255) {
        pixelShade[i][j].Green = 255;
      } else if( pixelShade[i][j].Red < 0) {
        pixelShade[i][j].Green = 0;
      } else {
        pixelShade[i][j].Green += green;
      }

      // Shade the blue pixel value, making sure it's between 0 and 255
      if(pixelShade[i][j].Blue + blue > 255) {
        pixelShade[i][j].Blue = 255;
      } else if( pixelShade[i][j].Blue < 0) {
        pixelShade[i][j].Blue = 0;
      } else {
        pixelShade[i][j].Blue += blue;
      }
    }
  }

  // return pointer to the shade altered image to main
  return pixelShade;
}

struct PIXEL** edgeImage(struct PIXEL** pixelEdge, struct PIXEL** Pixel,
  struct INFOHEADER* InfoHeader) {
  int i, j, k, l;
  char red, green, blue;
  char Matrix[3][3] =
  { {  0, -1,  0 },
    { -1,  4, -1 },
    {  0, -1,  0 }
  };

  // use the Martix as an "edge detector" as shown in assignment specifications
  for(i=1; i<InfoHeader->Height-1; i++) {
    for(j=1; j<InfoHeader->Width-1; j++){
      //reset color values on each call
      red = 0;
      green = 0;
      blue = 0;
      
      for(k=-1; k<1; k++){
        for(l=-1; l<1; l++){
          red = red + (Pixel[i+k][j+l].Red)*Matrix[k+1][l+1];
          green = green + (Pixel[i+k][j+l].Green)*Matrix[k+1][l+1];
          blue = blue + (Pixel[i+k][j+l].Blue)*Matrix[k+1][l+1];
        }
      }

      // make sure RGB values range from 0 to 255, adjust values if not
      if(red < 0) {
        red = 0;
      } else if(red > 255) {
        red = 255;
      }

      if(green < 0) {
        green = 0;
      } else if(green > 255) {
        green = 255;
      }

      if(blue < 0) {
        blue = 0;
      } else if(blue > 255) {
        blue = 255;
      }

      // store new edge values to double pointer
      pixelEdge[i][j].Red = red;
      pixelEdge[i][j].Green = green;
      pixelEdge[i][j].Blue = blue;
    }
  }

  // return pointer to the edge altered image to main
  return pixelEdge;
}

void outImage(struct HEADER* Header, struct INFOHEADER* InfoHeader,
  struct PIXEL** pixelShade, struct PIXEL** pixelEdge, char* fileName) {
    FILE* outFile;
    char *shadeFileName = calloc(strlen(fileName), sizeof(char));
    char *edgeFileName = calloc(strlen(fileName), sizeof(char));
    int i,j;

    // Create the file names for the edge and shade modified image
    fileName[strlen(fileName)-4] = '\0';
    strcpy(shadeFileName, fileName);
    strcpy(edgeFileName, fileName);
    strcat(shadeFileName, "(shade).bmp");
    strcat(edgeFileName, "(edge).bmp");

    // open outfile for write out for shade, check if file destination is valid
    outFile = fopen(shadeFileName, "w");
    if (outFile == NULL) {
      fprintf(stderr, "ERROR: Cannot read designated file, exitting.\n");
      exit(-1); //Terminate program
    }

    printf("\nSaving shaded image as \"%s\".\n", shadeFileName);

    // write out all data for Header of file
    fwrite(&Header->Type, sizeof(short int), 1, outFile);
    fwrite(&Header->Size, sizeof(int), 1, outFile);
    fwrite(&Header->Reserved1, sizeof(short int), 1, outFile);
    fwrite(&Header->Reserved2, sizeof(short int), 1, outFile);
    fwrite(&Header->Offset, sizeof(int), 1, outFile);

    // write out all data for info header of file
    fwrite(&InfoHeader->Size, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Width, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Height, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Planes, sizeof(short int), 1, outFile);
    fwrite(&InfoHeader->Bits, sizeof(short int), 1, outFile);
    fwrite(&InfoHeader->Compression, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->ImageSize, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->xResolution, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->yResolution, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Colors, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->ImportantColors, sizeof(int), 1, outFile);

    // Write out pixels values for pixel struct
    for(i=0;i<InfoHeader->Height;i++){
      for(j=0;j<InfoHeader->Width;j++){
        fwrite(&pixelShade[i][j], sizeof(struct PIXEL),1,outFile);
      }
    }

    //close file
    fclose(outFile);

    // open outfile for write out for edge, check if file destination is valid
    outFile = fopen(edgeFileName, "w");
    if (outFile == NULL) {
      fprintf(stderr, "ERROR: Cannot read designated file, exitting.\n");
      exit(-1); //Terminate program
    }

    printf("\nSaving edge image as \"%s\".\n", edgeFileName);

    // write out all data for Header of file
    fwrite(&Header->Type, sizeof(short int), 1, outFile);
    fwrite(&Header->Size, sizeof(int), 1, outFile);
    fwrite(&Header->Reserved1, sizeof(short int), 1, outFile);
    fwrite(&Header->Reserved2, sizeof(short int), 1, outFile);
    fwrite(&Header->Offset, sizeof(int), 1, outFile);

    // write out all data for info header of file
    fwrite(&InfoHeader->Size, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Width, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Height, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Planes, sizeof(short int), 1, outFile);
    fwrite(&InfoHeader->Bits, sizeof(short int), 1, outFile);
    fwrite(&InfoHeader->Compression, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->ImageSize, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->xResolution, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->yResolution, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->Colors, sizeof(int), 1, outFile);
    fwrite(&InfoHeader->ImportantColors, sizeof(int), 1, outFile);

    // Write out pixels values for pixel struct
    for(i=0;i<InfoHeader->Height;i++){
      for(j=0;j<InfoHeader->Width;j++){
        fwrite(&pixelEdge[i][j], sizeof(struct PIXEL),1,outFile);
      }
    }

    // close file
    fclose(outFile);

  }
