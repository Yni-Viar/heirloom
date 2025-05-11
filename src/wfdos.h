#pragma once

#include <windows.h>

void GetDiskSpace(DRIVE drive, PULARGE_INTEGER pqFreeSpace, PULARGE_INTEGER pqTotalSpace);
DWORD GetVolumeLabel(DRIVE, LPWSTR*, BOOL);
DWORD
FillVolumeInfo(
    DRIVE drive,
    LPWSTR lpszVolName,
    PDWORD pdwVolumeSerialNumber,
    PDWORD pdwMaximumComponentLength,
    PDWORD pdwFileSystemFlags,
    LPWSTR lpszFileSysName);

DWORD WF_CreateDirectory(HWND, LPWSTR, LPWSTR);
