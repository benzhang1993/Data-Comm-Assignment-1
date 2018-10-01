#ifndef Application_h
#define Application_h

#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include <conio.h>
#include <iostream>
#include <thread>
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	DWORD WINAPI readLoop(LPVOID);
	VOID writeLoop(MSG);
#endif

