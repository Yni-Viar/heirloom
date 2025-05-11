/********************************************************************

   wfcomman.c

   Windows File System Command Proc

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#include "winfile.h"
#include "lfn.h"
#include "wfcopy.h"
#include "wnetcaps.h"  // WNetGetCaps()
#include "wfdrop.h"

#include <shlobj.h>
#include <commctrl.h>
#include <ole2.h>

#ifndef HELP_PARTIALKEY
#define HELP_PARTIALKEY 0x0105L  // call the search engine in winhelp
#endif

#define VIEW_NOCHANGE 0x0020  // previously VIEW_PLUSES

VOID MDIClientSizeChange(HWND hwndActive, INT iFlags);
HWND LocateDirWindow(LPTSTR pszPath, BOOL bNoFileSpec, BOOL bNoTreeWindow);
VOID UpdateAllDirWindows(LPTSTR pszPath, DWORD dwFunction, BOOL bNoFileSpec);
VOID AddNetMenuItems(VOID);
VOID InitNetMenuItems(VOID);

INT UpdateConnectionsOnConnect(VOID);

VOID NotifySearchFSC(LPWSTR pszPath, DWORD dwFunction) {
    if (!hwndSearch)
        return;

    if (DRIVEID(pszPath) == SendMessage(hwndSearch, FS_GETDRIVE, 0, 0L) - CHAR_A) {
        SendMessage(hwndSearch, WM_FSC, dwFunction, 0L);
    }
}

VOID RepaintDriveWindow(HWND hwndChild) {
    MDIClientSizeChange(hwndChild, DRIVEBAR_FLAG);
}

// RedoDriveWindows
//
// Note: Assumes UpdateDriveList and InitDriveBitmaps already called!

VOID RedoDriveWindows(HWND hwndActive) {
    INT iCurDrive;

    if (hwndActive == NULL)
        hwndActive = (HWND)SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L);

    iCurDrive = (INT)GetWindowLongPtr(hwndActive, GWL_TYPE);

    MDIClientSizeChange(hwndActive, DRIVEBAR_FLAG);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  LocateDirWindow() - return MDI client window which has a tree window    */
/*  and for which the path matches                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

HWND LocateDirWindow(LPTSTR pszPath, BOOL bNoFileSpec, BOOL bNoTreeWindow) {
    HWND hwndT;
    HWND hwndDir;
    LPTSTR pT2;
    TCHAR szTemp[MAXPATHLEN];
    TCHAR szPath[MAXPATHLEN];

    pT2 = pszPath;

    //
    //  Only work with well-formed paths.
    //
    if ((lstrlen(pT2) < 3) || (pT2[1] != CHAR_COLON)) {
        return (NULL);
    }

    //
    //  Copy path to temp buffer and remove the filespec (if necessary).
    //
    lstrcpy(szPath, pT2);

    if (!bNoFileSpec) {
        StripFilespec(szPath);
    }

    //
    //  Cycle through all of the windows until a match is found.
    //
    for (hwndT = GetWindow(hwndMDIClient, GW_CHILD); hwndT; hwndT = GetWindow(hwndT, GW_HWNDNEXT)) {
        if (hwndDir = HasDirWindow(hwndT)) {
            //
            //  Get the Window's path information and remove the file spec.
            //
            GetMDIWindowText(hwndT, szTemp, COUNTOF(szTemp));
            StripFilespec(szTemp);

            //
            //  Compare the two paths.
            //
            if (!lstrcmpi(szTemp, szPath) && (!bNoTreeWindow || !HasTreeWindow(hwndT))) {
                break;
            }
        }
    }

    return (hwndT);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  UpdateAllDirWindows() -                                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/

VOID UpdateAllDirWindows(LPTSTR pszPath, DWORD dwFunction, BOOL bNoFileSpec) {
    HWND hwndT;
    HWND hwndDir;
    LPTSTR pT2;
    TCHAR szTemp[MAXPATHLEN];
    TCHAR szPath[MAXPATHLEN];

    pT2 = pszPath;

    //
    //  Only work with well-formed paths.
    //
    if ((lstrlen(pT2) < 3) || (pT2[1] != CHAR_COLON)) {
        return;
    }

    //
    //  Copy path to temp buffer and remove the filespec (if necessary).
    //
    lstrcpy(szPath, pT2);

    if (!bNoFileSpec) {
        StripFilespec(szPath);
    }

    //
    //  Cycle through all of the windows until a match is found.
    //
    for (hwndT = GetWindow(hwndMDIClient, GW_CHILD); hwndT; hwndT = GetWindow(hwndT, GW_HWNDNEXT)) {
        if (hwndDir = HasDirWindow(hwndT)) {
            //
            //  Get the Window's path information and remove the file spec.
            //
            GetMDIWindowText(hwndT, szTemp, COUNTOF(szTemp));
            StripFilespec(szTemp);

            //
            //  Compare the two paths.  If they are equal, send notification
            //  to update the window.
            //
            if (!lstrcmpi(szTemp, szPath)) {
                SendMessage(hwndT, WM_FSC, dwFunction, (LPARAM)pszPath);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  ChangeFileSystem() -                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/* There are two sources of FileSysChange messages.  They can be sent from
 * the 386 WinOldAp or they can be posted by WINFILE's callback function (i.e.
 * from Kernel).  In both cases, we are free to do processing at this point.
 * For posted messages, we have to free the buffer passed to us as well.
 *
 * we are really in the other tasks context when we get entered here.
 * (this routine should be in a DLL)
 *
 * the file names are partially qualified, they have at least a drive
 * letter and initial directory part (c:\foo\..\bar.txt) so our
 * QualifyPath() calls should work.
 */

