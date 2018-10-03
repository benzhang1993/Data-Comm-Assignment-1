/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Physical.cpp - Contains the initialization function for the terminal application.
--
--
-- FUNCTIONS:
-- BOOL initialize_port(LPCTSTR lpszCommName, HANDLE &hComm)--
--
-- DATE: September 31st, 2018
--
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- NOTES:
-- The initilization function will create the handle for the COMMPORT and displays an error message if unsuccessful.
----------------------------------------------------------------------------------------------------------------------*/

#include "Application.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initialize_port
--
-- DATE: September 31st, 2018
-- DESIGNER: Ben Zhang
--
-- PROGRAMMER: Ben Zhang
--
-- INTERFACE: initialize_port(LPCTSTR lpszCommName, HANDLE &hComm)
--
-- RETURNS: BOOL.
--
-- NOTES:
-- Initializes the COM PORT and grabbing its handle. Creates an error message if unable to create the port.
----------------------------------------------------------------------------------------------------------------------*/
BOOL initialize_port(LPCTSTR lpszCommName, HANDLE &hComm) {
	if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, 0, NULL))
		== INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
		return FALSE;
	}
	else {
		return TRUE;
	}
}