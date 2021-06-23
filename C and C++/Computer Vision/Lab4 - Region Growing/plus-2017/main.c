
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
	ShowPixelCoords = 0;
	ShowBigDots = 0;
	intensity_thresh = 10;
	centroid_thresh = 10;
	intensity_avg = 0;
	centroid_avgx = 0;
	centroid_avgy = 0;
	blue = 0;
	green = 0;
	red = 255;
	PlayMode = 0;
	StepMode = 0;
	count = 0;
	done = 1;
	getNewTemp = 1;
	ThreadRunning = 0;

	strcpy(filename, "");
	OriginalImage	= NULL;
	inRegion		= NULL;
	tempRegion		= NULL;

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
	int					BYTES, xPos, yPos;

	
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords = (ShowPixelCoords + 1) % 2;
			PaintImage();
			break;

		case ID_DISPLAY_BIGDOTS:
			ShowBigDots = (ShowBigDots + 1) % 2;
			PaintImage();
			break;

		case ID_REGIONGROW_PLAY:
			PlayMode = (PlayMode + 1) % 2;
			if (StepMode == 1) {
				StepMode = 0;
			}
			ShowBigDots = 0;
			ShowPixelCoords = 0;
			break;

		case ID_REGIONGROW_STEP:
			StepMode = (StepMode + 1) % 2;
			if (PlayMode == 1) {
				PlayMode = 0;
			}
			ShowBigDots = 0;
			ShowPixelCoords = 0;
			stepEnabled = 1;

			break;

		case ID_REGIONGROW_CLEARREGIONS:

			if (ROWS != NULL && COLS != NULL)
				CleanRegion();

			PaintImage();
			break;

		case ID_REGIONGROW_PREDICATES:
			
			predDlg = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_PRED), hWnd, (DLGPROC)AboutDlgProc);
			ShowWindow(predDlg, SW_SHOW);

			SetWindowText(predDlg, "Predicates");

			SetDlgItemText(predDlg, IDC_Centroid, "Centroid Value");
			SetDlgItemText(predDlg, IDC_INTENSITY, "Absolute Intensity Threshold");

			SetDlgItemInt(predDlg, IDC_EDIT_INTENSITY, intensity_thresh, 1);
			SetDlgItemInt(predDlg, IDC_EDIT_CENTROID, centroid_thresh, 1);
			break;

		case ID_REGIONGROW_COLOR:
			colorDlg = CreateDialog((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_COLOR), hWnd, (DLGPROC)AboutDlgProc);
			ShowWindow(colorDlg, SW_SHOW);

			SetWindowText(colorDlg, "Colors");

			SetDlgItemInt(colorDlg, IDC_EDIT_BLUE, blue, 1);
			SetDlgItemInt(colorDlg, IDC_EDIT_RED, red, 1);
			SetDlgItemInt(colorDlg, IDC_EDIT_GREEN, green, 1);

			SetDlgItemText(colorDlg, IDC_BLUE, "Blue");
			SetDlgItemText(colorDlg, IDC_RED, "Red");
			SetDlgItemText(colorDlg, IDC_GREEN, "Green");
			SetDlgItemText(colorDlg, IDC_COLOR_STATEMENT, "Values must be between 0-255:");
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage = NULL;
			}

			memset(&(ofn), 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			filename[0] = 0;
			ofn.nMaxFile = MAX_FILENAME_CHARS;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!(GetOpenFileName(&ofn)) || filename[0] == '\0')
				break;		/* user cancelled load */
			if ((fpt = fopen(filename, "rb")) == NULL)
			{
				MessageBox(NULL, "Unable to open file", filename, MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
			if (strcmp(header, "P5") != 0 || BYTES != 255)
			{
				MessageBox(NULL, "Not a PPM (P5 greyscale) image", filename, MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			OriginalImage = (unsigned char*)calloc(ROWS * COLS, 1);

			CleanRegion(); // calloc inRegion if needed

			header[0] = fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage, 1, ROWS * COLS, fpt);
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

	case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
		// get mouse click x & y positions and reset values for region fill
		if ((PlayMode == 1 || StepMode == 1) && OriginalImage != NULL) {
			ThreadRunning = 1;
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			hDC = GetDC(MainWnd);
			CleanRegion();

			// set inital values
			intensity_avg = OriginalImage[yPos * COLS + xPos];
			centroid_avgx = xPos;
			centroid_avgy = yPos;
			inRegion[yPos * COLS + xPos] = 1;
			SetPixel(hDC, xPos, yPos, RGB(red, green, blue));	/* color the cursor position red */
			count = 1;
			if (StepMode == 1) {
				stepEnabled = 1;
			}

			ReleaseDC(MainWnd, hDC);
			if (StepMode == 1 || PlayMode == 1) {
				_beginthread(RegionFill, 0, MainWnd);
			}
		}
		
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_MOUSEMOVE:
		if (ShowPixelCoords == 1)
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
				if (ShowBigDots == 1) {
					for (r = -2; r <= 2; r++) {
						for (c = -2; c <= 2; c++) {
							SetPixel(hDC, xPos + r, yPos + c, RGB(255, 0, 0));	/* color the cursor position red */
						}
					}
					Sleep(100);
				}
				else {
					SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */
				}
				ReleaseDC(MainWnd, hDC);
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
		if (wParam == 'j' || wParam == 'J')				/* step mode steps a single pixel*/
		{	
			if (stepEnabled == 0) {
				stepEnabled = 1;
			}
			
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

	if (ShowBigDots == 1)
		CheckMenuItem(hMenu, ID_DISPLAY_BIGDOTS, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_DISPLAY_BIGDOTS, MF_UNCHECKED);

	if (PlayMode == 1) {
		CheckMenuItem(hMenu, ID_REGIONGROW_PLAY, MF_CHECKED);
	}
	else
		CheckMenuItem(hMenu, ID_REGIONGROW_PLAY, MF_UNCHECKED);

	if (StepMode == 1)
		CheckMenuItem(hMenu, ID_REGIONGROW_STEP, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_REGIONGROW_STEP, MF_UNCHECKED);

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
			PaintImage();
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
			}
			break;
		case IDOK_COLOR:
			blue = GetDlgItemInt(hwnd, IDC_EDIT_BLUE, NULL, 0);
			red = GetDlgItemInt(hwnd, IDC_EDIT_RED, NULL, 0);
			green = GetDlgItemInt(hwnd, IDC_EDIT_GREEN, NULL, 0);

			if (red == NULL) {
				red = 0;
			}
			if (green == NULL) {
				green = 0;
			}
			if (blue == NULL) {
				blue = 0;
			}
			//OutputDebugString(test);
			
			if (blue > 255 || blue < 0 || red > 255 || red < 0 || green > 255 || green < 0) {
				break;
			}
			else {
				EndDialog(hwnd, IDOK_COLOR);
			}
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		break;
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


void RegionFill() {
	int r, c, adjacent;
	HDC hDC;
	int lowerR, upperR, lowerC, upperC;
	
	tempRegion = (unsigned char*)calloc(ROWS * COLS, 1);
	
	done = 0;
	while (done == 0) {
		done = 1;

		for (r = 0; r < ROWS; r++) {
			for (c = 0; c < COLS; c++) {
				adjacent = 0;
				// check for adjacent pixel in region
				if (inRegion[r * COLS + c] != 1) {
					if (c != COLS - 1) {
						if (inRegion[r * COLS + (c + 1)] == 1) {
							adjacent = 1;
						}
					}
					if (c != 0) {
						if (inRegion[r * COLS + (c - 1)] == 1) {
							adjacent = 1;
						}
					}
					if (r != ROWS - 1) {
						if (inRegion[(r + 1) * COLS + c] == 1) {
							adjacent = 1;
						}
					}
					if (r != 0) {
						if (inRegion[(r - 1) * COLS + c] == 1) {
							adjacent = 1;
						}
					}
					if (r != ROWS - 1 && c != COLS - 1) {
						if (inRegion[(r + 1) * COLS + (c + 1)] == 1) {
							adjacent = 1;
						}
					}
					if (r != ROWS - 1 && c != 0) {
						if (inRegion[(r + 1) * COLS + (c - 1)] == 1) {
							adjacent = 1;

						}
					}
					if (r != 0 && c != COLS - 1) {
						if (inRegion[(r - 1) * COLS + (c + 1)] == 1) {
							adjacent = 1;
						}
					}
					if (r != 0 && c != 0) {
						if (inRegion[(r - 1) * COLS + (c - 1)] == 1) {
							adjacent = 1;
						}
					}
				}

				// if there is an adjacent pixel in region, check threshold conditions
				if (adjacent == 1) {
					if (abs(OriginalImage[r * COLS + c] - intensity_avg) <= intensity_thresh && abs(sqrt(pow(centroid_avgx - c, 2) + pow(centroid_avgy - r, 2))) <= centroid_thresh) {
						tempRegion[r * COLS + c] = 1;
						done = 0;
					}
				}
			}
		}

		// copy over newly added region pixels from temp
		if (done == 0) {
			for (r = 0; r < ROWS; r++) {
				for (c = 0; c < COLS; c++) {
					if (tempRegion[r * COLS + c] == 1) {
						tempRegion[r * COLS + c] = 0;
						inRegion[r * COLS + c] = 1;
						WeightAvgs(OriginalImage[r * COLS + c], c, r); /* adjust weights for pixels added to region */

						if (stepEnabled == 1) {
							stepEnabled = 0;
						}else if (PlayMode == 1) {
							Sleep(1);
						}

						// wait for stepMode or stepEnabled to be changed
						while (StepMode == 0 && PlayMode == 0) {}
						while (stepEnabled == 0 && StepMode == 1) {}

						hDC = GetDC(MainWnd);
						SetPixel(hDC, c, r, RGB(red, green, blue));	/* color the pixel position */
						ReleaseDC(MainWnd, hDC);
					}
				}
			}
		}
	}
	ThreadRunning = 0;
}
void WeightAvgs(int intensity, int centroidx, int centroidy) {
	intensity_avg *= count;
	centroid_avgx *= count;
	centroid_avgy *= count;

	intensity_avg += intensity;
	centroid_avgx += centroidx;
	centroid_avgy += centroidy;

	count++;

	intensity_avg /= count;
	centroid_avgx /= count;
	centroid_avgy /= count;
}

void CleanRegion() {
	if (inRegion != NULL) {
		free(inRegion);
		inRegion = (unsigned char*)calloc(ROWS * COLS, 1);
	}
	if (inRegion == NULL) {
		inRegion = (unsigned char*)calloc(ROWS * COLS, 1);
	}
	if (tempRegion != NULL) {
		free(tempRegion);
		tempRegion = (unsigned char*)calloc(ROWS * COLS, 1);
	}
	if (tempRegion == NULL) {
		tempRegion = (unsigned char*)calloc(ROWS * COLS, 1);
	}
}