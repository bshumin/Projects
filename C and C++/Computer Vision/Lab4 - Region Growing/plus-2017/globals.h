
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;


	// Predicates
int		intensity_thresh;
int		centroid_thresh;
int		blue, green, red;

	// averages
float	intensity_avg;
float	centroid_avgx;
float	centroid_avgy;
int		count;

	// Display flags
int		ShowPixelCoords;
int		ShowBigDots;
int		PlayMode;
int		StepMode;
int		running;
int		done;
int		getNewTemp;
int		stepEnabled;

	// Image data
unsigned char	*OriginalImage;
unsigned char   *tempRegion;
unsigned char	*inRegion;
int				ROWS,COLS;

#define TIMER_SECOND	1			/* ID of timer used for animation */

	// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

	// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void RegionFill();
void WeightAvgs(int intensity, int centroidx, int centroidy);
void CleanRegion();
void QueuePaintFill(unsigned char* image, int ROWS, int COLS, int r, int c, int paint_over_label, int new_label, int* indices, int* count);
void AnimationThread(void *);		/* passes address of window */

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
