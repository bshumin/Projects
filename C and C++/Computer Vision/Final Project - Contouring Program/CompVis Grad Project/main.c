
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)

{
	MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;
	HDC			hDC;


	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, "ID_PLUS_ICON");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = "ID_MAIN_MENU";
	wc.lpszClassName = "PLUS";

	if (!RegisterClass(&wc))
		return(FALSE);

	hWnd = CreateWindow("PLUS", "LAB4",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 0, 400, 400, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return(FALSE);
	ShowScrollBar(hWnd, SB_BOTH, FALSE);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MainWnd = hWnd;

	// initialize globals
	count = 0;
	ThreadRunning = 0;
	lmbDown = 0;
	EnableContour = 1;
	//structCount = 0;
	centerx   = 0;
	centery   = 0;
	cleared   = 1;
	shiftDown = 0;
	gotPoint  = 0;
	xAllPts   = (int*)calloc(ROWS * COLS, sizeof(int));
	yAllPts   = (int*)calloc(ROWS * COLS, sizeof(int));
	xContPts  = (int*)calloc(ROWS * COLS, sizeof(int));
	yContPts  = (int*)calloc(ROWS * COLS, sizeof(int));
	contType  = 0;
	ClearStruct();

	strcpy(filename, "");
	OriginalImage	= NULL;
	
	ROWS = COLS = 0;

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)

