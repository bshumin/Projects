#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  FILE		             *fpt;
//  unsigned char	*image;
  char		             header[80];
  int		               ROWS,COLS,BYTES;
  int		               i;

  Display			         *Monitor;
  Window			         ImageWindow;
  GC				           ImageGC;
  XWindowAttributes		 Atts;
  XImage			         *Picture;
  unsigned char		     *displaydata;

  /* tell user how to use program if incorrect arguments */
  if (argc != 2)
  {
  printf("Usage:  lab1 [filename]\n");
  exit(0);
  }

  /* open image for reading */
  fpt=fopen(argv[1],"rb");
  if (fpt == NULL)
  {
  printf("Unable to open %s for reading\n",argv[1]);
  exit(0);
  }
  /* read image header (simple 8-bit greyscale PPM only) */
  i=fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (i != 4  ||  (strcmp(header,"P5") != 0 && strcmp(header,"P6") != 0)  ||  BYTES != 255)
  {
  printf("%s is not an 8-bit PPM greyscale (P5) image\n",argv[1]);
  fclose(fpt);
  exit(0);
  }
  /* allocate dynamic memory for image */
  displaydata=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  if (displaydata == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS,ROWS);
  exit(0);
  }
  /* read image data from file */
  fread(displaydata,1,ROWS*COLS,fpt);
  fclose(fpt);

  Monitor=XOpenDisplay(NULL);
  if (Monitor == NULL){
    printf("Unable to open graphics display\n");
    exit(0);
  }

  ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
  	50,10,		/* x,y on screen */
  	COLS,ROWS,	/* width, height */
  	2, 		/* border width */
  	BlackPixel(Monitor,0),
  	WhitePixel(Monitor,0));

  ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

  XMapWindow(Monitor,ImageWindow);
  XFlush(Monitor);
  while(1) {
    XGetWindowAttributes(Monitor,ImageWindow,&Atts);
    if (Atts.map_state == IsViewable /* 2 */){
      break;
    }
  }
  Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
  		DefaultDepth(Monitor,0),
  		ZPixmap,	/* format */
  		0,		/* offset */
  		displaydata,/* the data */
  		COLS,ROWS,	/* size of the image data */
  		16,		/* pixel quantum (8, 16 or 32) */
  		0);		/* bytes per line (0 causes compute) */
  XPutImage(Monitor,ImageWindow,ImageGC,Picture,
  		0,0,0,0,	/* src x,y and dest x,y offsets */
  		COLS,ROWS);	/* size of the image data */
  XFlush(Monitor);
  sleep(2);
  XCloseDisplay(Monitor);
}
