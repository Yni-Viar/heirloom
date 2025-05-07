/********************************************************************

   wffile.c

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#include "winfile.h"
#include "lfn.h"
#include "wfcopy.h"
#include "treectl.h"

/////////////////////////////////////////////////////////////////////////////
//
//  MKDir
//
//  Creates the given directory.
//
/////////////////////////////////////////////////////////////////////////////

DWORD MKDir(LPTSTR pName, LPTSTR pSrc) {
    DWORD dwErr = ERROR_SUCCESS;

    if ((pSrc && *pSrc) ? CreateDirectoryEx(pSrc, pName, NULL) : CreateDirectory(pName, NULL)) {
        ChangeFileSystem(FSC_MKDIR, pName, NULL);
    } else {
        dwErr = GetLastError();

        // CreateDirectoryEx does not support developer mode, so create symbolic ourselves
        if (ERROR_PRIVILEGE_NOT_HELD == dwErr) {
            dwErr = WFCopyIfSymlink(pSrc, pName, SYMBOLIC_LINK_FLAG_DIRECTORY, FSC_SYMLINKD);
        }
    }

    return dwErr;
}

/////////////////////////////////////////////////////////////////////////////
//
//  RMDir
//
//  Removes the given directory.
//
/////////////////////////////////////////////////////////////////////////////

DWORD RMDir(LPTSTR pName) {
    DWORD dwErr = 0;

    if (RemoveDirectory(pName)) {
        ChangeFileSystem(FSC_RMDIR, pName, NULL);
    } else {
        dwErr = (WORD)GetLastError();
    }

    return (dwErr);
}

/////////////////////////////////////////////////////////////////////////////
//
//  WFSetAttr
//
//  Sets the file attributes.
//
/////////////////////////////////////////////////////////////////////////////

BOOL WFSetAttr(LPTSTR lpFile, DWORD dwAttr) {
    BOOL bRet;

    //
    //  Compression attribute is handled separately -
    //  do not try to set it here.
    //
    dwAttr = dwAttr & ~(ATTR_COMPRESSED | ATTR_ENCRYPTED);

    bRet = SetFileAttributes(lpFile, dwAttr);

    if (bRet) {
        ChangeFileSystem(FSC_ATTRIBUTES, lpFile, NULL);
    }

    return ((BOOL)!bRet);
}

//////////////////////////////////////////////////////////////////////////////
//
//  CentreWindow
//
//  Positions a window so that it is centered in its parent.
//
//////////////////////////////////////////////////////////////////////////////

VOID CentreWindow(HWND hwnd) {
    RECT rect;
    RECT rectParent;
    HWND hwndParent;
    LONG dx, dy;
    LONG dxParent, dyParent;
    DWORD Style;

    //
    //  Get window rect.
    //
    GetWindowRect(hwnd, &rect);

    dx = rect.right - rect.left;
    dy = rect.bottom - rect.top;

    //
    //  Get parent rect.
    //
    Style = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
    if ((Style & WS_CHILD) == 0) {
        hwndParent = GetDesktopWindow();
    } else {
        hwndParent = GetParent(hwnd);
        if (hwndParent == NULL) {
            hwndParent = GetDesktopWindow();
        }
    }
    GetWindowRect(hwndParent, &rectParent);

    dxParent = rectParent.right - rectParent.left;
    dyParent = rectParent.bottom - rectParent.top;

    //
    //  Centre the child in the parent.
    //
    rect.left = (dxParent - dx) / 2;
    rect.top = (dyParent - dy) / 3;

    //
    //  Move the child into position.
    //
    SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    SetForegroundWindow(hwnd);
}

/////////////////////////////////////////////////////////////////////////////
//
//  GetRootPath
//
//  Gets the root directory path name.  Take into account "Quoted" paths
//  because the first char is not the drive letter.
//
/////////////////////////////////////////////////////////////////////////////

BOOL GetRootPath(LPTSTR szPath, LPTSTR szReturn) {
    if (!QualifyPath(szPath)) {
        return (FALSE);
    } else {
        szReturn[0] = TEXT('\0');
    }

    //
    //  Return the correct drive letter by taking into account
    //  "quoted" pathname strings.
    //

    szReturn[0] = (szPath[0] == CHAR_DQUOTE) ? szPath[1] : szPath[0];
    szReturn[1] = TEXT(':');
    szReturn[2] = TEXT('\\');
    szReturn[3] = TEXT('\0');

    return (TRUE);
}

// this function is not used right now, but I think I'll need it
// in the future for network connections.

BOOL CD(LPTSTR pszDir) {
    return SetCurrentDirectory(pszDir);
}

// this is used for saving/setting the current directory when using
// applications that change the current directory.