{
	HMENU				hMenu;
	HWND				predDlg, colorDlg;
	OPENFILENAME		ofn;
	FILE*				fpt;
	HDC					hDC;
	char				header[320], text[320];
	int					BYTES, xPos, yPos, r, c, yCirc, circCount=0, i;
	unsigned char		pixel[3] = {0, 0, 0};
	
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords = (ShowPixelCoords + 1) % 2;
			PaintImage();
			break;

		case ID_DISPLAY_CLEARIMAGE:
			ClearStruct();
			Sleep(70);
			PaintImage();
			break;

		case ID_DISPLAY_ENABLECONTOUR:
			EnableContour = (EnableContour + 1) % 2;
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage = NULL;
			}
			PaintImage();
			ClearStruct();

			memset(&(ofn), 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			filename[0] = 0;
			ofn.nMaxFile = MAX_FILENAME_CHARS;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PNM files\0*.pnm\0PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!(GetOpenFileName(&ofn)) || filename[0] == '\0')
				break;		/* user cancelled load */
			if ((fpt = fopen(filename, "rb")) == NULL)
			{
				MessageBox(NULL, "Unable to open file", filename, MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
			if (BYTES != 255 && strcmp(header, "P6") != 0)
			{
				MessageBox(NULL, "Not a valid image", filename, MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			else if (strcmp(header, "P6") == 0)
			{
				OriginalImage = (unsigned char*)calloc(ROWS * COLS, 1);

				header[0] = fgetc(fpt);	/* whitespace character after header */

				// convert rgb to greyscale
				r = 0;
				for (r = 0; r < COLS * ROWS; r++) {
					fread(pixel, 3, 1, fpt);
					OriginalImage[r] = (pixel[0] + pixel[1] + pixel[2]) / 3;
				}
			}
			else if(strcmp(header, "P5") != 0)
			{
				MessageBox(NULL, "Not a PPM (P5 greyscale) image, or valid PNM (P6 RGB) file", filename, MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			else
			{
				OriginalImage = (unsigned char*)calloc(ROWS * COLS, 1);

				header[0] = fgetc(fpt);	/* whitespace character after header */
				fread(OriginalImage, 1, ROWS * COLS, fpt);
			}

			
			fclose(fpt);
			SetWindowText(hWnd, filename);
			PaintImage();
			break;

		case ID_FILE_QUIT:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_SIZE:		  /* could be used to detect when window size changes */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;

	case WM_PAINT:
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_RBUTTONDOWN:
		ThreadRunning = 0;
		Sleep(70);
		CleanContour();
		ClearStruct();
		PaintImage();
		//ClearStruct();
		break;
	case WM_RBUTTONUP:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);

		centerx = xPos;
		centery = yPos;
		hDC = GetDC(MainWnd);
		// graph circle
		i = 0;
		circCount = 0;
		count = 14;
		xContPts = (int*)malloc(42 * sizeof(int)); // always 42 points
		yContPts = (int*)malloc(42 * sizeof(int));

		if (xPos - 10 >= 0 && xPos + 10 < COLS && yPos - 10 >= 0 && yPos + 10 < ROWS) {

			for (c = -10; c <= 10; c++)
			{
				yCirc = (int)sqrt(abs((double)100 - (double)pow(c, 2)));
				SetPixel(hDC, xPos + c, yCirc + yPos, RGB(255, 0, 0));	/* color the cursor position red */
				if (i % 3 == 0)
				{
					xContPts[circCount] = xPos + c;
					yContPts[circCount] = yPos + yCirc;
					circCount++;
				}
				i++;
			}

			for (c = 10; c >= -10; c--)
			{
				yCirc = (int)-sqrt(abs((double)100 - (double)pow(c, 2)));
				SetPixel(hDC, xPos + c, yCirc + yPos, RGB(255, 0, 0));	/* color the cursor position red */
				if (i % 3 == 0)
				{
					xContPts[circCount] = xPos + c;
					yContPts[circCount] = yPos + yCirc;
					circCount++;
				}
				i++;
			}
			for (i = 0; i < circCount; i++) {
				sprintf(str, "%d| xCirc:%d yCirc:%d\n", i, xContPts[i], yContPts[i]);
				OutputDebugString(str);
			}
			FillStruct(xContPts, yContPts, 0, count);
			_beginthread(BubblePoints, 0, MainWnd);

		}
		break;
	case WM_LBUTTONDOWN:
		shiftDown = GetKeyState(VK_SHIFT);
		sprintf(str, "Shift:%d\n", shiftDown);
		OutputDebugString(str);
		ThreadRunning = 0;
		Sleep(50);
		if (shiftDown >= 0)
		{
			lmbDown = 1;
			// initialize contour points
			CleanContour();
			ClearStruct();
			PaintImage();
		}
		else
		{
			FillStruct(xContPts, yContPts, 0, count);
			
			gotPoint = 0;
		}
		
		
		break;
	case WM_LBUTTONUP:
		// on shift click, rerun contour after anchoring new point
		if (shiftDown < 0)
		{
			shiftDown = 0;
			if (contStruct != NULL) {
				if(contType == 0)
					_beginthread(ContourPoints, 0, MainWnd);
				else
					_beginthread(BubblePoints, 0, MainWnd);
			}
		}
		else
		{
			lmbDown = 0;
			count = 0;
			cleared = 0;
			// downsample contour points
			for (r = 0; r < array_size; r++)
			{
				if (r % 5 == 0)
				{
					count++;
					// if this is the first contour point
					if (count == 1)
					{
						xContPts[0] = xAllPts[r];
						yContPts[0] = yAllPts[r];
					}
					else
					{
						xContPts = (int*)realloc(xContPts, (count) * sizeof(int));
						yContPts = (int*)realloc(yContPts, (count) * sizeof(int));

						xContPts[count - 1] = xAllPts[r];
						yContPts[count - 1] = yAllPts[r];
					}
				}
			}

			ClearStruct();
			FillStruct(xContPts, yContPts, 0, count);
			_beginthread(ContourPoints, 0, MainWnd);

		}

		
		break;
		
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_MOUSEMOVE:
		if (lmbDown == 1 && ShowPixelCoords == 0)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			int found = 0;
			int *tmp;
			

			// look for value already in array
			for (r = 0; r < array_size; r++)
			{
				for (r = 0; r < array_size; r++)
				{
					if (xAllPts[r] == xPos && yAllPts[r] == yPos)
					{
						found == 1;
					}
				}
			}
			// if value not in array, add to array
			if (found == 0)
			{
				if (xAllPts[0] == 0 && yAllPts[0] == 0) 
				{
					
					//set initial pixel
					xAllPts[0] = xPos;
					yAllPts[0] = yPos;
					//sprintf(str, "x[0]:%d y[0]:%d\n", xAllPts[0], yAllPts[0]);
					//OutputDebugString(str);
				} 
				else
				{
					// realloc array and add new element
					array_size++;
					xAllPts = (int*)realloc(xAllPts, array_size * sizeof(int));
					yAllPts = (int*)realloc(yAllPts, array_size * sizeof(int));

					xAllPts[array_size - 1] = xPos;
					yAllPts[array_size - 1] = yPos;
				}
			}
			if (xPos >= 0 && xPos < COLS && yPos >= 0 && yPos < ROWS)
			{
				hDC = GetDC(MainWnd);
				int r;
				int c;
				SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */
				
				ReleaseDC(MainWnd, hDC);
			}
			//Sleep(10);
		}
		else if (ShowPixelCoords == 1)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			if (xPos >= 0 && xPos < COLS && yPos >= 0 && yPos < ROWS)
			{
				sprintf(text, "%d,%d=>%d     ", xPos, yPos, OriginalImage[yPos * COLS + xPos]);
				hDC = GetDC(MainWnd);
				TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
				int r;
				int c;
				
				SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */
				
				ReleaseDC(MainWnd, hDC);
			}
		}
		else if (shiftDown < 0 && gotPoint == 0) 
		{
			// find which contour point is being moved
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);

			//FillStruct(xContPts, yContPts, 0, count);
			ptIndex = FindPoint(xPos, yPos);
		}
		else if (shiftDown < 0 && gotPoint == 1)
		{
			hDC = GetDC(MainWnd);
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			// move contour point
			contStruct[0].xPts[ptIndex] = xPos;
			contStruct[0].yPts[ptIndex] = yPos;

			// paint new point
			PaintImage();
			for (i = 0; i < contStruct[0].lenPts; i++) {
				for (r = -3; r <= 3; r++) {
					for (c = -3; c <= 3; c++) {
						if (r == 0 || c == 0)
						{
							if (i == ptIndex) 
							{
								SetPixel(hDC, contStruct[0].xPts[i] + c, contStruct[0].yPts[i] + r, RGB(0, 255, 255));	/* color the cursor position cyan */
							}
							else {
								SetPixel(hDC, contStruct[0].xPts[i] + c, contStruct[0].yPts[i] + r, RGB(255, 0, 0));	/* color the cursor position red */
							}
						}
					}
				}
			}

		}


		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_KEYDOWN:
		if (wParam == 's' || wParam == 'S')
			PostMessage(MainWnd, WM_COMMAND, ID_SHOWPIXELCOORDS, 0);	  /* send message to self */
		if ((TCHAR)wParam == '1')
		{
			TimerRow = TimerCol = 0;
			SetTimer(MainWnd, TIMER_SECOND, 10, NULL);	/* start up 10 ms timer */
		}
		if ((TCHAR)wParam == '2')
		{
			KillTimer(MainWnd, TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
			PaintImage();								/* redraw original image, erasing animation */
		}
		if ((TCHAR)wParam == '3')
		{
			ThreadRunning = 1;
			_beginthread(AnimationThread, 0, MainWnd);	/* start up a child thread to do other work while this thread continues GUI */
		}
		if ((TCHAR)wParam == '4')
		{
			ThreadRunning = 0;							/* this is used to stop the child thread (see its code below) */
		}
		if (wParam == 'c' || wParam == 'C')
		{
			ClearStruct();								/* emergency exit */
		}
		
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
		hDC = GetDC(MainWnd);
		SetPixel(hDC, TimerCol, TimerRow, RGB(0, 0, 255));	/* color the animation pixel blue */
		ReleaseDC(MainWnd, hDC);
		TimerRow++;
		TimerCol += 2;
		break;
	case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	}

	hMenu = GetMenu(MainWnd);
	if (ShowPixelCoords == 1)
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	else
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_UNCHECKED);

	if (EnableContour == 1)
		CheckMenuItem(hMenu, ID_DISPLAY_ENABLECONTOUR, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_DISPLAY_ENABLECONTOUR, MF_UNCHECKED);

	DrawMenuBar(hWnd);

	return(0L);
}

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_INITDIALOG:
		return TRUE;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			/*PaintImage();
			intensity_thresh = GetDlgItemInt(hwnd, IDC_EDIT_INTENSITY, NULL, 0);
			centroid_thresh = GetDlgItemInt(hwnd, IDC_EDIT_CENTROID, NULL, 0);

			if (intensity_thresh == NULL || centroid_thresh == NULL) {
				break;
			}
			else if (intensity_thresh > 255 || intensity_thresh <= 0 || centroid_thresh <= 0) {
				break;
			}
			else {
				EndDialog(hwnd, IDOK);
			}*/
			break;
		case IDCANCEL:
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}


