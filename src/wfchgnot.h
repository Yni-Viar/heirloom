#pragma once

#include <windows.h>

typedef int DRIVE;

void InitializeWatchList();
void ModifyWatchList(HWND hwndWatch, LPWSTR lpPath, DWORD fdwFilter);
void DestroyWatchList();
void NotifyPause(DRIVE drive, UINT uType);
void NotifyResume(DRIVE drive, UINT uType);
void vWaitMessage();
