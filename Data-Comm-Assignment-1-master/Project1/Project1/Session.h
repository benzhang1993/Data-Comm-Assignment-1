#ifndef Session_h
#define Session_h

#include <windows.h>
BOOL connect(LPCTSTR lpszCommName, HANDLE &hComm, COMMCONFIG &cc, COMMTIMEOUTS &timeouts, BOOL &connected, HWND &hwnd);
BOOL disconnect(BOOL &connected, HANDLE &hComm);

#endif