void PaintImage(){
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO* bm_info;
	int					i, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	unsigned char* DisplayImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

			/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	DisplayImage = (unsigned char*)calloc(DISPLAY_ROWS * DISPLAY_COLS, 1);
	for (r = 0; r < ROWS; r++)
		for (c = 0; c < COLS; c++)
			DisplayImage[r * DISPLAY_COLS + c] = OriginalImage[r * COLS + c];

	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i < 256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(DisplayImage);
	free(bm_info);
}

void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	char	text[300];

	ThreadRow = ThreadCol = 0;
	while (ThreadRunning == 1)
	{
		hDC = GetDC(MainWnd);
		SetPixel(hDC, ThreadCol, ThreadRow, RGB(0, 255, 0));	/* color the animation pixel green */
		sprintf(text, "%d,%d     ", ThreadRow, ThreadCol);
		TextOut(hDC, 300, 0, text, strlen(text));		/* draw text on the window */
		ReleaseDC(MainWnd, hDC);
		ThreadRow += 3;
		ThreadCol++;
		Sleep(100);		/* pause 100 ms */
	}
}

void CleanContour() {

	array_size = 1;

	if (xAllPts != NULL)
	{
		//free(xAllPts);
		xAllPts = (int*)calloc(1, sizeof(int));
	}
	if (yAllPts != NULL)
	{
		//free(yAllPts);
		yAllPts = (int*)calloc(1, sizeof(int));
	}
	if (xContPts != NULL)
	{
		//free(xContPts);
		xContPts = (int*)calloc(1, sizeof(int));
	}
	if (yContPts != NULL)
	{
		//free(yContPts);
		yContPts = (int*)calloc(1, sizeof(int));
		
	}
}

// function to increment the amount of current contour structs by 1 and realloc space for new struct
/*
void ReallocStruct()
{
	/* NOTE: a structCount size of 0 indicates the struct is initialized but empty, and not NULL
	
	structCount++;
	contStruct = (ContPtStruct*)realloc(contStruct, (structCount) * sizeof(ContPtStruct));
	
	sprintf(str, "structCount:%d\n", structCount);
	OutputDebugString(str);
}
*/

void ClearStruct()
{
	cleared = 1;
	if (contStruct == NULL)
	{
		contStruct = (ContPtStruct*)malloc(sizeof(ContPtStruct));
	}
	else
	{
		free(contStruct);
		contStruct = (ContPtStruct*)malloc(sizeof(ContPtStruct));
	}
	// set filled struct cout to 0
	sprintf(str, "freed Struct\n");
	OutputDebugString(str);
}

void FillStruct(int* xPt, int* yPt, int doneVal, int numPts)
{
	contStruct[0].xPts = xPt;
	contStruct[0].yPts = yPt;	
	contStruct[0].doneCont = doneVal;
	contStruct[0].lenPts = numPts;

	sprintf(str, "xPts[0]:%d yPts[0]: %d lenPts:%d done:%d\n", contStruct[0].xPts[0], contStruct[0].yPts[0], contStruct[0].lenPts, contStruct[0].doneCont);
	OutputDebugString(str);
}

int FindPoint(int xPt, int yPt)
{
	int i, point=0;
	for (i=0;i<contStruct[0].lenPts;i++) 
	{
		sprintf(str, "FP: xPts[0]:%d yPts[0]: %d lenPts:%d done:%d\n", contStruct[0].xPts[0], contStruct[0].yPts[0], contStruct[0].lenPts, contStruct[0].doneCont);
		OutputDebugString(str);

		if (contStruct[0].xPts[i] + 3 >= xPt && contStruct[0].xPts[i] - 3 <= xPt && contStruct[0].yPts[i] + 3 >= yPt && contStruct[0].yPts[i] - 3 <= yPt && gotPoint == 0)
		{
			gotPoint = 1;
			point = i;
		}
	}
	sprintf(str, "Point:%d\n", gotPoint);
	OutputDebugString(str);
	if (gotPoint == 1)
	{
		return point;
	}
	else
	{
		return -1;
	}
}