VOID ChangeFileSystem(DWORD dwFunction, LPCTSTR lpszFile, LPCTSTR lpszTo) {
    HWND hwnd, hwndTree, hwndOld;
    TCHAR szFrom[MAXPATHLEN];
    TCHAR szTo[MAXPATHLEN];
    TCHAR szTemp[MAXPATHLEN];
    TCHAR szPath[MAXPATHLEN + MAXPATHLEN];
    DWORD dwFSCOperation;

    // As FSC messages come in from outside winfile
    // we set a timer, and when that expires we
    // refresh everything.  If another FSC comes in while
    // we are waiting on this timer, we reset it so we
    // only refresh on the last operations.  This lets
    // the timer be much shorter.

    if (cDisableFSC == 0 || bFSCTimerSet) {
        if (bFSCTimerSet)
            KillTimer(hwndFrame, 1);  // reset the timer

        //
        // !! LATER !!
        // Move this info to the registry!
        //

        if (SetTimer(hwndFrame, 1, 1000, NULL)) {
            bFSCTimerSet = TRUE;
            if (cDisableFSC == 0)  // only disable once
                SendMessage(hwndFrame, FS_DISABLEFSC, 0, 0L);
        }
    }

    lstrcpy(szFrom, lpszFile);
    QualifyPath(szFrom);  // already partly qualified

    dwFSCOperation = FSC_Operation(dwFunction);

    switch (dwFSCOperation) {
        case (FSC_RENAME): {
            DWORD dwAttribs;
            DWORD dwFSCOperation;

            lstrcpy(szTo, lpszTo);
            QualifyPath(szTo);  // already partly qualified

            NotifySearchFSC(szFrom, dwFunction);

            // Update the original directory window (if any).
            if (hwndOld = LocateDirWindow(szFrom, FALSE, FALSE))
                SendMessage(hwndOld, WM_FSC, dwFunction, (LPARAM)szFrom);

            NotifySearchFSC(szTo, dwFunction);

            // Update the new directory window (if any).
            if ((hwnd = LocateDirWindow(szTo, FALSE, FALSE)) && (hwnd != hwndOld))
                SendMessage(hwnd, WM_FSC, dwFunction, (LPARAM)szTo);

            // Are we renaming a directory?
            lstrcpy(szTemp, szTo);

            dwAttribs = GetFileAttributes(szTemp);

            if (dwAttribs & ATTR_DIR) {
                dwFSCOperation = FSC_MKDIR;

                // Check if the directory is a junction or symbolic link.  These
                // have unique operation codes to keep optional junction display
                // straightforward.
                if (dwAttribs & ATTR_REPARSE_POINT) {
                    DWORD dwReparseTag;
                    dwReparseTag = DecodeReparsePoint(szTemp, NULL, 0);
                    if (dwReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
                        dwFSCOperation = FSC_JUNCTION;
                    } else if (dwReparseTag == IO_REPARSE_TAG_SYMLINK) {
                        dwFSCOperation = FSC_SYMLINKD;
                    }
                }
                for (hwnd = GetWindow(hwndMDIClient, GW_CHILD); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
                    if (hwndTree = HasTreeWindow(hwnd)) {
                        SendMessage(hwndTree, WM_FSC, FSC_RMDIR | FSC_QUIET, (LPARAM)szFrom);

                        // if the current selection is szFrom, we update the
                        // selection after the rename occurs

                        SendMessage(hwnd, FS_GETDIRECTORY, COUNTOF(szPath), (LPARAM)szPath);
                        StripBackslash(szPath);

                        SendMessage(hwndTree, WM_FSC, dwFSCOperation | FSC_QUIET, (LPARAM)szTo);

                        // update the selection if necessary, also
                        // change the window text in this case to
                        // reflect the new name

                        if (!lstrcmpi(szPath, szFrom)) {
                            SendMessage(hwndTree, TC_SETDIRECTORY, FALSE, (LPARAM)szTo);
                        }
                    }
                }
#ifdef NETCHECK
                InvalidateAllNetTypes();
#endif
            }
            break;
        }

        case (FSC_RMDIR): {
            // Close any open directory window.
            if (hwnd = LocateDirWindow(szFrom, TRUE, TRUE)) {
                SendMessage(hwnd, WM_CLOSE, 0, 0L);
            }

            /*** FALL THROUGH ***/
        }

        case (FSC_MKDIR):
        case (FSC_JUNCTION):
        case (FSC_SYMLINKD): {
            /* Update the tree. */
            for (hwnd = GetWindow(hwndMDIClient, GW_CHILD); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
                if (hwndTree = HasTreeWindow(hwnd)) {
                    SendMessage(hwndTree, WM_FSC, dwFunction, (LPARAM)szFrom);
                }
            }

            /*** FALL THROUGH ***/
        }

        case (FSC_DELETE):
        case (FSC_CREATE):
        case (FSC_REFRESH):
        case (FSC_ATTRIBUTES): {
            if (CHAR_COLON == szFrom[1])
                R_Space(DRIVEID(szFrom));

            SPC_SET_HITDISK(qFreeSpace);  // cause this stuff to be refreshed

            UpdateAllDirWindows(szFrom, dwFunction, FALSE);

            NotifySearchFSC(szFrom, dwFunction);

            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////
//
// Name:     CreateTreeWindow
//
// Synopsis: Creates a tree window
//
//           szPath  fully qualified ANSI path name WITH filespec
//           nSplit split position of tree and dir windows, if this is
//                   less than the threshold a tree will not be created,
//                   if it is more then a dir will not be created.
//                   0 to create a dir only
//                   very large number for tree only
//                   < 0 to have the split put in the middle
//
// Return:   hwnd of the MDI child created
//
//
// Assumes:
//
// Effects:
//
//
// Notes:
//
/////////////////////////////////////////////////////////////////////

HWND CreateTreeWindow(LPWSTR szPath, INT x, INT y, INT dx, INT dy, INT dxSplit) {
    HWND hwnd;
    DWORD style = 0L;

    //
    // this saves people from creating more windows than they should
    // note, when the mdi window is maximized many people don't realize
    // how many windows they have opened.
    //
    if (iNumWindows > 26) {
        LoadString(hAppInstance, IDS_WINFILE, szTitle, COUNTOF(szTitle));
        LoadString(hAppInstance, IDS_TOOMANYWINDOWS, szMessage, COUNTOF(szMessage));
        MessageBox(hwndFrame, szMessage, szTitle, MB_OK | MB_ICONEXCLAMATION);
        return NULL;
    }

    hwnd = (HWND)SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L);
    if (hwnd && GetWindowLongPtr(hwnd, GWL_STYLE) & WS_MAXIMIZE)
        style |= WS_MAXIMIZE;

    hwnd = CreateMDIWindow(szTreeClass, szPath, style, x, y, dx, dy, hwndMDIClient, hAppInstance, dxSplit);

    //
    // Set all the view/sort/include parameters.  This is to make
    // sure these values get initialized in the case when there is
    // no directory window.
    //
    SetWindowLongPtr(hwnd, GWL_VIEW, dwNewView);
    SetWindowLongPtr(hwnd, GWL_SORT, dwNewSort);
    SetWindowLongPtr(hwnd, GWL_ATTRIBS, dwNewAttribs);

    return hwnd;
}

/////////////////////////////////////////////////////////////////////
//
// Name:     CreateDirWindow
//
// Synopsis:
//
//      szPath          fully qualified path with no filespec
//      bReplaceOpen    default replacement mode, shift always toggles this
//      hwndActive      active mdi child that we are working on
//
// Return:   hwnd of window created or of existing dir window that we
//           activated or replaced if replace on open was active
//
//
// Assumes:
//
// Effects:
//
//
// Notes:
//
/////////////////////////////////////////////////////////////////////

HWND CreateDirWindow(LPWSTR szPath, BOOL bReplaceOpen, HWND hwndActive) {
    HWND hwndT;
    INT dxSplit;
    BOOLEAN bDriveChanged;

    if (hwndActive == hwndSearch) {
        bReplaceOpen = FALSE;
        dxSplit = -1;
    } else {
        dxSplit = GetSplit(hwndActive);
    }

    //
    // Is a window with this path already open?
    //
    if (!bReplaceOpen && (hwndT = LocateDirWindow(szPath, TRUE, FALSE))) {
        SendMessage(hwndMDIClient, WM_MDIACTIVATE, GET_WM_MDIACTIVATE_MPS(0, 0, hwndT));
        if (IsIconic(hwndT))
            SendMessage(hwndT, WM_SYSCOMMAND, SC_RESTORE, 0L);
        return hwndT;
    }

    bDriveChanged = FALSE;

    //
    // Are we replacing the contents of the currently active child?
    //
    if (bReplaceOpen) {
        DRIVE drive;
        INT i;

        CharUpperBuff(szPath, 1);  // make sure

        drive = DRIVEID(szPath);
        for (i = 0; i < cDrives; i++) {
            if (drive == rgiDrive[i]) {
                // if not already selected, do so now
                if (i != SendMessage(hwndDriveList, CB_GETCURSEL, i, 0L)) {
                    bDriveChanged = TRUE;
                }
                break;
            }
        }

        if (hwndT = HasDirWindow(hwndActive)) {
            WCHAR szFileSpec[MAXPATHLEN];

            AddBackslash(szPath);  // default to all files
            SendMessage(hwndT, FS_GETFILESPEC, COUNTOF(szFileSpec), (LPARAM)szFileSpec);
            lstrcat(szPath, szFileSpec);
            SendMessage(hwndT, FS_CHANGEDISPLAY, CD_PATH, (LPARAM)szPath);
            StripFilespec(szPath);
        }

        //
        // update the tree if necessary
        //

        if (hwndT = HasTreeWindow(hwndActive)) {
            SendMessage(hwndT, TC_SETDRIVE, 0, (LPARAM)(szPath));
        }

        //
        // Update the status in case we are "reading"
        //
        UpdateStatus(hwndActive);
        if (bDriveChanged) {
            InvalidateRect(hwndDriveBar, NULL, TRUE);
            UpdateWindow(hwndDriveBar);
        }

        return hwndActive;
    }

    AddBackslash(szPath);  // default to all files
    lstrcat(szPath, szStarDotStar);

    //
    // create tree and/or dir based on current window split
    //
    hwndActive = CreateTreeWindow(szPath, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, dxSplit);

    // call TC_SETDRIVE like use of CreateTreeWindow in NewTree()
    if (hwndActive && (hwndT = HasTreeWindow(hwndActive)))
        SendMessage(hwndT, TC_SETDRIVE, MAKELONG(MAKEWORD(FALSE, 0), TRUE), 0L);

    return hwndActive;
}

VOID OpenOrEditSelection(HWND hwndActive, BOOL fEdit) {
    LPTSTR p;
    BOOL bDir;
    DWORD ret;
    HCURSOR hCursor;

    WCHAR szPath[MAXPATHLEN + 2];  // +2 for quotes if needed

    HWND hwndTree, hwndDir, hwndFocus;

    //
    // Is the active MDI child minimized? if so restore it!
    //
    if (IsIconic(hwndActive)) {
        SendMessage(hwndActive, WM_SYSCOMMAND, SC_RESTORE, 0L);
        return;
    }

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    //
    // set the current directory
    //
    SetWindowDirectory();

    //
    // get the relevant parameters
    //
    GetTreeWindows(hwndActive, &hwndTree, &hwndDir);
    if (hwndTree || hwndDir)
        hwndFocus = GetTreeFocus(hwndActive);
    else
        hwndFocus = NULL;

    if (hwndDriveBar && hwndFocus == hwndDriveBar) {
        //
        // open a drive by sending a <CR>
        //
        SendMessage(hwndDriveBar, WM_KEYDOWN, VK_RETURN, 0L);

        goto OpenExit;
    }

    //
    // Get the first selected item.
    //
    p = (LPWSTR)SendMessage(hwndActive, FS_GETSELECTION, 1, (LPARAM)&bDir);

    if (!p)
        goto OpenExit;

    // less 2 characters in case we need to add quotes below
    if (!GetNextFile(p, szPath, COUNTOF(szPath) - 2) || !szPath[0])
        goto OpenFreeExit;

    if (bDir) {
        if (hwndDir && hwndFocus == hwndDir) {
            if (hwndTree) {
                SendMessage(hwndTree, TC_EXPANDLEVEL, FALSE, 0L);

                //
                // undo some things that happen in TC_EXPANDLEVEL
                //
                SetFocus(hwndDir);
            }

            //
            // shift toggles 'replace on open'
            //
            CreateDirWindow(szPath, GetKeyState(VK_SHIFT) >= 0, hwndActive);

        } else if (hwndTree) {
            // this came through because of
            // SHIFT open a dir only tree

            if (GetKeyState(VK_SHIFT) < 0) {
                CreateDirWindow(szPath, FALSE, hwndActive);
            } else {
                SendMessage(hwndTree, TC_TOGGLELEVEL, FALSE, 0L);
            }
        }

    } else {
        QualifyPath(szPath);

        //
        // Attempt to spawn the selected file.
        //
        if (fEdit) {
            TCHAR szEditPath[MAXPATHLEN];
            TCHAR szNotepad[MAXPATHLEN];

            // NOTE: assume system directory and "\\notepad.exe" never exceed MAXPATHLEN
            if (GetSystemDirectory(szNotepad, MAXPATHLEN) != 0)
                lstrcat(szNotepad, TEXT("\\notepad.exe"));
            else
                lstrcpy(szNotepad, TEXT("notepad.exe"));

            GetPrivateProfileString(szSettings, szEditorPath, szNotepad, szEditPath, MAXPATHLEN, szTheINIFile);

            CheckEsc(szPath);  // add quotes if necessary; reserved space for them above

            if (wcslen(szEditPath))
                ret = ExecProgram(szEditPath, szPath, NULL, (GetKeyState(VK_SHIFT) < 0), FALSE);
            // If INI entry is empty
            else
                ret = ExecProgram(szNotepad, szPath, NULL, (GetKeyState(VK_SHIFT) < 0), FALSE);

        } else {
            ret = ExecProgram(szPath, szNULL, NULL, (GetKeyState(VK_SHIFT) < 0), (GetKeyState(VK_CONTROL) < 0));
        }
        if (ret)
            MyMessageBox(hwndFrame, IDS_EXECERRTITLE, ret, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        else if (bMinOnRun)
            PostMessage(hwndFrame, WM_SYSCOMMAND, SC_MINIMIZE, 0L);
    }

OpenFreeExit:

    LocalFree((HLOCAL)p);

OpenExit:
    ShowCursor(FALSE);
    SetCursor(hCursor);
}

// Function to update the display when the size of the MDI client
// window changes (for example, when the status bar is turned on
// or off).

VOID MDIClientSizeChange(HWND hwndActive, INT iFlags) {
    RECT rc;

    GetClientRect(hwndFrame, &rc);
    SendMessage(hwndFrame, WM_SIZE, SIZENORMAL, MAKELONG(rc.right, rc.bottom));
    UpdateStatus(hwndActive);

    InvalidateRect(hwndMDIClient, NULL, FALSE);

    if (bDriveBar && (iFlags & DRIVEBAR_FLAG))
        InvalidateRect(hwndDriveBar, NULL, TRUE);

    UpdateWindow(hwndFrame);
}

BOOL FmifsLoaded() {
    return TRUE;
}

BOOL GetPowershellExePath(LPTSTR szPSPath) {
    HKEY hkey;
    if (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\PowerShell"), &hkey)) {
        return FALSE;
    }

    szPSPath[0] = TEXT('\0');

    for (int ikey = 0; ikey < 5; ikey++) {
        TCHAR szSub[10];  // just the "1" or "3"

        DWORD dwError = RegEnumKey(hkey, ikey, szSub, COUNTOF(szSub));

        if (dwError == ERROR_SUCCESS) {
            // if installed, get powershell exe
            DWORD dwInstall;
            DWORD dwType;
            DWORD cbValue = sizeof(dwInstall);
            dwError = RegGetValue(hkey, szSub, TEXT("Install"), RRF_RT_DWORD, &dwType, (PVOID)&dwInstall, &cbValue);

            if (dwError == ERROR_SUCCESS && dwInstall == 1) {
                // this install of powershell is active; get path

                HKEY hkeySub;
                dwError = RegOpenKey(hkey, szSub, &hkeySub);

                if (dwError == ERROR_SUCCESS) {
                    LPTSTR szPSExe = TEXT("\\Powershell.exe");

                    cbValue = (MAXPATHLEN - lstrlen(szPSExe)) * sizeof(TCHAR);
                    dwError = RegGetValue(
                        hkeySub, TEXT("PowerShellEngine"), TEXT("ApplicationBase"),
                        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, &dwType, (PVOID)szPSPath, &cbValue);

                    if (dwError == ERROR_SUCCESS) {
                        lstrcat(szPSPath, szPSExe);
                    } else {
                        // reset to empty string if not successful
                        szPSPath[0] = TEXT('\0');
                    }

                    RegCloseKey(hkeySub);
                }
            }
        }
    }

    RegCloseKey(hkey);

    // return true if we got a valid path
    return szPSPath[0] != TEXT('\0');
}

BOOL GetBashExePath(LPTSTR szBashPath, UINT bufSize) {
    const TCHAR szBashFilename[] = TEXT("bash.exe");
    UINT len;

    len = GetSystemDirectory(szBashPath, bufSize);
    if ((len != 0) && (len + COUNTOF(szBashFilename) + 1 < bufSize) && PathAppend(szBashPath, TEXT("bash.exe"))) {
        if (PathFileExists(szBashPath))
            return TRUE;
    }

    // If we are running 32 bit Winfile on 64 bit Windows, System32 folder is redirected to SysWow64, which
    // doesn't include bash.exe. So we also need to check Sysnative folder, which always maps to System32 folder.
    len = ExpandEnvironmentStrings(TEXT("%SystemRoot%\\Sysnative\\bash.exe"), szBashPath, bufSize);
    if (len != 0 && len <= bufSize) {
        return PathFileExists(szBashPath);
    }

    return FALSE;
}
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  AppCommandProc() -                                                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/

BOOL AppCommandProc(DWORD id) {
    DWORD dwFlags;
    HMENU hMenu;
    HWND hwndActive;
    BOOL bTemp;
    HWND hwndT;
    TCHAR szPath[MAXPATHLEN];
    INT ret;

    hwndActive = (HWND)SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L);

    switch (id) {
        case IDM_SPLIT:
            SendMessage(hwndActive, WM_SYSCOMMAND, SC_SPLIT, 0L);
            break;

        case IDM_TREEONLY:
        case IDM_DIRONLY:
        case IDM_BOTH: {
            RECT rc;
            INT x;

            if (hwndActive != hwndSearch) {
                GetClientRect(hwndActive, &rc);

                if (id == IDM_DIRONLY) {
                    x = 0;
                } else if (id == IDM_TREEONLY) {
                    x = rc.right;
                } else {
                    x = rc.right / 2;
                }

                if (ResizeSplit(hwndActive, x))
                    SendMessage(hwndActive, WM_SIZE, SIZENOMDICRAP, MAKELONG(rc.right, rc.bottom));
            }
            break;
        }

        case IDM_ESCAPE:
            bCancelTree = TRUE;
            break;

        case IDM_EMPTYRECYCLE:
            // Prompt for confirmation if enabled in settings
            if (bConfirmDelete) {
                TCHAR szTitle[64];
                TCHAR szMessage[128];

                LoadString(hAppInstance, 431, szTitle, COUNTOF(szTitle));
                LoadString(hAppInstance, 432, szMessage, COUNTOF(szMessage));

                if (MessageBox(hwndFrame, szMessage, szTitle, MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
                    break;
            }

            // Empty the Recycle Bin
            EmptyRecycleBin(hwndFrame);

            // Update the status bar
            if (hwndActive && HasDirWindow(hwndActive)) {
                UpdateStatus(hwndActive);
            }
            break;

        case IDM_OPEN:

            if (GetFocus() == hwndDriveList)
                break; /* user hit Enter in drive list */

            if (GetKeyState(VK_MENU) < 0)
                PostMessage(hwndFrame, WM_COMMAND, GET_WM_COMMAND_MPS(IDM_ATTRIBS, 0, 0));
            else {
                TypeAheadString('\0', NULL);

                OpenOrEditSelection(hwndActive, FALSE);
            }
            break;

        case IDM_EDIT:
            TypeAheadString('\0', NULL);

            OpenOrEditSelection(hwndActive, TRUE);
            break;

        case IDM_GOTODIR:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(GOTODIRDLG), hwndFrame, GotoDirDlgProc);
            break;

        case IDM_HISTORYBACK:
        case IDM_HISTORYFWD: {
            HWND hwndT;
            if (GetPrevHistoryDir(id == IDM_HISTORYFWD, &hwndT, szPath)) {
                // history is saved with wildcard spec; remove it
                StripFilespec(szPath);
                SetCurrentPathOfWindow(szPath);
            }
        } break;

        case IDM_SEARCH:

            // bug out if one is running

            if (SearchInfo.hSearchDlg) {
                SetFocus(SearchInfo.hSearchDlg);
                return 0;
            }

            if (SearchInfo.hThread) {
                //
                // Don't create any new worker threads
                // Just create old dialog
                //

                CreateDialog(hAppInstance, (LPTSTR)MAKEINTRESOURCE(SEARCHPROGDLG), hwndFrame, SearchProgDlgProc);
                break;
            }

            dwSuperDlgMode = IDM_SEARCH;

            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(SEARCHDLG), hwndFrame, SearchDlgProc);
            break;

        case IDM_RUN:

            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(RUNDLG), hwndFrame, RunDlgProc);
            break;

        case IDM_STARTCMDSHELL: {
            BOOL bRunAs;
            BOOL bUseCmd;
            BOOL bDir;
            DWORD cchEnv;
            TCHAR szToRun[MAXPATHLEN];
            LPTSTR szDir;

#define ConEmuParamFormat TEXT(" -Single -Dir \"%s\"")
#define CmdParamFormat TEXT("/k cd /d ")
            TCHAR szParams[MAXPATHLEN + max(COUNTOF(CmdParamFormat), COUNTOF(ConEmuParamFormat))];

            szDir = GetSelection(1 | 4 | 16, &bDir);
            if (!bDir && szDir)
                StripFilespec(szDir);

            bRunAs = GetKeyState(VK_SHIFT) < 0;

            // check if conemu exists: %ProgramFiles%\ConEmu\ConEmu64.exe
            bUseCmd = TRUE;
            cchEnv = GetEnvironmentVariable(TEXT("ProgramFiles"), szToRun, MAXPATHLEN);
            if (cchEnv != 0) {
                if (lstrcmpi(szToRun + cchEnv - 6, TEXT(" (x86)")) == 0) {
                    szToRun[cchEnv - 6] = TEXT('\0');
                }
                // NOTE: assume ProgramFiles directory and "\\ConEmu\\ConEmu64.exe" never exceed MAXPATHLEN
                lstrcat(szToRun, TEXT("\\ConEmu\\ConEmu64.exe"));
                if (PathFileExists(szToRun)) {
                    wsprintf(szParams, ConEmuParamFormat, szDir);
                    bUseCmd = FALSE;
                }
            }

            // use cmd.exe if ConEmu doesn't exist or we are running admin mode
            if (bUseCmd) {
                // NOTE: assume system directory and "\\cmd.exe" never exceed MAXPATHLEN
                if (GetSystemDirectory(szToRun, MAXPATHLEN) != 0)
                    lstrcat(szToRun, TEXT("\\cmd.exe"));
                else
                    lstrcpy(szToRun, TEXT("cmd.exe"));

                if (bRunAs) {
                    // Execute a command prompt and cd into the directory
                    lstrcpy(szParams, CmdParamFormat);
                    lstrcat(szParams, szDir);
                } else {
                    szParams[0] = TEXT('\0');
                }
            }

            ret = ExecProgram(szToRun, szParams, szDir, FALSE, bRunAs);
            LocalFree(szDir);
        } break;

        case IDM_STARTEXPLORER: {
            BOOL bDir;
            LPTSTR szDir;
            TCHAR szToRun[MAXPATHLEN];

            szDir = GetSelection(1 | 4 | 16, &bDir);
            if (!bDir && szDir)
                StripFilespec(szDir);

            if (GetSystemDirectory(szToRun, MAXPATHLEN) != 0)
                lstrcat(szToRun, TEXT("\\..\\explorer.exe"));
            else
                lstrcpy(szToRun, TEXT("explorer.exe"));

            TCHAR szParams[MAXPATHLEN] = { TEXT('\0') };

            ret = ExecProgram(szToRun, szDir, szDir, FALSE, FALSE);
            LocalFree(szDir);
        } break;

        case IDM_STARTPOWERSHELL: {
            BOOL bRunAs;
            BOOL bDir;
            TCHAR szToRun[MAXPATHLEN];
            LPTSTR szDir;
#define PowerShellParamFormat TEXT(" -noexit -command \"cd \\\"%s\\\"\"")
            TCHAR szParams[MAXPATHLEN + COUNTOF(PowerShellParamFormat)];

            szDir = GetSelection(1 | 4 | 16, &bDir);
            if (!bDir && szDir)
                StripFilespec(szDir);

            bRunAs = GetKeyState(VK_SHIFT) < 0;

            if (GetPowershellExePath(szToRun)) {
                wsprintf(szParams, PowerShellParamFormat, szDir);

                ret = ExecProgram(szToRun, szParams, szDir, FALSE, bRunAs);
            }

            LocalFree(szDir);
        } break;

        case IDM_STARTBASHSHELL: {
            BOOL bRunAs;
            BOOL bDir;
            TCHAR szToRun[MAXPATHLEN];
            LPTSTR szDir;

            szDir = GetSelection(1 | 4 | 16, &bDir);
            if (!bDir && szDir)
                StripFilespec(szDir);

            bRunAs = GetKeyState(VK_SHIFT) < 0;

            if (GetBashExePath(szToRun, COUNTOF(szToRun))) {
                ret = ExecProgram(szToRun, NULL, szDir, FALSE, bRunAs);
            }

            LocalFree(szDir);
        } break;

        case IDM_CLOSEWINDOW: {
            HWND hwndActive;

            hwndActive = (HWND)SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L);
            PostMessage(hwndActive, WM_CLOSE, 0, 0L);
        } break;

        case IDM_SELECT:

            // push the focus to the dir half so when they are done
            // with the selection they can manipulate without undoing the
            // selection.

            if (hwndT = HasDirWindow(hwndActive))
                SetFocus(hwndT);

            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(SELECTDLG), hwndFrame, SelectDlgProc);
            break;

        case IDM_MOVE:
        case IDM_COPY:
        case IDM_RENAME:
        case IDM_SYMLINK:
        case IDM_HARDLINK:
            dwSuperDlgMode = id;

            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(MOVECOPYDLG), hwndFrame, SuperDlgProc);
            break;

        case IDM_PASTE: {
            IDataObject* pDataObj;
            FORMATETC fmtetcDrop = { 0, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            CLIPFORMAT uFormatEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
            FORMATETC fmtetcEffect = { uFormatEffect, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stgmed;
            DWORD dwEffect = DROPEFFECT_COPY;
            LPWSTR szFiles = NULL;

            OleGetClipboard(&pDataObj);  // pDataObj == NULL if error

            if (pDataObj != NULL && pDataObj->GetData(&fmtetcEffect, &stgmed) == S_OK) {
                LPDWORD lpEffect = (LPDWORD)GlobalLock(stgmed.hGlobal);
                if (*lpEffect & DROPEFFECT_COPY)
                    dwEffect = DROPEFFECT_COPY;
                if (*lpEffect & DROPEFFECT_MOVE)
                    dwEffect = DROPEFFECT_MOVE;
                GlobalUnlock(stgmed.hGlobal);
                ReleaseStgMedium(&stgmed);
            }

            // Try CF_HDROP
            if (pDataObj != NULL)
                szFiles = QuotedDropList(pDataObj);

            // Try CFSTR_FILEDESCRIPTOR
            if (szFiles == NULL) {
                szFiles = QuotedContentList(pDataObj);
                if (szFiles != NULL)
                    // need to move the already copied files
                    dwEffect = DROPEFFECT_MOVE;
            }

            // Try "LongFileNameW"
            fmtetcDrop.cfFormat = RegisterClipboardFormat(TEXT("LongFileNameW"));
            if (szFiles == NULL && pDataObj != NULL && pDataObj->GetData(&fmtetcDrop, &stgmed) == S_OK) {
                LPWSTR lpFile = (LPWSTR)GlobalLock(stgmed.hGlobal);
                SIZE_T cchFile = wcslen(lpFile);
                szFiles = (LPWSTR)LocalAlloc(LMEM_FIXED, (cchFile + 3) * sizeof(WCHAR));
                lstrcpy(szFiles + 1, lpFile);
                *szFiles = '\"';
                *(szFiles + 1 + cchFile) = '\"';
                *(szFiles + 1 + cchFile + 1) = '\0';

                GlobalUnlock(stgmed.hGlobal);

                // release the data using the COM API
                ReleaseStgMedium(&stgmed);
            }

            if (szFiles != NULL) {
                WCHAR szTemp[MAXPATHLEN];

                SendMessage(hwndActive, FS_GETDIRECTORY, COUNTOF(szTemp), (LPARAM)szTemp);

                AddBackslash(szTemp);
                lstrcat(szTemp, szStarDotStar);  // put files in this dir

                CheckEsc(szTemp);

                DMMoveCopyHelper(szFiles, szTemp, dwEffect == DROPEFFECT_COPY);

                LocalFree((HLOCAL)szFiles);
            }

            if (pDataObj != NULL)
                pDataObj->Release();
        } break;

        case IDM_DELETE:
            dwSuperDlgMode = id;

            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(DELETEDLG), hwndFrame, SuperDlgProc);
            break;

        case IDM_COPYTOCLIPBOARD:
        case IDM_CUTTOCLIPBOARD: {
            LPTSTR pszFiles;
            HANDLE hMemLongW, hMemTextW, hDrop;
            LONG cbMemLong;
            HANDLE hMemDropEffect;
            TCHAR szPathLong[MAXPATHLEN];
            POINT pt;
            INT iMultipleResult;

            pszFiles = GetSelection(4, NULL);
            if (pszFiles == NULL) {
                break;
            }

            // LongFileNameW (only if one file or directory)
            // This is allocated twice to record a text form also;
            // clipboard requires one handle per format despite them
            // having identical contents
            hMemLongW = NULL;
            hMemTextW = NULL;
            iMultipleResult = CheckMultiple(pszFiles);
            if (iMultipleResult == 2 || iMultipleResult == 0) {
                GetNextFile(pszFiles, szPathLong, COUNTOF(szPathLong));
                cbMemLong = ByteCountOf(lstrlen(szPathLong) + 1);
                hMemLongW = GlobalAlloc(GPTR | GMEM_DDESHARE, cbMemLong);
                if (hMemLongW) {
                    lstrcpy((LPWSTR)GlobalLock(hMemLongW), szPathLong);
                    GlobalUnlock(hMemLongW);
                }
                hMemTextW = GlobalAlloc(GPTR | GMEM_DDESHARE, cbMemLong);
                if (hMemTextW) {
                    lstrcpy((LPWSTR)GlobalLock(hMemTextW), szPathLong);
                    GlobalUnlock(hMemTextW);
                }
            }

            hMemDropEffect = NULL;
            if (id == IDM_CUTTOCLIPBOARD) {
                hMemDropEffect = GlobalAlloc(GPTR | GMEM_DDESHARE, sizeof(DWORD));
                if (hMemDropEffect) {
                    *(DWORD*)GlobalLock(hMemDropEffect) = DROPEFFECT_MOVE;
                    GlobalUnlock(hMemDropEffect);
                }
            }

            // CF_HDROP
            pt.x = 0;
            pt.y = 0;
            hDrop = CreateDropFiles(pt, FALSE, pszFiles);

            if (OpenClipboard(hwndFrame)) {
                EmptyClipboard();

                if (hMemDropEffect) {
                    SetClipboardData(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT), hMemDropEffect);
                }

                SetClipboardData(RegisterClipboardFormat(TEXT("LongFileNameW")), hMemLongW);
                SetClipboardData(CF_UNICODETEXT, hMemTextW);
                SetClipboardData(CF_HDROP, hDrop);

                CloseClipboard();
            } else {
                GlobalFree(hMemLongW);
                GlobalFree(hMemTextW);
                GlobalFree(hDrop);
            }

            LocalFree((HANDLE)pszFiles);

            UpdateMoveStatus(id == IDM_CUTTOCLIPBOARD ? DROPEFFECT_MOVE : DROPEFFECT_COPY);
        } break;

        case IDM_ATTRIBS: {
            LPTSTR pSel, p;
            INT count;
            TCHAR szTemp[MAXPATHLEN];

            BOOL bDir = FALSE;

            // should do the multiple or single file properties

            pSel = GetSelection(0, &bDir);

            if (!pSel)
                break;

            count = 0;
            p = pSel;

            while (p = GetNextFile(p, szTemp, COUNTOF(szTemp))) {
                count++;
            }

            LocalFree((HANDLE)pSel);

            if (count == 1) {
                SHELLEXECUTEINFO sei;

                memset(&sei, 0, sizeof(sei));
                sei.cbSize = sizeof(sei);
                sei.fMask = SEE_MASK_INVOKEIDLIST;
                sei.hwnd = hwndActive;
                sei.lpVerb = TEXT("properties");
                sei.lpFile = szTemp;

                if (!bDir) {
                    SendMessage(hwndActive, FS_GETDIRECTORY, COUNTOF(szPath), (LPARAM)szPath);
                    StripBackslash(szPath);

                    sei.lpDirectory = szPath;
                }

                ShellExecuteEx(&sei);
            } else if (count > 1)
                DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(MULTIPLEATTRIBSDLG), hwndFrame, AttribsDlgProc);
            break;
        }

        case IDM_MAKEDIR:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(MAKEDIRDLG), hwndFrame, MakeDirDlgProc);
            break;

        case IDM_SELALL:
        case IDM_DESELALL: {
            INT iSave;
            HWND hwndLB;
            HWND hwndDir;
            LPXDTALINK lpStart;
            LPXDTA lpxdta;

            hwndDir = HasDirWindow(hwndActive);

            if (!hwndDir)
                break;

            hwndLB = GetDlgItem(hwndDir, IDCW_LISTBOX);

            if (!hwndLB)
                break;

            SendMessage(hwndLB, WM_SETREDRAW, FALSE, 0L);

            iSave = (INT)SendMessage(hwndLB, LB_GETCURSEL, 0, 0L);
            SendMessage(hwndLB, LB_SETSEL, (id == IDM_SELALL), -1L);

            if (id == IDM_DESELALL) {
                SendMessage(hwndLB, LB_SETSEL, TRUE, (LONG)iSave);

            } else if (hwndActive != hwndSearch) {
                //
                // Is the first item the [..] directory?
                //
                lpStart = (LPXDTALINK)GetWindowLongPtr(hwndDir, GWL_HDTA);

                if (lpStart) {
                    lpxdta = MemLinkToHead(lpStart)->alpxdtaSorted[0];

                    if (lpxdta->dwAttrs & ATTR_PARENT)
                        SendMessage(hwndLB, LB_SETSEL, 0, 0L);
                }
            }
            SendMessage(hwndLB, WM_SETREDRAW, TRUE, 0L);
            InvalidateRect(hwndLB, NULL, FALSE);

            //
            // Emulate a SELCHANGE notification.
            //
            SendMessage(hwndDir, WM_COMMAND, GET_WM_COMMAND_MPS(0, hwndLB, LBN_SELCHANGE));

            break;
        }

        case IDM_SAVENOW:

            SaveWindows(hwndFrame);
            break;

        case IDM_EXIT:

            // SHIFT Exit saves settings, doesn't exit.

            if (GetKeyState(VK_SHIFT) < 0) {
                SaveWindows(hwndFrame);
                break;
            }

            if (iReadLevel) {
                // don't exit now.  setting this variable will force the
                // tree read to terminate and then post a close message
                // when it is safe
                bCancelTree = 2;
                break;
            }

            //
            // If disk formatting/copying, ask user if they really want to quit.
            //

            if (CancelInfo.hThread) {
                DWORD dwIDS;

                switch (CancelInfo.eCancelType) {
                    case _CANCEL_INFO::CANCEL_FORMAT:

                        dwIDS = IDS_BUSYFORMATQUITVERIFY;
                        break;

                    case _CANCEL_INFO::CANCEL_COPY:

                        dwIDS = IDS_BUSYCOPYQUITVERIFY;
                        break;

                    default:

                        //
                        // We should never get here
                        //
                        dwIDS = IDS_BUSYFORMATQUITVERIFY;
                        break;
                }

                if (MyMessageBox(hwndFrame, IDS_WINFILE, dwIDS, MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL) {
                    break;
                }
            }

            SetCurrentDirectory(szOriginalDirPath);

            if (bSaveSettings)
                SaveWindows(hwndFrame);

            return FALSE;
            break;

        case IDM_EXPONE:
            if (hwndT = HasTreeWindow(hwndActive))
                SendMessage(hwndT, TC_EXPANDLEVEL, FALSE, 0L);
            break;

        case IDM_COLLAPSE:
            if (hwndT = HasTreeWindow(hwndActive))
                SendMessage(hwndT, TC_COLLAPSELEVEL, 0, 0L);
            break;

        case IDM_VNAME:
            dwFlags = VIEW_NAMEONLY | (GetWindowLongPtr(hwndActive, GWL_VIEW) & VIEW_NOCHANGE);
            id = CD_VIEW;
            goto ChangeDisplay;

        case IDM_VDETAILS:
            dwFlags = VIEW_EVERYTHING | (GetWindowLongPtr(hwndActive, GWL_VIEW) & VIEW_NOCHANGE);
            id = CD_VIEW;
            goto ChangeDisplay;

        case IDM_VOTHER:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(OTHERDLG), hwndFrame, OtherDlgProc);

            dwFlags = GetWindowLongPtr(hwndActive, GWL_VIEW) & VIEW_EVERYTHING;
            break;

        case IDM_BYNAME:
        case IDM_BYTYPE:
        case IDM_BYSIZE:
        case IDM_BYDATE:
        case IDM_BYFDATE:
            dwFlags = (id - IDM_BYNAME) + IDD_NAME;
            id = CD_SORT;

        ChangeDisplay:

            if (hwndT = HasDirWindow(hwndActive)) {
                SendMessage(hwndT, FS_CHANGEDISPLAY, id, MAKELONG(LOWORD(dwFlags), 0));
            } else if (hwndActive == hwndSearch) {
                SetWindowLongPtr(hwndActive, GWL_VIEW, dwFlags);
                SendMessage(hwndSearch, FS_CHANGEDISPLAY, CD_VIEW, 0L);
                //        InvalidateRect(hwndActive, NULL, TRUE);
            }

            // Update menu checkmarks for View menu items
            if (id == CD_SORT) {
                HMENU hMenu = GetMenu(hwndFrame);
                CheckMenuItem(hMenu, IDM_BYNAME, (dwFlags == IDD_NAME) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_BYTYPE, (dwFlags == IDD_TYPE) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_BYSIZE, (dwFlags == IDD_SIZE) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_BYDATE, (dwFlags == IDD_DATE) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_BYFDATE, (dwFlags == IDD_FDATE) ? MF_CHECKED : MF_UNCHECKED);
                DrawMenuBar(hwndFrame);
            } else if (id == CD_VIEW) {
                HMENU hMenu = GetMenu(hwndFrame);
                DWORD viewFlags = dwFlags & VIEW_EVERYTHING;
                CheckMenuItem(hMenu, IDM_VNAME, (viewFlags == VIEW_NAMEONLY) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_VDETAILS, (viewFlags == VIEW_EVERYTHING) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(
                    hMenu, IDM_VOTHER,
                    (viewFlags != VIEW_NAMEONLY && viewFlags != VIEW_EVERYTHING) ? MF_CHECKED : MF_UNCHECKED);
                DrawMenuBar(hwndFrame);
            }

            break;

        case IDM_VINCLUDE:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(INCLUDEDLG), hwndFrame, IncludeDlgProc);
            break;

        case IDM_CONFIRM:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(CONFIRMDLG), hwndFrame, ConfirmDlgProc);
            break;

        case IDM_PREF:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(PREFDLG), hwndFrame, PrefDlgProc);
            break;

        case IDM_STATUSBAR:
            bTemp = bStatusBar = !bStatusBar;
            WritePrivateProfileBool(szStatusBar, bStatusBar);

            ShowWindow(hwndStatus, bStatusBar ? SW_SHOW : SW_HIDE);
            MDIClientSizeChange(hwndActive, DRIVEBAR_FLAG);

            goto CHECK_OPTION;
            break;

        case IDM_DRIVEBAR:
            bTemp = bDriveBar = !bDriveBar;
            WritePrivateProfileBool(szDriveBar, bDriveBar);

            ShowWindow(hwndDriveBar, bDriveBar ? SW_SHOW : SW_HIDE);
            MDIClientSizeChange(hwndActive, DRIVEBAR_FLAG);

            goto CHECK_OPTION;
            break;

        case IDM_DRIVELISTJUMP:
            // Drive list functionality removed
            break;

        case IDM_FONT:
            NewFont();
            break;

        case IDM_SAVESETTINGS:
            bTemp = bSaveSettings = !bSaveSettings;
            WritePrivateProfileBool(szSaveSettings, bSaveSettings);
            goto CHECK_OPTION;

        case IDM_MINONRUN:
            bTemp = bMinOnRun = !bMinOnRun;
            WritePrivateProfileBool(szMinOnRun, bMinOnRun);
            goto CHECK_OPTION;

        case IDM_INDEXONLAUNCH:
            bTemp = bIndexOnLaunch = !bIndexOnLaunch;
            WritePrivateProfileBool(szIndexOnLaunch, bIndexOnLaunch);
            goto CHECK_OPTION;

        CHECK_OPTION:
            //
            // Check/Uncheck the menu item.
            //
            hMenu = GetMenu(hwndFrame);
            CheckMenuItem(hMenu, id, (bTemp ? MF_CHECKED : MF_UNCHECKED));
            DrawMenuBar(hwndFrame);
            break;

        case IDM_NEWWINDOW:
            NewTree((INT)SendMessage(hwndActive, FS_GETDRIVE, 0, 0L) - CHAR_A, hwndActive);
            break;

        case IDM_CASCADE:
            SendMessage(hwndMDIClient, WM_MDICASCADE, 0L, 0L);
            break;

        case IDM_TILE:
            SendMessage(hwndMDIClient, WM_MDITILE, 0, 0L);
            break;

        case IDM_TILEHORIZONTALLY:
            SendMessage(hwndMDIClient, WM_MDITILE, 1, 0L);
            break;

        case IDM_REFRESH: {
            INT i;

#define NUMDRIVES (sizeof(rgiDrive) / sizeof(rgiDrive[0]))
            INT rgiSaveDrives[NUMDRIVES];

            if (WAITNET_LOADED) {
                //
                // Refresh net status
                //
                WNetStat(NS_REFRESH);

                AddNetMenuItems();
            }

            for (i = 0; i < NUMDRIVES; ++i)
                rgiSaveDrives[i] = rgiDrive[i];

            for (i = 0; i < iNumExtensions; i++) {
                (extensions[i].ExtProc)(hwndFrame, FMEVENT_USER_REFRESH, 0L);
            }

            RefreshWindow(hwndActive, TRUE, TRUE);

            // If any drive has changed, then we must resize all drive windows
            // (done by sending the WM_SIZE below) and invalidate them so that
            // they will reflect the current status

            for (i = 0; i < NUMDRIVES; ++i) {
                if (rgiDrive[i] != rgiSaveDrives[i]) {
                    // RedoDriveWindows no longer calls
                    // updatedrivelist or initdrivebitmaps;
                    // they are called in RefreshWindow above
                    // (nix, above!), so don't duplicate.
                    // (a speed hack now that updatedrivelist /
                    // initdrivebitmaps are s-l-o-w.

                    RedoDriveWindows(hwndActive);
                    break; /* Break out of the for loop */
                }
            }

            //
            // update free space
            // (We invalidated the aDriveInfo cache, so this is enough)
            //
            SPC_SET_INVALID(qFreeSpace);
            UpdateStatus(hwndActive);

            StartBuildingDirectoryTrie();

            break;
        }

        case IDM_ABOUT:
            DialogBox(hAppInstance, (LPTSTR)MAKEINTRESOURCE(ABOUTDLG), hwndFrame, AboutDlgProc);
            break;

        default: {
            INT i;

            for (i = 0; i < iNumExtensions; i++) {
                WORD delta = extensions[i].Delta;

                if ((id >= delta) && (id < (WORD)(delta + 100))) {
                    (extensions[i].ExtProc)(hwndFrame, (WORD)(id - delta), 0L);
                    break;
                }
            }
        }
            return FALSE;
    }

    return TRUE;
}

