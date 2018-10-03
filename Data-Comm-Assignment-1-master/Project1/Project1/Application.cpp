/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Application.cpp - A program that reads and writes characters from and to another terminal through the
-- serial port.
--
--
-- FUNCTIONS:
-- int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
-- LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
-- DWORD WINAPI readLoop(LPVOID hwnd)
--
-- DATE: September 31st, 2018
--
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- NOTES:
-- The program is a graphical interfaced terminal that allows user to set up PORT configurations and connect to the
-- serial port. Once connected, the user is able to send/receive characters from and to the terminal. The user can press
-- the escape key or the disconnect menu button to disconnect from the connection.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
--
-- RETURNS: int.
--
-- NOTES:
-- The main function that initializes and creates the window as well as listen for Windows Messages.
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	WNDCLASSEX Wcl;

	// Initializing Window Class
	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	Wcl.hIconSm = NULL; 
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = TEXT("MYMENU"); 
	Wcl.cbClsExtra = 0;      
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	// Create Window
	hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		600, 400, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Main Windows Message processing loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: LRESULT.
--
-- NOTES:
-- Processes Windows Messages, handles the menu button functionalities, writes the the serial port if a character is
-- entered, repaints the window if required, and terminates the program appropriately.
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
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
		// Disconnect if escape key is pressed
		if (wParam == 27) {
			disconnect(connected, hComm);
			break;
		}
		hdc = GetDC(hwnd);		
		char temp[1];
		temp[0] = (char)wParam;
		// Writes character to COMMPORT
		WriteFile(hComm, temp, sizeof(char), &written, NULL);
		ReleaseDC(hwnd, hdc); 
		break;

	case WM_PAINT:		
		hdc = BeginPaint(hwnd, &paintstruct); 
		TextOut(hdc, 0, 0, str, strlen(str)); 
		EndPaint(hwnd, &paintstruct);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: readLoop
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: WINAPI readLoop(LPVOID hwnd)
--
-- RETURNS: DWORD.
--
-- NOTES:
-- Continously reads from serial port and displays the character on the terminal screen.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI readLoop(LPVOID hwnd) {
	while (connected) {
		ReadFile(hComm, buffer, sizeof(buffer), &read, NULL);
		if (read) {
			hdc = GetDC((HWND)hwnd);
			TextOut(hdc, 10 * x_cordinate, y_cordinate, buffer, strlen(buffer)); 	
			x_cordinate++; 
			ReleaseDC((HWND)hwnd, hdc); 
		}
	}
	ExitThread(0);
}