void ContourPoints() {
	FILE* fpt;
	HDC			   hDC;
	unsigned char  *image, *norm_ext, *origContour, *finalContour;
	char		   header[320];
	int		       run, r2, c2;
	int            r, c, i, j, temp1, temp2, * colsCont, *rowsCont;
	float		   *intEnergy1, *intEnergy2, * extEnergy1, *extEnergy2, avgDist = 0, *sumEnergy, sum2, min_val, max_val, min_val2, max_val2, sum;
	float		   *norm_int1, *norm_int2, * use_ext;
	int            nextPtx = 0, nextPty = 0, tempRow, tempCol, numCont, maxRun, staged = 0;
	float		   eCoeff = 1, i1Coeff = 1, i2Coeff = 1;
	int            xsobel7x7[49] = { -3,   -2,   -1,   0,   1,  2,  3,
									 -4,   -3,   -2,   0,   2,  3,  4,
									 -5,   -4,   -3,   0,   3,  4,  5,
									 -6,   -5,   -4,   0,   4,  5,  6,
									 -5,   -4,   -3,   0,   3,  4,  5,
									 -4,   -3,   -2,   0,   2,  3,  4,
									 -3,   -2,   -1,   0,   1,  2,  3 };
	
	int            ysobel7x7[49] = { 3,    4,    5,   6,   5,  4,  3,
									 2,    3,    4,   5,   4,  3,  2,
									 1,    2,    3,   4,   3,  2,  1,
									 0,    0,    0,   0,   0,  0,  0,
								    -1,   -2,   -3,  -4,  -3, -2, -1,
									-2,   -3,   -4,  -5,  -4, -3, -2,
									-3,   -4,   -5,  -6,  -5, -4, -3 };
	int            xsobel3x3[9] = { -1,  0,  1,
								-2,  0,  2,
								-1,  0,  1 };

	int            ysobel3x3[9] = { 1,  2,  1,
									 0,  0,  0,
									-1, -2, -1 };
	contType = 0;

	ThreadRunning = 1;
	numCont = count; // save number of contour points
	//structID = structCount; // save struct ID for altering values later
	image = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	for (j = 0; j < COLS * ROWS; j++) {
		image[j] = OriginalImage[j];
	}

	intEnergy1 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	intEnergy2 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	sumEnergy  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int1  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int2  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	use_ext    = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	
	/* dynamic active contouring -----------------------------------------------*/
	maxRun = 100;
	
	i1Coeff = 2;
	i2Coeff = 1.2;
	eCoeff  = 2;
	
	/*--------------------------------------------------------------------------*/

	/* read contour points -----------------------------------------------------*/
	
	colsCont = (int*)calloc(numCont, sizeof(int));
	rowsCont = (int*)calloc(numCont, sizeof(int));

	for (i = 0; i < numCont; i++) {
		colsCont[i] = xContPts[i];
		rowsCont[i] = yContPts[i];
	}
	/*--------------------------------------------------------------------------*/

	/* plot contour points -----------------------------------------------------*/
	origContour = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	for (r = 0; r < ROWS; r++) {
		for (c = 0; c < COLS; c++) {
			origContour[r * COLS + c] = image[r * COLS + c];
		}
	}

	for (i = 0; i < numCont; i++) {
		for (r = -(WINDOW / 2); r <= (WINDOW / 2); r++) {
			for (c = -(WINDOW / 2); c <= (WINDOW / 2); c++) {
				if (r == 0 || c == 0)
					origContour[(rowsCont[i] + r) * COLS + (colsCont[i] + c)] = 0;
			}
		}
	}
	/*--------------------------------------------------------------------------*/

	/* find external energy/ Sobel convolution ---------------------------------*/
	//sum=0;
	extEnergy1 = (float*)calloc(ROWS * COLS, sizeof(float));
	extEnergy2 = (float*)calloc(ROWS * COLS, sizeof(float));
	norm_ext = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));

	//x-axis sobel
	for (r = (WINDOW2 / 2); r < ROWS - (WINDOW2 / 2); r++) {
		for (c = (WINDOW2 / 2); c < COLS - (WINDOW2 / 2); c++) {
			sum = 0;
			sum2 = 0;
			for (r2 = -(WINDOW2 / 2); r2 <= (WINDOW2 / 2); r2++) {
				for (c2 = -(WINDOW2 / 2); c2 <= (WINDOW2 / 2); c2++) {
					sum += image[(r + r2) * COLS + (c + c2)] * xsobel3x3[(r2 + (WINDOW2 / 2)) * WINDOW2 + (c2 + (WINDOW2 / 2))];
					sum2 += image[(r + r2) * COLS + (c + c2)] * ysobel3x3[(r2 + (WINDOW2 / 2)) * WINDOW2 + (c2 + (WINDOW2 / 2))];
				}
			}
			extEnergy1[r * COLS + c] = sum;
			extEnergy2[r * COLS + c] = sum2;
		}
	}

	// get min/max
	for (r = 0; r <= ROWS - 1; r++) {
		for (c = 0; c <= COLS - 1; c++) {
			if (r == 0 && c == 0) {
				min_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
				max_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}

			if (sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) < min_val) {
				min_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}
			if (sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) > max_val) {
				max_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}
		}
	}

	for (r = 0; r <= ROWS - 1; r++) {
		for (c = 0; c <= COLS - 1; c++) {
			// use the normalization formula to normalize msf to an 8-bit image
			norm_ext[r * COLS + c] = (((sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) - min_val) * 255) / (max_val - (double)min_val));
		}
	}
	/*--------------------------------------------------------------------------*/

	/*keep contouring until done------------------------------------------------*/
	cleared = 0;
	for (run = 0; run < maxRun; run++) { 
		if (cleared == 1)
			break;

		// wait for enable contour to run
		while(EnableContour == 0){}

		avgDist = 0;
		// get avg distance for internal energy calc
		for (i = 0; i < numCont; i++) {
			// check final point to initial poiunt since the contour connects x=c
			if (i == numCont - 1) {
				nextPtx = colsCont[0];
				nextPty = rowsCont[0];
			}
			else {
				nextPtx = colsCont[i + 1];
				nextPty = rowsCont[i + 1];
			}

			avgDist += sqrt(pow(((double)colsCont[i] - (double)nextPtx), 2) + pow(((double)rowsCont[i] - (double)nextPty), 2));
		}

		avgDist /= numCont;

		for (i = 0; i < numCont; i++) {
			
			// reset all energies to 0
			for (j = 0; j < WINDOW * WINDOW; j++) {
				intEnergy1[j] = 0;
				intEnergy2[j] = 0;
				sumEnergy [j] = 0;
				norm_int1 [j] = 0;
				norm_int2 [j] = 0;
				use_ext   [j] = 0;
			}

			if (i == numCont - 1) {
				nextPtx = colsCont[0];
				nextPty = rowsCont[0];
			}
			else {
				nextPtx = colsCont[i + 1];
				nextPty = rowsCont[i + 1];
			}

			//check internal energies
			for (r = -(WINDOW/2); r <= (WINDOW/2); r++) {
				for (c = -(WINDOW/2); c <= (WINDOW/2); c++) {
					// distance between points
					intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					if (r == -(WINDOW/2) && c == -(WINDOW/2)) {
						min_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
						max_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}

					if (pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2) > max_val) {
						max_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}
					if (pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2) < min_val) {
						min_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}
					// avg deviation
					intEnergy2[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					if (r == -(WINDOW/2) && c == -(WINDOW/2)) {
						min_val2 = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
						max_val2 = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}

					if (pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2) > max_val2) {
						max_val2 = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}
					if (pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2) < min_val2) {
						min_val2 = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}
				}
			}

			// normalize internal energies
			for (r = 0; r < WINDOW; r++) {
				for (c = 0; c < WINDOW; c++) {
					norm_int1[r * WINDOW + c] = ((intEnergy1[r * WINDOW + c] - min_val)) / (max_val - min_val);
					norm_int2[r * WINDOW + c] = ((intEnergy2[r * WINDOW + c] - min_val2)) / (max_val2 - min_val2);
				}
			}

			// get external energy for point
			for (r = -(WINDOW/2); r <= (WINDOW/2); r++) {
				for (c = -(WINDOW/2); c <= (WINDOW/2); c++) {
					use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = ((float)norm_ext[(rowsCont[i] + r) * COLS + (colsCont[i] + c)] / 255);
					use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = sqrt(pow((use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] - (double)1), 2));
				}
			}

			tempRow = 0;
			tempCol = 0;
			
			
			// stage to ensure best contouring
			if (run > 20 && staged == 0)
			{
				
				eCoeff /= 1.5;
				i1Coeff /= 1.2;
				i2Coeff *= 1.2;
				staged = 1;
				
				
				//sprintf(str,"Staged\n");
				//OutputDebugString(str);
			}
			
			
			// sum all energies, find min, and move to that point
			for (r = 0; r < WINDOW; r++) {
				for (c = 0; c < WINDOW; c++) {
					sumEnergy[r * WINDOW + c] = eCoeff*use_ext[r * WINDOW + c] + i1Coeff*norm_int1[r * WINDOW + c] + i2Coeff*norm_int2[r * WINDOW + c];
					if (r == 0 && c == 0) {
						min_val = sumEnergy[r * WINDOW + c];
						tempRow = rowsCont[i] + (-(WINDOW/2) + r);
						tempCol = colsCont[i] + (-(WINDOW/2) + c);
					}

					if (sumEnergy[r * WINDOW + c] < min_val) {
						min_val = sumEnergy[r * WINDOW + c];
						tempRow = rowsCont[i] + (-(WINDOW/2) + r);
						tempCol = colsCont[i] + (-(WINDOW/2) + c);
					}
				}
			}

			rowsCont[i] = tempRow;
			colsCont[i] = tempCol;

		}
		
		FillStruct(colsCont, rowsCont, 0, numCont);
		hDC = GetDC(MainWnd);
		PaintImage();

		// plot contour points for this run
		
		for (i = 0; i < numCont; i++) {
			for (r = -3; r <= 3; r++) {
				for (c = -3; c <= 3; c++) {
					if (r == 0 || c == 0)
						SetPixel(hDC, colsCont[i] + c, rowsCont[i] + r, RGB(255, 0, 0));	/* color the cursor position red */
				}
			}
		}
		
		Sleep(50);
	}
	for (i = 0; i < numCont; i++) {
		for (r = -3; r <= 3; r++) {
			for (c = -3; c <= 3; c++) {
				if (r == 0 || c == 0)
					SetPixel(hDC, colsCont[i] + c, rowsCont[i] + r, RGB(0, 0, 255));	/* color the cursor position blue */
			}
		}
	}
	FillStruct(colsCont, rowsCont, 1, numCont);
	/*--------------------------------------------------------------------------*/

	/* save original image -----------------------------------------------------*/
	fpt = fopen("sobelImage.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(norm_ext, COLS* ROWS, 1, fpt);
	fclose(fpt);
	/*--------------------------------------------------------------------------*/
	
	for (i = 0; i < numCont; i++) {
		if (ThreadRunning != 0) {
			xContPts[i] = colsCont[i];
			yContPts[i] = rowsCont[i];
		}
	}

	/* free all pointers -------------------------------------------------------*/
	free(image);
	free(norm_ext);
	free(origContour);
	free(norm_int1);
	free(norm_int2);
	free(use_ext);
	free(colsCont);
	free(rowsCont);
	free(intEnergy1);
	free(intEnergy2);
	free(extEnergy1);
	free(extEnergy2);
	free(sumEnergy);
	/*--------------------------------------------------------------------------*/
}

