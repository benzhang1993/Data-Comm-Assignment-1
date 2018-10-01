#include "Application.h"
#include "Session.h"

DCB dcb;
COMMPROP CommProp;

BOOL connect(LPCTSTR lpszCommName, HANDLE &hComm, COMMCONFIG &cc, COMMTIMEOUTS &timeouts, BOOL &connected, HWND &hwnd) {

	// Initializes Com Port
	if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, 0, NULL))
		== INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
		return FALSE;
	}

	// sets timeout
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;

	connected = TRUE;
	CreateThread(NULL, 0, readLoop, hwnd, 0, NULL);

}

BOOL disconnect(BOOL &connected, HANDLE &hComm) {
	connected = FALSE;
	CloseHandle(hComm);
	return true;
}