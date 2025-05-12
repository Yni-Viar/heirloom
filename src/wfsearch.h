#pragma once

#include <windows.h>

typedef struct _SEARCH_INFO {
    HWND hSearchDlg;
    int iDirsRead;
    int iFileCount;
    DWORD dwError;
    HANDLE hThread;
    HWND hwndLB;
    BOOL bUpdateStatus;
    BOOL bCancel;
    BOOL bDontSearchSubs;
    BOOL bIncludeSubDirs;
    BOOL bCasePreserved;
    int iRet;
    LPXDTALINK lpStart;
    enum _SEARCH_STATUS { SEARCH_NULL = 0, SEARCH_CANCEL, SEARCH_ERROR, SEARCH_MDICLOSE } eStatus;
    WCHAR szSearch[MAXPATHLEN + 1];
    FILETIME ftSince;  // UTC
} SEARCH_INFO, *PSEARCH_INFO;

void GetSearchPath(HWND hwnd, LPWSTR szTemp);
INT_PTR CALLBACK SearchProgDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateSearchStatus(HWND hwndLB, int nCount);
void SearchEnd();

LRESULT CALLBACK SearchWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

extern SEARCH_INFO SearchInfo;
