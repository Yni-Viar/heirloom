/********************************************************************

   wfgwl.h

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

typedef struct {
    HWND hwnd;
    HWND hwndLastFocus;
    WORD wDriveNum;
    WORD wView;
    BOOL fFSCFlag;
    WORD wSort;
    DWORD dwAttribs;
    int iSplit;
    LPTREECTLDATA lptcd;
    LPDIRDDATA lpdird;
    LPDRIVESDATA lpdrvd;
} TREEDATA, FAR* LPTREEDATA;

typedef struct {
    HWND hwnd;
    int iReadLevel;
    HWND hwndLB;
    LPTREEDATA lptreed;
} TREECTLDATA, FAR* LPTREECTLDATA;

typedef struct {
    HWND hwnd;
    int iFirstTab;
    HANDLE hDTA;
    HWND hwndLB;
    LPTREEDATA lptreed;
} DIRDATA, FAR* LPDIRDATA;

typedef struct {
    HWND hwnd;
    LPWSTR lpstrVolume;
    int iCurDriveInd;
    int iCurDriveFocus;
    LPTREEDATA lptreed;
} DRIVESDATA, FAR* LPDRIVESDATA;

typedef struct {
    HWND hwnd;
    WORD wDriveNum;
    WORD wView;
    BOOL fFSCFlag;
    WORD wSort;
    DWORD dwAttribs;
    HANDLE hDTASearch;
    HWND hwndLB;
    LPTREEDATA lptreed;
} SEARCHDATA, FAR* LPSEARCHDATA;
