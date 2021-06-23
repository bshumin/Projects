#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define PI 3.14159265
#define ROWS 256
#define COLS 256
#define SQR(x) ((x)*(x))

// compile with: gcc -o render lab5.c -Wall -g -lm

int main(int argc, char *argv[]) {
  FILE  *fpt, *fpt2;
  int   numFaces, numVert, tempI, *faces, i, j, X, Y, Z, r, c;
  char  header[320], temp[320];
  float *verts, min[3] = {0}, max[3] = {0}, center[3] = {0}, extent;
  float tempCam[3] = {0}, tempCam2[3] = {0}, tempCam3[3] = {0};
  float camera[3] = {1,0,0}, up[3] = {0,0,1}, left[3] = {0}, right[3] = {0}, a;
  float top[3] = {0}, bottom[3] = {0}, topleft[3] = {0};
  float Rx[9] = {0}, Ry[9] = {0}, Rz[9] = {0};
  float zBuffDepth = 999999;

  unsigned char pixels[256][256] = {0};
  float image[256][256][3] = {0};
  float n, d, intersect[3] = {0}, dot1, dot2, dot3;
  float v0[3] = {0}, v1[3] = {0}, v2[3] = {0};
  float crossP1[3] = {0}, crossP2[3] = {0};

  // confirm user input is valid
  if(argc != 6){
    printf("Error: Format should be ./triangle [file.ply] [X_Angle] [Y_Angle] [Z_Angle] [out.ppm]\n");
    exit(0);
  }
  // get rotational angles
  X = atoi(argv[2]);
  Y = atoi(argv[3]);
  Z = atoi(argv[4]);

  if ((fpt=fopen(argv[1],"rb")) == NULL){
    printf("Unable to open %s for reading\n", argv[1]);
    exit(0);
  }
  fscanf(fpt,"%s ",header);
  if (strcmp(header,"ply") != 0){
    printf("Not a PLY image\n");
    exit(0);
  }
  // read in header data
  fscanf(fpt,"%s %s %s ",temp, temp, temp);
  fscanf(fpt,"%s %s %d ",temp, temp, &numVert);
  fscanf(fpt,"%s %s %s ",temp, temp, temp);
  fscanf(fpt,"%s %s %s ",temp, temp, temp);
  fscanf(fpt,"%s %s %s ",temp, temp, temp);
  fscanf(fpt,"%s %s %d ",temp, temp, &numFaces);
  fscanf(fpt,"%s %s %s %s %s",temp, temp, temp, temp, temp);
  fscanf(fpt,"%s ",temp);

  printf("NumFaces: %d\nNumVert: %d\n", numFaces, numVert);
  float planeEq[numFaces][4];

  // calloc vertices and faces
  verts = (float*)calloc(numVert*3, sizeof(float));
  faces = (int*  )calloc(numFaces*3, sizeof(int));

  j=0;
  for(i=0;i<numVert;i++){
    fscanf(fpt,"%f %f %f ", &verts[j], &verts[j+1], &verts[j+2]);
    //printf("%f %f %f\n",verts[j], verts[j+1], verts[j+2]);
    //initialize mins/maxes
    if(j == 0){
      min[0] = verts[j];
      min[1] = verts[j+1];
      min[2] = verts[j+2];
      max[0] = verts[j];
      max[1] = verts[j+1];
      max[2] = verts[j+2];
    }
    if(verts[j] < min[0]){
      min[0] = verts[j];
    } else
    if(verts[j] > max[0]){
      max[0] = verts[j];
    }
    if(verts[j+1] < min[1]){
      min[1] = verts[j+1];
    } else
    if(verts[j+1] > max[1]){
      max[1] = verts[j+1];
    }
    if(verts[j+2] < min[2]){
      min[2] = verts[j+2];
    } else
    if(verts[j+2] > max[2]){
      max[2] = verts[j+2];
    }
    // center[0]+=verts[j];
    // center[1]+=verts[j+1];
    // center[3]+=verts[j+2];
    j+=3;
  }

  //calculate center values
  center[0] = (min[0]+max[0])/2.0;
  center[0] = center[0] == -0 ? 0 : center[0];
  center[1] = (min[1]+max[1])/2.0;
  center[1] = center[1] == -0 ? 0 : center[1];
  center[2] = (min[2]+max[2])/2.0;
  center[2] = center[2] == -0 ? 0 : center[2];

  //determine largest extent, set to arbitrary value first
  extent = max[0]-min[0];
  for(i=0;i<3;i++){
    if(max[i]-min[i] > extent){
      extent = max[i]-min[i];
    }

  }

  printf("Min: x:%f y:%f z:%f\n",min[0],min[1],min[2]);
  printf("Max: x:%f y:%f z:%f\n",max[0],max[1],max[2]);
  printf("Center: x:%f y:%f z:%f\n",center[0],center[1],center[2]);
  printf("Extent: %f \n", extent);

  j=0;
  for(i=0;i<numFaces;i++){
    fscanf(fpt,"%d %d %d %d ", &tempI, &faces[j], &faces[j+1], &faces[j+2]);
    //printf("%d %d %d\n",faces[j], faces[j+1], faces[j+2]);
    j+=3;
  }
  fclose(fpt);

  // calculate rotations
  Rx[0] = 1;
  Rx[4] = cos((float)X*PI/(float)180);
  Rx[5] = -1*sin((float)X*PI/(float)180);
  Rx[5] = Rx[5] == -0 ? 0 : Rx[5];
  Rx[7] = sin((float)X*PI/(float)180);
  Rx[8] = cos((float)X*PI/(float)180);

  Ry[0] = cos((float)Y*PI/(float)180);
  Ry[2] = sin((float)Y*PI/(float)180);
  Ry[4] = 1;
  Ry[6] = -1*sin((float)Y*PI/(float)180);
  Ry[6] = Ry[6] == -0 ? 0 : Ry[6];
  Ry[8] = cos((float)Y*PI/(float)180);

  Rz[0] = cos((float)Z*PI/(float)180);
  Rz[1] = -1*sin((float)Z*PI/(float)180);
  Rz[1] = Rz[1] == -0 ? 0 : Rz[1];
  Rz[3] = sin((float)Z*PI/(float)180);
  Rz[4] = cos((float)Z*PI/(float)180);
  Rz[8] = 1;

  //rotate camera
  tempCam[0] = camera[0]*Rx[0]+camera[1]*Rx[3]+camera[2]*Rx[6];
  tempCam[1] = camera[0]*Rx[1]+camera[1]*Rx[4]+camera[2]*Rx[7];
  tempCam[2] = camera[0]*Rx[2]+camera[1]*Rx[5]+camera[2]*Rx[8];

  tempCam2[0] = tempCam[0]*Ry[0]+tempCam[1]*Ry[3]+tempCam[2]*Ry[6];
  tempCam2[1] = tempCam[0]*Ry[1]+tempCam[1]*Ry[4]+tempCam[2]*Ry[7];
  tempCam2[2] = tempCam[0]*Ry[2]+tempCam[1]*Ry[5]+tempCam[2]*Ry[8];


  tempCam3[0] = tempCam2[0]*Rz[0]+tempCam2[1]*Rz[3]+tempCam2[2]*Rz[6];
  tempCam3[1] = tempCam2[0]*Rz[1]+tempCam2[1]*Rz[4]+tempCam2[2]*Rz[7];
  tempCam3[2] = tempCam2[0]*Rz[2]+tempCam2[1]*Rz[5]+tempCam2[2]*Rz[8];


  camera[0] = tempCam3[0];
  camera[1] = tempCam3[1];
  camera[2] = tempCam3[2];
  printf("camera before extent: %f %f %f \n", camera[0],camera[1],camera[2]);


  //rotate up
  tempCam[0] = up[0]*Rx[0]+up[1]*Rx[3]+up[2]*Rx[6];
  tempCam[1] = up[0]*Rx[1]+up[1]*Rx[4]+up[2]*Rx[7];
  tempCam[2] = up[0]*Rx[2]+up[1]*Rx[5]+up[2]*Rx[8];

  tempCam2[0] = tempCam[0]*Ry[0]+tempCam[1]*Ry[3]+tempCam[2]*Ry[6];
  tempCam2[1] = tempCam[0]*Ry[1]+tempCam[1]*Ry[4]+tempCam[2]*Ry[7];
  tempCam2[2] = tempCam[0]*Ry[2]+tempCam[1]*Ry[5]+tempCam[2]*Ry[8];

  tempCam3[0] = tempCam2[0]*Rz[0]+tempCam2[1]*Rz[3]+tempCam2[2]*Rz[6];
  tempCam3[1] = tempCam2[0]*Rz[1]+tempCam2[1]*Rz[4]+tempCam2[2]*Rz[7];
  tempCam3[2] = tempCam2[0]*Rz[2]+tempCam2[1]*Rz[5]+tempCam2[2]*Rz[8];

  up[0] = tempCam3[0];
  up[1] = tempCam3[1];
  up[2] = tempCam3[2];
  printf("up: %f %f %f \n", up[0],up[1],up[2]);



  printf("%d %d %d\n", X, Y, Z);
  for(i=0;i<9;i++){
    printf("%f\t%f\t%f\n",Rx[i],Ry[i],Rz[i]);
  }

  // move and scale camera based on extent
  printf("Camera: ");
  for(i=0;i<3;i++){
    camera[i] = camera[i]*1.5*extent + center[i];
    printf("%f ", camera[i]);
  }
  printf("\n");

  //<left> = <up> x <center-camera>
  // for(i=0;i<3;i++){
  //   left[i] = up[i]*center[i]-camera[i];
  // }
  left[0] = up[1]*(center[2]-camera[2]) - up[2]*(center[1]-camera[1]);
  left[1] = up[2]*(center[0]-camera[0]) - up[0]*(center[2]-camera[2]);
  left[2] = up[0]*(center[1]-camera[1]) - up[1]*(center[0]-camera[0]);

  //a = ||<left>||
  a = sqrt(SQR(left[0]) + SQR(left[1]) + SQR(left[2]));

  //<left> = (E/2a)*<left> + <center>
  for(i=0;i<3;i++){
    left[i] = (extent/(2*a))*left[i]+center[i];
  }

  //<right> = <center - camera> x <up>
  // for(i=0;i<3;i++){
  //   right[i] = center[i]-camera[i]*up[i];
  // }
  right[0] = (center[1]-camera[1])*up[2] - (center[2]-camera[2])*up[1];
  right[1] = (center[2]-camera[2])*up[0] - (center[0]-camera[0])*up[2];
  right[2] = (center[0]-camera[0])*up[1] - (center[1]-camera[1])*up[0];

  //<right> = (E/2a)<right> + <center>
  for(i=0;i<3;i++){
    right[i] = (extent/(2*a))*right[i]+center[i];
  }

  //<top> = (E/2)<up>+<center>
  for(i=0;i<3;i++){
    top[i] = (extent/2)*up[i]+center[i];
  }

  //<bottom> = (-E/2)<up>+<center>
  for(i=0;i<3;i++){
    bottom[i] = (-extent/2)*up[i]+center[i];
  }

  //<topleft> = (E/2)<up>+<left>
  for(i=0;i<3;i++){
    topleft[i] = (extent/2)*up[i]+left[i];
  }
  printf("left: %f %f %f\n", left[0], left[1], left[2]);
  printf("right: %f %f %f\n", right[0], right[1], right[2]);
  printf("top: %f %f %f\n", top[0], top[1], top[2]);
  //printf("up: %f %f %f\n", up[0], up[1], up[2]);
  printf("bottom: %f %f %f\n", bottom[0], bottom[1], bottom[2]);
  printf("topleft: %f %f %f\n", topleft[0], topleft[1], topleft[2]);
  printf("a: %f\n", a);

  // find image pixel vector coordinates
  for(r=0;r<ROWS;r++){
    for(c=0;c<COLS;c++){
      for(i=0;i<3;i++){
        image[r][c][i] = topleft[i] + ((float)c/(float)(COLS-1))*(right[i]-left[i]) + ((float)r/(float)(ROWS-1))*(bottom[i]-top[i]);
      }
      //printf("Image vector: <%f, %f, %f> \n",image[r][c][0],image[r][c][1],image[r][c][2]);
      // check all faces to find plane equations
      zBuffDepth = 999999;
      for(i=0;i<numFaces;i++){
        // get v0, v1, v2
        v0[0] = verts[3*faces[3*i  ]  ];
        v0[1] = verts[3*faces[3*i  ]+1];
        v0[2] = verts[3*faces[3*i  ]+2];

        v1[0] = verts[3*faces[3*i+1]  ];
        v1[1] = verts[3*faces[3*i+1]+1];
        v1[2] = verts[3*faces[3*i+1]+2];

        v2[0] = verts[3*faces[3*i+2]  ];
        v2[1] = verts[3*faces[3*i+2]+1];
        v2[2] = verts[3*faces[3*i+2]+2];

        if(r==0 && c == 0 && i==0){
          printf("v0: <%f, %f, %f> \n",v0[0], v0[1],v0[2]);
          printf("v1: <%f, %f, %f> \n",v1[0], v1[1],v1[2]);
          printf("v2: <%f, %f, %f> \n",v2[0], v2[1],v2[2]);
        }

        // get A,B,C
        //<A,B,C> = <v1-v0> X <v2-v0>
        float v3[3] = {0}, v4[4]= {0};
        v3[0] = v1[0]-v0[0];
        v3[1] = v1[1]-v0[1];
        v3[2] = v1[2]-v0[2];

        v4[0] = v2[0]-v0[0];
        v4[1] = v2[1]-v0[1];
        v4[2] = v2[2]-v0[2];

        // planeEq[i][0] = ((v1[1]-v0[1])*(v2[2]-v0[2]) - (v1[2]-v0[2])*(v2[1]-v0[1]));        // A
        // planeEq[i][1] = ((v1[0]-v0[0])*(v2[2]-v0[2]) - (v1[0]-v0[0])*(v2[2]-v0[2]));        // B
        // planeEq[i][2] = ((v1[0]-v0[0])*(v2[1]-v0[1]) - (v1[1]-v0[1])*(v2[0]-v0[0]));        // C
        planeEq[i][0] = (v3[1]*v4[2]) - (v3[2]*v4[1]);        // A
        planeEq[i][1] = (v3[2]*v4[0]) - (v3[0]*v4[2]);        // B
        planeEq[i][2] = (v3[0]*v4[1]) - (v3[1]*v4[0]);        // C

        // D = -<A,B,C>*<image-camera>
        planeEq[i][3] = -1*(planeEq[i][0]*v0[0]+planeEq[i][1]*v0[1]+planeEq[i][2]*v0[2]); // D

        //exit(0);

        // n = -<A,B,C>*<camera> - D
        n = -1*(planeEq[i][0]*camera[0]+planeEq[i][1]*camera[1]+planeEq[i][2]*camera[2]) - planeEq[i][3];
        d = (planeEq[i][0]*(image[r][c][0]-camera[0]) + planeEq[i][1]*(image[r][c][1]-camera[1]) + planeEq[i][2]*(image[r][c][2]-camera[2]));

        // if d is close to zero, skip this plane (near parallel to view)
        if(fabs(d) < 0.000001){
          continue;
        }
        // if(c==0 && r==0){
        //   //printf("(n/d): %f\n",(n/d));
        //   printf("Image: %f %f %f\n",image[r][c][0],image[r][c][1],image[r][c][2]);
        //   printf("<A,B,C,D>: <%f, %f, %f, %f> \n",planeEq[i][0], planeEq[i][1], planeEq[i][2], planeEq[i][3]);
        //   //printf("Cam: %f %f %f\n",camera[0],camera[1],camera[2]);
        //   printf("n: %.5f d: %.5f ----- (n/d): %f\n", n, d,(n/d));
        // }


        // find intersect with ray and plane
        for(j=0;j<3;j++){
          intersect[j] = camera[j] + (n/d)*(image[r][c][j]-camera[j]);
        }

        crossP1[0] = (v2[1]-v0[1])*(v1[2]-v0[2])-(v2[2]-v0[2])*(v1[1]-v0[1]);
        crossP1[1] = (v2[2]-v0[2])*(v1[0]-v0[0])-(v2[0]-v0[0])*(v1[2]-v0[2]);
        crossP1[2] = (v2[0]-v0[0])*(v1[1]-v0[1])-(v2[1]-v0[1])*(v1[0]-v0[0]);

        crossP2[0] = (intersect[1]-v0[1])*(v1[2]-v0[2])-(intersect[2]-v0[2])*(v1[1]-v0[1]);
        crossP2[1] = (intersect[2]-v0[2])*(v1[0]-v0[0])-(intersect[0]-v0[0])*(v1[2]-v0[2]);
        crossP2[2] = (intersect[0]-v0[0])*(v1[1]-v0[1])-(intersect[1]-v0[1])*(v1[0]-v0[0]);
        if(r==0 && c == 1 && i==0){
          printf("intersect: %f %f %f\n",intersect[0], intersect[1], intersect[2]);
          printf("crossP1: %f %f %f\n",crossP1[0], crossP1[1], crossP1[2]);
          printf("crossP2: %f %f %f\n",crossP2[0], crossP2[1], crossP2[2]);
        }

        dot1 = crossP1[0]*crossP2[0]+crossP1[1]*crossP2[1]+crossP1[2]*crossP2[2];

        crossP1[0] = (v0[1]-v1[1])*(v2[2]-v1[2])-(v0[2]-v1[2])*(v2[1]-v1[1]);
        crossP1[1] = (v0[2]-v1[2])*(v2[0]-v1[0])-(v0[0]-v1[0])*(v2[2]-v1[2]);
        crossP1[2] = (v0[0]-v1[0])*(v2[1]-v1[1])-(v0[1]-v1[1])*(v2[0]-v1[0]);

        crossP2[0] = (intersect[1]-v1[1])*(v2[2]-v1[2])-(intersect[2]-v1[2])*(v2[1]-v1[1]);
        crossP2[1] = (intersect[2]-v1[2])*(v2[0]-v1[0])-(intersect[0]-v1[0])*(v2[2]-v1[2]);
        crossP2[2] = (intersect[0]-v1[0])*(v2[1]-v1[1])-(intersect[1]-v1[1])*(v2[0]-v1[0]);
        if(r==0 && c == 1 && i==0){
          printf("crossP1: %f %f %f\n",crossP1[0], crossP1[1], crossP1[2]);
          printf("crossP2: %f %f %f\n",crossP2[0], crossP2[1], crossP2[2]);
        }

        dot2 = crossP1[0]*crossP2[0]+crossP1[1]*crossP2[1]+crossP1[2]*crossP2[2];

        crossP1[0] = (v1[1]-v2[1])*(v0[2]-v2[2])-(v1[2]-v2[2])*(v0[1]-v2[1]);
        crossP1[1] = (v1[2]-v2[2])*(v0[0]-v2[0])-(v1[0]-v2[0])*(v0[2]-v2[2]);
        crossP1[2] = (v1[0]-v2[0])*(v0[1]-v2[1])-(v1[1]-v2[1])*(v0[0]-v2[0]);

        crossP2[0] = (intersect[1]-v2[1])*(v0[2]-v2[2])-(intersect[2]-v2[2])*(v0[1]-v2[1]);
        crossP2[1] = (intersect[2]-v2[2])*(v0[0]-v2[0])-(intersect[0]-v2[0])*(v0[2]-v2[2]);
        crossP2[2] = (intersect[0]-v2[0])*(v0[1]-v2[1])-(intersect[1]-v2[1])*(v0[0]-v2[0]);
        if(r==0 && c == 1 && i==0){
          printf("crossP1: %f %f %f\n",crossP1[0], crossP1[1], crossP1[2]);
          printf("crossP2: %f %f %f\n",crossP2[0], crossP2[1], crossP2[2]);
        }

        dot3 = crossP1[0]*crossP2[0]+crossP1[1]*crossP2[1]+crossP1[2]*crossP2[2];

        if(r==0 && c == 0 && i==0){
          printf("dots: %f %f %f\n", dot1, dot2, dot3);
          printf("<A,B,C,D>: <%f, %f, %f, %f> \n",planeEq[i][0], planeEq[i][1], planeEq[i][2], planeEq[i][3]);
          printf("n:%f\nd:%f\n",n,d);
        }

        if(dot1 < 0 || dot2 < 0 || dot3 < 0){
          //printf("DOT LESS THAN 0\n");
          continue;
        }

        // check for closer triangle blocking current triangle

        if((n/d) > zBuffDepth){
          continue;
        } else {
          zBuffDepth = (n/d);
        }

        // set pixel value
        pixels[r][c] = 155+(i%100);

      }
    }
  }
  // write out output image
  fpt2=fopen(argv[5],"wb");
  fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(pixels,COLS*ROWS,1,fpt2);
  fclose(fpt2);

  free(faces);
  free(verts);

}