void BubblePoints()
{
	FILE* fpt;
	HDC			   hDC;
	unsigned char  * image, * norm_ext, * origContour, * finalContour;
	char		   header[320];
	int		       run, r2, c2, index;
	int            r, c, i, j, temp1, temp2, * colsCont, * rowsCont;
	float		   * intEnergy1, * intEnergy2, * intEnergy3, * intEnergy4, * extEnergy1, * extEnergy2, avgDist = 0, * sumEnergy, sum2, min_val, max_val, min_val2, max_val2, min_val3, max_val3, sum;
	float		   * norm_int1, * norm_int2, * norm_int3, * norm_int4, * use_ext, min_val4, max_val4;
	int            nextPtx = 0, nextPty = 0, tempRow, tempCol, numCont, maxRun, staged = 0, prevPtx = 0, prevPty = 0, *tempRowsCont, *tempColsCont;
	float		   eCoeff = 1, i1Coeff = 1, i2Coeff = 1, i3Coeff = 1, i4Coeff = 1, dist;
	long int	   avgx, avgy;
	int            xsobel7x7[49] = { -3,   -2,   -1,   0,   1,  2,  3,
									 -4,   -3,   -2,   0,   2,  3,  4,
									 -5,   -4,   -3,   0,   3,  4,  5,
									 -6,   -5,   -4,   0,   4,  5,  6,
									 -5,   -4,   -3,   0,   3,  4,  5,
									 -4,   -3,   -2,   0,   2,  3,  4,
									 -3,   -2,   -1,   0,   1,  2,  3 };

	int            ysobel7x7[49] = { 3,    4,    5,   6,   5,  4,  3,
									 2,    3,    4,   5,   4,  3,  2,
									 1,    2,    3,   4,   3,  2,  1,
									 0,    0,    0,   0,   0,  0,  0,
									-1,   -2,   -3,  -4,  -3, -2, -1,
									-2,   -3,   -4,  -5,  -4, -3, -2,
									-3,   -4,   -5,  -6,  -5, -4, -3 };

	int            xsobel3x3[9] = { -1,  0,  1,
									-2,  0,  2,
									-1,  0,  1 };

	int            ysobel3x3[9] = {  1,  2,  1,
									 0,  0,  0,
									-1, -2, -1 };
	contType = 1;
	ThreadRunning = 1;

	numCont = count; // save number of contour points
	//structID = structCount; // save struct ID for altering values later
	image = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	for (j = 0; j < COLS * ROWS; j++) {
		image[j] = OriginalImage[j];
	}
	sprintf(str, "numCont: %d\n", count);
	OutputDebugString(str);

	intEnergy1 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	intEnergy2 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	intEnergy3 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	intEnergy4 = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	sumEnergy  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int1  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int2  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int3  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	norm_int4  = (float*)calloc(WINDOW * WINDOW, sizeof(float));
	use_ext    = (float*)calloc(WINDOW * WINDOW, sizeof(float));

	/* dynamic active contouring -----------------------------------------------*/
	maxRun = 80;

	i1Coeff = .5; // bias to minimize movement outward from the point
	i2Coeff = -1; // bias to move away from avg point of the contours
	//i3Coeff = 0; // bias of expansion DEPRECATED
	i4Coeff = 1; // bias to keep points evenly spaced
	eCoeff  = 2; // gradiant detection

	/*--------------------------------------------------------------------------*/

	/* read contour points -----------------------------------------------------*/

	colsCont = (int*)calloc(numCont, sizeof(int));
	rowsCont = (int*)calloc(numCont, sizeof(int));
	tempColsCont = (int*)calloc(numCont, sizeof(int));
	tempRowsCont = (int*)calloc(numCont, sizeof(int));

	for (i = 0; i < numCont; i++) {
		colsCont[i] = xContPts[i];
		rowsCont[i] = yContPts[i];
	}
	/*--------------------------------------------------------------------------*/

	/* plot contour points -----------------------------------------------------*/
	origContour = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	for (r = 0; r < ROWS; r++) {
		for (c = 0; c < COLS; c++) {
			origContour[r * COLS + c] = image[r * COLS + c];
		}
	}
	
	for (i = 0; i < numCont; i++) {
		for (r = -(WINDOW / 2); r <= (WINDOW / 2); r++) {
			for (c = -(WINDOW / 2); c <= (WINDOW / 2); c++) {
				if (r == 0 || c == 0)
					origContour[(rowsCont[i] + r) * COLS + (colsCont[i] + c)] = 0;
			}
		}
	}
	/*--------------------------------------------------------------------------*/

	/* find external energy/ Sobel convolution ---------------------------------*/
	//sum=0;
	extEnergy1 = (float*)calloc(ROWS * COLS, sizeof(float));
	extEnergy2 = (float*)calloc(ROWS * COLS, sizeof(float));
	norm_ext = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));

	//x-axis sobel
	for (r = (WINDOW2 / 2); r < ROWS - (WINDOW2 /2); r++) {
		for (c = (WINDOW2 /2); c < COLS - (WINDOW2 /2); c++) {
			sum = 0;
			sum2 = 0;
			for (r2 = -(WINDOW2 / 2); r2 <= (WINDOW2 / 2); r2++) {
				for (c2 = -(WINDOW2 / 2); c2 <= (WINDOW2 / 2); c2++) {
					sum  += image[(r + r2) * COLS + (c + c2)] * xsobel3x3[(r2 + (WINDOW2 / 2)) * WINDOW2 + (c2 + (WINDOW2 / 2))];
					sum2 += image[(r + r2) * COLS + (c + c2)] * ysobel3x3[(r2 + (WINDOW2 / 2)) * WINDOW2 + (c2 + (WINDOW2 / 2))];
				}
			}
			extEnergy1[r * COLS + c] = sum;
			extEnergy2[r * COLS + c] = sum2;
		}
	}

	// get min/max
	for (r = 0; r <= ROWS - 1; r++) {
		for (c = 0; c <= COLS - 1; c++) {
			if (r == 0 && c == 0) {
				min_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
				max_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}

			if (sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) < min_val) {
				min_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}
			if (sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) > max_val) {
				max_val = sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2));
			}
		}
	}

	for (r = 0; r <= ROWS - 1; r++) {
		for (c = 0; c <= COLS - 1; c++) {
			// use the normalization formula to normalize msf to an 8-bit image
			norm_ext[r * COLS + c] = (((sqrt(pow(extEnergy1[r * COLS + c], 2) + pow(extEnergy2[r * COLS + c], 2)) - min_val) * 255) / ((double)max_val - (double)min_val));
		}
	}
	/*--------------------------------------------------------------------------*/

	/*keep contouring until done------------------------------------------------*/
	cleared = 0;
	for (run = 0; run < maxRun; run++) {
		if (cleared == 1 || ThreadRunning == 0)
			break;

		// wait for enable contour to run
		while (EnableContour == 0) {}

		avgDist = 0;
		avgx = 0;
		avgy = 0;
		// get avg distance for internal energy calc
		for (i = 0; i < numCont; i++) {
			// check final point to initial poiunt since the contour connects x=c
			if (i == numCont - 1) {
				nextPtx = colsCont[0];
				nextPty = rowsCont[0];
			}
			else {
				nextPtx = colsCont[i + 1];
				nextPty = rowsCont[i + 1];
			}

			avgx += colsCont[i];
			avgy += rowsCont[i];

			avgDist += sqrt(pow(((double)colsCont[i] - (double)nextPtx), 2) + pow(((double)rowsCont[i] - (double)nextPty), 2));
		}

		

		avgx    /= numCont;
		avgy    /= numCont;
		avgDist /= numCont;

		cleared = 0;
		for (i = 0; i < numCont; i++) {
			if (cleared == 1 || ThreadRunning == 0)
				break;
			// reset all energies to 0
			for (j = 0; j < WINDOW * WINDOW; j++) {
				intEnergy1[j] = 0;
				intEnergy2[j] = 0;
				intEnergy3[j] = 0;
				intEnergy4[j] = 0;
				sumEnergy[j] = 0;
				norm_int1[j] = 0;
				norm_int2[j] = 0;
				norm_int3[j] = 0;
				norm_int4[j] = 0;
				use_ext[j] = 0;
			}

			if (i == numCont - 1) {
				nextPtx = colsCont[0];
				nextPty = rowsCont[0];
			}
			else {
				nextPtx = colsCont[i + 1];
				nextPty = rowsCont[i + 1];
			}
			if (i == 0) {
				prevPtx = colsCont[numCont - 1];
				prevPty = rowsCont[numCont - 1];
			}
			else {
				prevPtx = colsCont[i - 1];
				prevPty = rowsCont[i - 1];
			}

			//check internal energies
			for (r = -(WINDOW / 2); r <= (WINDOW / 2); r++) {
				for (c = -(WINDOW / 2); c <= (WINDOW / 2); c++) {
					index = (r + (WINDOW / 2)) * WINDOW + (c + (WINDOW / 2));
					dist = pow(abs(colsCont[i] + c - avgx), 2) + pow(abs(rowsCont[i] + r - avgy), 2);

					if (r == 0 && c == 0){
						intEnergy1[index] = 0;
					}
					else {
						intEnergy1[index] = (pow(c, 2) + pow(r, 2));
					}
					
					intEnergy2[index] = dist;

					if (r == -(WINDOW / 2) && c == -(WINDOW / 2)) {
						min_val  = intEnergy1[index];
						max_val  = intEnergy1[index];
						min_val2 = intEnergy2[index];
						max_val2 = intEnergy2[index];
					}

					if (intEnergy1[index] > max_val) {
						max_val = intEnergy1[index];
					}
					if (intEnergy2[index] > max_val2) {
						max_val2 = intEnergy2[index];
					}
					if (intEnergy1[index] < min_val) {
						min_val = intEnergy1[index];
					}
					if (intEnergy2[index] < min_val2) {
						min_val2 = intEnergy2[index];
					}

					// distance between point and next point
					/*
					intEnergy1[(r + (WINDOW / 2)) * WINDOW + (c + (WINDOW / 2))] = pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					if (r == -(WINDOW / 2) && c == -(WINDOW / 2)) {
						min_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
						max_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}

					if (pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2) > max_val) {
						max_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}
					if (pow(((colsCont[i] + (double)c) - (double)nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2) < min_val) {
						min_val = pow(((colsCont[i] + (double)c) - nextPtx), 2) + pow(((rowsCont[i] + (double)r) - (double)nextPty), 2);
					}

					// expansion force
					intEnergy3[(r + (WINDOW / 2)) * WINDOW + (c + (WINDOW / 2))] = pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2);
					if (r == -(WINDOW / 2) && c == -(WINDOW / 2)) {
						min_val3 = pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2);
						max_val3 = pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2);
					}

					if (pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2) > max_val3) {
						max_val3 = pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2);
					}
					if (pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2) < min_val3) {
						min_val3 = pow(((centerx + (double)c) - (double)nextPtx), 2) + pow(((centery + (double)r) - (double)nextPty), 2);
					}

					// avg deviation
					intEnergy2[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = pow((avgDist - sqrt(intEnergy1[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					if (r == -(WINDOW/2) && c == -(WINDOW/2)) {
						min_val2 = pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
						max_val2 = pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}

					if (pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2) > max_val2) {
						max_val2 = pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}
					if (pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2) < min_val2) {
						min_val2 = pow((avgDist - sqrt(intEnergy3[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))])), 2);
					}
					*/
					// maintain distance between points
					
					intEnergy4[(r + (WINDOW / 2)) * WINDOW + (c + (WINDOW / 2))] = pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)prevPty + (double)r) - (double)nextPty), 2);
					if (r == -(WINDOW / 2) && c == -(WINDOW / 2)) {
						min_val4 = pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2);
						max_val4 = pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2);
					}

					if (pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2) > max_val4) {
						max_val4 = pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2);
					}
					if (pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2) < min_val4) {
						min_val4 = pow((((double)prevPtx + (double)c) - (double)nextPtx), 2) + pow((((double)nextPty + (double)r) - (double)nextPty), 2);
					}
					
					if (intEnergy4[index] > max_val4) {
						min_val4 = intEnergy4[index];
					}
					if (intEnergy4[index] < min_val4) {
						min_val4 = intEnergy4[index];
					}
				}
			}

			// normalize internal energies
			for (r = 0; r < WINDOW; r++) {
				for (c = 0; c < WINDOW; c++) {
					// reverse distance effects for inflation
					norm_int1[r * WINDOW + c] = ((intEnergy1[r * WINDOW + c] - min_val)) / (max_val - min_val);
					//norm_int1[r * WINDOW + c] = abs(norm_int1[r * WINDOW + c] - 1);

					norm_int2[r * WINDOW + c] = ((intEnergy2[r * WINDOW + c] - min_val2)) / (max_val2 - min_val2);

					//norm_int3[r * WINDOW + c] = ((intEnergy3[r * WINDOW + c] - min_val3)) / (max_val3 - min_val3);
					//norm_int3[r * WINDOW + c] = abs(norm_int3[r * WINDOW + c] - 1);

					norm_int4[r * WINDOW + c] = ((intEnergy4[r * WINDOW + c] - min_val4)) / (max_val4 - min_val4);
				}
			}

			// get external energy for point
			for (r = -(WINDOW/2); r <= (WINDOW/2); r++) {
				for (c = -(WINDOW/2); c <= (WINDOW/2); c++) {
					use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = ((float)norm_ext[(rowsCont[i] + r) * COLS + (colsCont[i] + c)] / 255);
					use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] = sqrt(pow((use_ext[(r + (WINDOW/2)) * WINDOW + (c + (WINDOW/2))] - (double)1), 2));
				}
			}

			tempRow = 0;
			tempCol = 0;


			// stage to ensure best contouring
			if (run > 20 && staged == 0)
			{
				eCoeff *= 1;
				i1Coeff *= 1;
				i2Coeff *= 1;
				staged = 1;

				//sprintf(str, "Staged\n");
				//OutputDebugString(str);
			}


			// sum all energies, find min, and move to that point
			for (r = 0; r < WINDOW; r++) {
				for (c = 0; c < WINDOW; c++) {
					sumEnergy[r * WINDOW + c] = eCoeff * use_ext[r * WINDOW + c] + i1Coeff * norm_int1[r * WINDOW + c] + i2Coeff * norm_int2[r * WINDOW + c]; //+ i3Coeff * norm_int3[r * WINDOW + c] + i4Coeff * norm_int4[r * WINDOW + c];
					if (r == 0 && c == 0) {
						min_val = sumEnergy[r * WINDOW + c];
						tempRow = rowsCont[i] + (-(WINDOW/2) + r);
						tempCol = colsCont[i] + (-(WINDOW/2) + c);
					}

					if (sumEnergy[r * WINDOW + c] < min_val) {
						min_val = sumEnergy[r * WINDOW + c];
						tempRow = rowsCont[i] + (-(WINDOW/2) + r);
						tempCol = colsCont[i] + (-(WINDOW/2) + c);
					}
				}
			}

			//rowsCont[i] = tempRow;
			//colsCont[i] = tempCol;
			if (tempRow > ROWS - 4)
			{
				tempRow = ROWS - 4;
			} 
			else if(tempRow < 3)
			{
				tempRow = 3;
			}

			if (tempCol > COLS - 4)
			{
				tempCol = COLS - 4;
			}
			else if (tempCol < 3)
			{
				tempCol = 3;
			}

			tempRowsCont[i] = tempRow;
			tempColsCont[i] = tempCol;

		}
		for (i = 0; i < numCont; i++) 
		{
			rowsCont[i] = tempRowsCont[i];
			colsCont[i] = tempColsCont[i];
		}

		//xContPts = colsCont;
		//yContPts = rowsCont;
		FillStruct(colsCont, rowsCont, 0, numCont);
		hDC = GetDC(MainWnd);
		PaintImage();

		// plot contour points for this run

		for (i = 0; i < numCont; i++) {
			for (r = -3; r <= 3; r++) {
				for (c = -3; c <= 3; c++) {
					if (r == 0 || c == 0)
						if (ThreadRunning == 1) {
							SetPixel(hDC, colsCont[i] + c, rowsCont[i] + r, RGB(255, 0, 0));	/* color the cursor position red */
						}
				}			
			}
		}

		Sleep(50);
	}
	for (i = 0; i < numCont; i++) {
		for (r = -3; r <= 3; r++) {
			for (c = -3; c <= 3; c++) {
				if (r == 0 || c == 0)
					if (ThreadRunning == 1) {
						SetPixel(hDC, colsCont[i] + c, rowsCont[i] + r, RGB(0, 0, 255));	/* color the cursor position blue */
					}
			}
		}
	}
	//FillStruct(colsCont, rowsCont, 1, numCont);
	/*--------------------------------------------------------------------------*/
	for (i = 0; i < numCont; i++) {
		if (ThreadRunning != 0) {
			xContPts[i] = colsCont[i];
			yContPts[i] = rowsCont[i];
		}
	}

	/* save original image -----------------------------------------------------*/
	/*
	fpt = fopen("sobelImage2.ppm", "wb");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(norm_ext, COLS* ROWS, 1, fpt);
	fclose(fpt);
	*/
	/*--------------------------------------------------------------------------*/

	/* free all pointers -------------------------------------------------------*/
	free(image);
	free(norm_ext);
	free(origContour);
	free(norm_int1);
	free(norm_int2);
	free(use_ext);
	free(colsCont);
	free(rowsCont);
	free(intEnergy1);
	free(intEnergy2);
	free(extEnergy1);
	free(extEnergy2);
	free(sumEnergy);
	/*--------------------------------------------------------------------------*/
}