// SwitchToSafeDrive:
// Used when on a network drive and disconnecting: must switch to
// a "safe" drive so that we don't prevent disconnect.
// IN: VOID
// OUT: VOID
// Precond: System directory is on a safe hard disk
//          szMessage not being used
// Postcond: Switch to this directory.
//          szMessage trashed.

VOID SwitchToSafeDrive(VOID) {
    WCHAR szSafePath[MAXPATHLEN];

    GetSystemDirectory(szSafePath, COUNTOF(szSafePath));
    SetCurrentDirectory(szSafePath);
}

VOID AddNetMenuItems(VOID) {
    HMENU hMenu;

    hMenu = GetMenu(hwndFrame);
}

VOID InitNetMenuItems(VOID) {}

/////////////////////////////////////////////////////////////////////
//
// Name:     UpdateConnectionsOnConnect
//
// Synopsis: This handles updating connections after a new connection
//           is created.  It only invalidates aWNetGetCon[i] for the
//           new drive.  If there is no new drive (i.e., a replaced
//           connection), then all are invalidated.
//
//           If you want everything to update always, just modify
//           this routine to always set all bValids to FALSE.
//
//
// Return:   INT      >= 0 iDrive that is new
//                    = -1 if no detected new drive letter
//
// Assumes:
//
// Effects:
//
//
// Notes:    Updates drive list by calling worker thread.
//
// Rev:      216 1.22
//
/////////////////////////////////////////////////////////////////////

