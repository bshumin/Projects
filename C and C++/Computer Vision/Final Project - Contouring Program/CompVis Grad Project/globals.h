
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

#define WINDOW 7
#define WINDOW2 3

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

	// struct
typedef struct {
	int *xPts;
	int *yPts;
	int  lenPts;
	int  doneCont;
} ContPtStruct;

ContPtStruct *contStruct;

	// Global Controls
int		lmbDown;
int		*xAllPts;
int		*yAllPts;
int		*xContPts;
int		*yContPts;
int		EnableContour;
int		count;
int		contControl;
int		found;
int		array_size;
int		cleared;
int		centerx, centery;
int		shiftDown;
int		gotPoint; // var for making sure only one point is anchored at a time
int		ptIndex;
int		contType; // 0 - Active, 1 - Bubble

/* NOTE: a structCount size of 0 indicates the struct is initialized but empty, and not NULL*/

	// Display flags
int		ShowPixelCoords;
int		stepEnabled;

	// Image data
unsigned char	*OriginalImage;
int				ROWS,COLS;
char			str[100];

#define TIMER_SECOND	1			/* ID of timer used for animation */

	// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

	// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void ContourPoints();
void BubblePoints();
void CleanContour();
void AnimationThread(void *);		/* passes address of window */
//void ReallocStruct();
void ClearStruct();
//void PaintStruct();
void FillStruct(int *xPt, int *yPt, int doneVal, int numPts);
int  FindPoint(int xPt, int yPt);

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
