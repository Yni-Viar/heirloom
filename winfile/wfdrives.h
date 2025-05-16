#pragma once

#include <windows.h>

typedef int DRIVE;
typedef int DRIVEIND;

BOOL CheckDrive(HWND hwnd, DRIVE drive, DWORD dwFunc);
void NewTree(DRIVE drive, HWND hWnd);
void GetDriveRect(DRIVEIND driveInd, PRECT prc);
LRESULT CALLBACK DrivesWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