INT UpdateConnectionsOnConnect(VOID) {
    INT rgiOld[MAX_DRIVES];

    BOOL abRemembered[MAX_DRIVES];
    PDRIVEINFO pDriveInfo;
    DRIVE drive;

    DRIVEIND i;

    //
    // save old drive map so that we can compare it against the
    // current map for new drives.
    //
    for (i = 0; i < MAX_DRIVES; i++)
        rgiOld[i] = rgiDrive[i];

    //
    // save a list of what's remembered
    //
    for (pDriveInfo = &aDriveInfo[0], i = 0; i < MAX_DRIVES; i++, pDriveInfo++)
        abRemembered[i] = pDriveInfo->bRemembered;

    //
    // Now update the drive list
    //
    UpdateDriveList();

    //
    // !! BUGBUG !!
    //
    // This will fail if the user add/deletes drives from cmd then
    // attempts to add a connection using winfile without refreshing first.
    //
    for (i = 0; i < MAX_DRIVES; i++) {
        drive = rgiDrive[i];

        if (rgiOld[i] != drive || (abRemembered[drive] && !aDriveInfo[drive].bRemembered)) {
            break;
        }
    }

    if (i < MAX_DRIVES) {
        I_NetCon(rgiDrive[i]);
        return rgiDrive[i];
    } else {
        return -1;
    }
}

DWORD
ReadMoveStatus() {
    IDataObject* pDataObj;
    FORMATETC fmtetcDrop = { 0, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    CLIPFORMAT uFormatEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    FORMATETC fmtetcEffect = { uFormatEffect, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;
    DWORD dwEffect = DROPEFFECT_COPY;

    OleGetClipboard(&pDataObj);  // pDataObj == NULL if error

    if (pDataObj != NULL && pDataObj->GetData(&fmtetcEffect, &stgmed) == S_OK && stgmed.hGlobal != NULL) {
        LPDWORD lpEffect = (LPDWORD)GlobalLock(stgmed.hGlobal);
        if (*lpEffect & DROPEFFECT_COPY)
            dwEffect = DROPEFFECT_COPY;
        if (*lpEffect & DROPEFFECT_MOVE)
            dwEffect = DROPEFFECT_MOVE;
        GlobalUnlock(stgmed.hGlobal);
        ReleaseStgMedium(&stgmed);
    }

    return dwEffect;
}

VOID UpdateMoveStatus(DWORD dwEffect) {
    SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)(dwEffect == DROPEFFECT_MOVE ? TEXT("MOVE PENDING") : NULL));
}
