// Program WinMenu.cpp

// TODO LIST
// 1. COMMENTS
// 2. SEPARATE INTO LAYERS, READ TERMINAL EMULATION FILE

#define STRICT
#include "Application.h"
#include "Session.h"

#pragma warning (disable: 4096)
static TCHAR Name[] = TEXT("Comm Shell");
char str[80] = "";
LPCTSTR	lpszCommName = TEXT("COM1");
COMMCONFIG	cc;
HANDLE hComm;
COMMTIMEOUTS timeouts;
char buffer[1];
DWORD read, written;
HDC hdc;
static unsigned x_cordinate = 0;
static unsigned y_cordinate = 0;
int ch;
MSG Msg;
BOOL connected = FALSE;
HWND hwnd;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	WNDCLASSEX Wcl;

	OutputDebugStringA("hello");

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		600, 400, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}



	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	int response;
	PAINTSTRUCT paintstruct;

	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_COMMANDMODE:
			cc.dwSize = sizeof(COMMCONFIG);
			cc.wVersion = 0x100;
			if (!CommConfigDialog(lpszCommName, hwnd, &cc))
				break;
			
			break;
		case IDM_CONNECTMODE:
			connect(lpszCommName, hComm, cc,  timeouts, connected, hwnd);
			break;
		case IDM_DISCONNECT:
			disconnect(connected, hComm);
			break;
		}
		break;

	case WM_CHAR:	// Process keystroke
		hdc = GetDC(hwnd);			 // get device context

		// TODO: IMPLEMENT DELETE BUTTON

		char temp[1];
		temp[0] = (char)wParam; // Convert char to string
		//TextOut(hdc, 10 * x_cordinate, y_cordinate, str, strlen(str)); // output character	
		//x_cordinate++; // increment the screen x-coordinate
		WriteFile(hComm, temp, sizeof(char), &written, NULL);
		ReleaseDC(hwnd, hdc); // Release device context
		break;

	case WM_PAINT:		// Process a repaint message
		hdc = BeginPaint(hwnd, &paintstruct); // Acquire DC
		TextOut(hdc, 0, 0, str, strlen(str)); // output character
		EndPaint(hwnd, &paintstruct); // Release DC
		break;

	case WM_DESTROY:	// Terminate program
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI readLoop(LPVOID hwnd) {
	while (connected) {
		ReadFile(hComm, buffer, sizeof(buffer), &read, NULL);
		if (read) {
			hdc = GetDC((HWND)hwnd);
			TextOut(hdc, 10 * x_cordinate, y_cordinate, buffer, strlen(buffer)); // output character	
			x_cordinate++; // increment the screen x-coordinate
			ReleaseDC((HWND)hwnd, hdc); // Release device context
		}
	}
	ExitThread(0);
}