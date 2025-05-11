/********************************************************************

   wfext.h

   Windows File Manager Extensions definitions (Win32 variant)

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#pragma once

extern "C" {

#define MENU_TEXT_LEN 40

#define FMMENU_FIRST 1
#define FMMENU_LAST 99

#define FMEVENT_LOAD 100
#define FMEVENT_UNLOAD 101
#define FMEVENT_INITMENU 102
#define FMEVENT_USER_REFRESH 103
#define FMEVENT_SELCHANGE 104
#define FMEVENT_HELPSTRING 106
#define FMEVENT_HELPMENUITEM 107

#define FMFOCUS_DIR 1
#define FMFOCUS_TREE 2
#define FMFOCUS_DRIVES 3
#define FMFOCUS_SEARCH 4

#define FM_GETFOCUS (WM_USER + 0x0200)
#define FM_GETSELCOUNT (WM_USER + 0x0202)
#define FM_GETSELCOUNTLFN (WM_USER + 0x0203) /* LFN versions are odd */
#define FM_REFRESH_WINDOWS (WM_USER + 0x0206)
#define FM_RELOAD_EXTENSIONS (WM_USER + 0x0207)

#define FM_GETDRIVEINFOA (WM_USER + 0x0201)
#define FM_GETFILESELA (WM_USER + 0x0204)
#define FM_GETFILESELLFNA (WM_USER + 0x0205) /* LFN versions are odd */

#define FM_GETDRIVEINFOW (WM_USER + 0x0211)
#define FM_GETFILESELW (WM_USER + 0x0214)
#define FM_GETFILESELLFNW (WM_USER + 0x0215) /* LFN versions are odd */

#define FM_GETDRIVEINFO FM_GETDRIVEINFOW
#define FM_GETFILESEL FM_GETFILESELW
#define FM_GETFILESELLFN FM_GETFILESELLFNW

typedef struct _FMS_GETFILESELA {
    FILETIME ftTime;
    DWORD dwSize;
    BYTE bAttr;
    CHAR szName[260];  // always fully qualified
} FMS_GETFILESELA, FAR* LPFMS_GETFILESELA;

typedef struct _FMS_GETFILESELW {
    FILETIME ftTime;
    DWORD dwSize;
    BYTE bAttr;
    WCHAR szName[260];  // always fully qualified
} FMS_GETFILESELW, FAR* LPFMS_GETFILESELW;

#define FMS_GETFILESEL FMS_GETFILESELW
#define LPFMS_GETFILESEL LPFMS_GETFILESELW

typedef struct _FMS_GETDRIVEINFOA {  // for drive
    DWORD dwTotalSpace;
    DWORD dwFreeSpace;
    CHAR szPath[260];   // current directory
    CHAR szVolume[14];  // volume label
    CHAR szShare[128];  // if this is a net drive
} FMS_GETDRIVEINFOA, FAR* LPFMS_GETDRIVEINFOA;

typedef struct _FMS_GETDRIVEINFOW {  // for drive
    DWORD dwTotalSpace;
    DWORD dwFreeSpace;
    WCHAR szPath[260];   // current directory
    WCHAR szVolume[14];  // volume label
    WCHAR szShare[128];  // if this is a net drive
} FMS_GETDRIVEINFOW, FAR* LPFMS_GETDRIVEINFOW;

#define FMS_GETDRIVEINFO FMS_GETDRIVEINFOW
#define LPFMS_GETDRIVEINFO LPFMS_GETDRIVEINFOW

typedef struct _FMS_LOADA {
    DWORD dwSize;                    // for version checks
    CHAR szMenuName[MENU_TEXT_LEN];  // output
    HMENU hMenu;                     // output
    UINT wMenuDelta;                 // input
} FMS_LOADA, FAR* LPFMS_LOADA;

typedef struct _FMS_LOADW {
    DWORD dwSize;                     // for version checks
    WCHAR szMenuName[MENU_TEXT_LEN];  // output
    HMENU hMenu;                      // output
    UINT wMenuDelta;                  // input
} FMS_LOADW, FAR* LPFMS_LOADW;

#define FMS_LOAD FMS_LOADW
#define LPFMS_LOAD LPFMS_LOADW

typedef struct tagFMS_HELPSTRINGA {
    int idCommand;    /* input, -1==>the menu was selected */
    HMENU hMenu;      /* input, the extensions menu */
    CHAR szHelp[128]; /* output, the help string */
} FMS_HELPSTRINGA, FAR* LPFMS_HELPSTRINGA;

typedef struct tagFMS_HELPSTRINGW {
    int idCommand;     /* input, -1==>the menu was selected */
    HMENU hMenu;       /* input, the extensions menu */
    WCHAR szHelp[128]; /* output, the help string */
} FMS_HELPSTRINGW, FAR* LPFMS_HELPSTRINGW;

#define FMS_HELPSTRING FMS_HELPSTRINGW
#define LPFMS_HELPSTRING LPFMS_HELPSTRINGW

typedef DWORD(APIENTRY* FM_EXT_PROC)(HWND, WPARAM, LPARAM);

LONG WINAPI FMExtensionProcW(HWND hwnd, WPARAM wEvent, LPARAM lParam);

} /* End of extern "C" { */
