// Program WinMenu.cpp

// TODO LIST
// 1. COMMENTS
// 2. SEPARATE INTO LAYERS, READ TERMINAL EMULATION FILE

#define STRICT

#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include <conio.h>
#include <iostream>
#include <thread>

static TCHAR Name[] = TEXT("Comm Shell");
char str[80] = "";
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI readLoop(LPVOID);
VOID writeLoop(MSG);
BOOL connect();
VOID disconnect();

#pragma warning (disable: 4096)

LPCTSTR	lpszCommName = TEXT("com1");
COMMCONFIG	cc;
HANDLE hComm;
COMMPROP CommProp;
DCB dcb;
COMMTIMEOUTS timeouts;
char buffer[1];
DWORD read, written;
HDC hdc;
static unsigned x_cordinate = 0;
static unsigned y_cordinate = 0;
int ch;
MSG Msg;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	HWND hwnd;
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
	
	DWORD threadId;
	CreateThread(NULL, 0, readLoop, (LPVOID) hwnd, 0, &threadId);
	writeLoop(Msg);

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
			GetCommConfig(hComm, &cc, &cc.dwSize);
			if (!CommConfigDialog(lpszCommName, hwnd, &cc))
				break;
			break;
		case IDM_CONNECTMODE:
			connect();
			break;
		case IDM_DISCONNECT:
			disconnect();
			break;
		}
		break;

	case WM_CHAR:	// Process keystroke
		hdc = GetDC(hwnd);			 // get device context

		// TODO: IMPLEMENT DELETE BUTTON and enter button

		sprintf_s(str, "%c", (char)wParam); // Convert char to string
		WriteFile(hComm, str, 1, &written, NULL);
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
	while (1) {
		ReadFile(hComm, buffer, sizeof(buffer), &read, NULL);
		if (read) {
			hdc = GetDC((HWND) hwnd);
			TextOut(hdc, 10 * x_cordinate, y_cordinate, buffer, strlen(buffer)); // output character	
			x_cordinate++; // increment the screen x-coordinate
			ReleaseDC((HWND) hwnd, hdc); // Release device context
		}
	}
}

VOID writeLoop(MSG Msg) {
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

BOOL connect() {

	// Initializes Com Port
	if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, 0, NULL))
		== INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// Initializes Serial Port
	if (!SetupComm(hComm, 16, 16)) {
		MessageBox(NULL, TEXT("Error initializing serial port:"), TEXT(""), MB_OK);
		return FALSE;
	};

	// Gets COM port properties and store in lpCommProp
	if (!GetCommProperties(hComm, &CommProp)) {
		MessageBox(NULL, TEXT("Error initializing COMMPROP:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// Gets current DCB, used for configuring serial port properties
	if (!GetCommState(hComm, &dcb)) {
		MessageBox(NULL, TEXT("Error getting COMM state:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// Changes Serial Port settings, sets 9600 bps, no parity, 8 data bits, and 1 stop bit
	if (!BuildCommDCB("24,N,8,1", &dcb)) {
		MessageBox(NULL, TEXT("Error building COMM DCB:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// Sets the Com state using initial contents of DCB
	if (!SetCommState(hComm, &dcb)) {
		MessageBox(NULL, TEXT("Unable to set Com state:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// sets timeout
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;
}

VOID disconnect() {
	CloseHandle(hComm);
}