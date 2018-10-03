/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Session.cpp - Contains the connect and disconnect function for the terminal application.
--
--
-- FUNCTIONS:
-- BOOL connect(LPCTSTR lpszCommName, HANDLE &hComm, COMMCONFIG &cc, COMMTIMEOUTS &timeouts, BOOL &connected, HWND &hwnd) 
-- BOOL disconnect(BOOL &connected, HANDLE &hComm)
--
--
-- DATE: September 31st, 2018
--
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- NOTES:
-- The connect function will call the physical layer (Physical.cpp)'s initialize function to initialize the serial port,
-- and sets the timeouts for reading and writing. Once completed, it will create a thread to monitor PORT for reading
-- characters and displaying them on the screen.
-- The disconnect function will set the connect flag to false, so that the thread will end itself, and it will release the
-- handle for the COMMPORT.
----------------------------------------------------------------------------------------------------------------------*/

#include "Application.h"
#include "Session.h"
#include "Physical.h"

DCB dcb;
COMMPROP CommProp;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connect
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: connect(LPCTSTR lpszCommName, HANDLE &hComm, COMMCONFIG &cc, COMMTIMEOUTS &timeouts, BOOL &connected, HWND &hwnd) 
--
-- RETURNS: BOOL.
--
-- NOTES:
-- Initializes the serial port and sets timeouts for reading and writing.
-- Then create a thread for continously reading input.
----------------------------------------------------------------------------------------------------------------------*/
BOOL connect(LPCTSTR lpszCommName, HANDLE &hComm, COMMCONFIG &cc, COMMTIMEOUTS &timeouts, BOOL &connected, HWND &hwnd) {

	// Initializes Com Port
	initialize_port(lpszCommName, hComm);

	// sets timeout
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;

	// Creates thread for reading input
	connected = TRUE;
	CreateThread(NULL, 0, readLoop, hwnd, 0, NULL);

	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: disconnect
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: disconnect(BOOL &connected, HANDLE &hComm)
--
-- RETURNS: BOOL.
--
-- NOTES:
-- Sets connect flag to FALSE so the reading thread will end.
-- Also closes the handle for serial port.
----------------------------------------------------------------------------------------------------------------------*/
BOOL disconnect(BOOL &connected, HANDLE &hComm) {
	connected = FALSE;
	CloseHandle(hComm);
	return TRUE;
}