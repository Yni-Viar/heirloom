/********************************************************************

   wfdlgs3.c

   Windows File System Dialog procedures

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#include "winfile.h"
#include "lfn.h"
#include "wfcopy.h"
#include <shlobj.h>
#include "resize.h"

#define LABEL_NTFS_MAX 32
#define LABEL_FAT_MAX 11
#define CCH_VERSION 40
#define CCH_DRIVE 3

DWORD WINAPI FormatDrive(IN PVOID ThreadParameter);
DWORD WINAPI CopyDiskette(IN PVOID ThreadParameter);
void SwitchToSafeDrive();
void MDIClientSizeChange(HWND hwndActive, int iFlags);

void CancelDlgQuit();

BOOL GetProductVersion(WORD* pwMajor, WORD* pwMinor, WORD* pwBuild, WORD* pwRevision);

DWORD ulTotalSpace, ulSpaceAvail;

typedef enum { FDC_FALSE, FDC_FALSE_Q, FDC_TRUE } FDC_RET;

FDC_RET
FillDriveCapacity(HWND hDlg, int nDrive, FMIFS_MEDIA_TYPE fmSelect, BOOL fDoPopup) {
    FMIFS_MEDIA_TYPE fmMedia[12];  // Number of types in enumeration
    WCHAR wchDrive[4] = L"A:";
    DWORD MediaCount;
    int index;
    UINT uiCount;
    WCHAR szTemp[32];

    int iCurSel = 0;

    SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_RESETCONTENT, 0, 0L);

    wchDrive[0] += nDrive;

    if (!(*lpfnQuerySupportedMedia)((LPWSTR)wchDrive, fmMedia, 12L, (PDWORD)&MediaCount)) {
        return FDC_FALSE;
    }

    if (MediaCount == 1 && fmMedia[0] == FmMediaRemovable) {
        WCHAR szTmpStr[256];

        /*
         * We can't format this type of drive, tell the user to run WinDisk.Exe
         */
        if (fDoPopup) {
            LoadString(hAppInstance, IDS_CANTFORMAT, szTmpStr, COUNTOF(szTmpStr));
            wsprintf(szMessage, szTmpStr, wchDrive);

            LoadString(hAppInstance, IDS_CANTFORMATTITLE, szTmpStr, COUNTOF(szTmpStr));

            MessageBox(hDlg, szMessage, szTmpStr, MB_ICONEXCLAMATION | MB_OK);
        }
        return FDC_FALSE_Q;
    }

    for (index = 0, uiCount = 0; uiCount < MediaCount; uiCount++) {
        // If our selection == the media count, select it now!

        if (fmSelect == fmMedia[uiCount])
            iCurSel = index;

        switch (fmMedia[uiCount]) {
            case FmMediaUnknown:
            case FmMediaFixed:
            case FmMediaRemovable:  // Removable media other than floppy
                break;

            case FmMediaF5_1Pt2_512:  // 5.25", 1.2MB,  512 bytes/sector
                LoadString(hAppInstance, IDS_12MB, szTemp, COUNTOF(szTemp));
                wsprintf(szTitle, szTemp, szDecimal);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF5_1Pt2_512, 0));
                break;

            case FmMediaF5_360_512:  // 5.25", 360KB,  512 bytes/sector
                LoadString(hAppInstance, IDS_360KB, szTitle, COUNTOF(szTitle));
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF5_360_512, 0));
                break;

            case FmMediaF5_320_512:   // 5.25", 320KB,  512 bytes/sector
            case FmMediaF5_320_1024:  // 5.25", 320KB,  1024 bytes/sector
            case FmMediaF5_180_512:   // 5.25", 180KB,  512 bytes/sector
            case FmMediaF5_160_512:   // 5.25", 160KB,  512 bytes/sector
                break;

            case FmMediaF3_2Pt88_512:  // 3.5",  2.88MB, 512 bytes/sector
                LoadString(hAppInstance, IDS_288MB, szTemp, COUNTOF(szTemp));
                wsprintf(szTitle, szTemp, szDecimal);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF3_2Pt88_512, 0));
                break;

            case FmMediaF3_1Pt44_512:  // 3.5",  1.44MB, 512 bytes/sector
                LoadString(hAppInstance, IDS_144MB, szTemp, COUNTOF(szTemp));
                wsprintf(szTitle, szTemp, szDecimal);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF3_1Pt44_512, 0));
                break;

            case FmMediaF3_720_512:  // 3.5",  720KB,  512 bytes/sector
                LoadString(hAppInstance, IDS_720KB, szTitle, COUNTOF(szTitle));
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF3_720_512, 0));
                break;

            case FmMediaF3_20Pt8_512:
                LoadString(hAppInstance, IDS_2080MB, szTemp, COUNTOF(szTemp));
                wsprintf(szTitle, szTemp, szDecimal);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_INSERTSTRING, index, (LPARAM)szTitle);
                SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETITEMDATA, index++, MAKELONG(FmMediaF3_20Pt8_512, 0));
                break;

            default:
                break;
        }
    }

    SendDlgItemMessage(hDlg, IDD_HIGHCAP, CB_SETCURSEL, iCurSel, 0L);

    return (FDC_TRUE);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  AboutDlgProc() -  DialogProc callback function for ABOUTDLG             */
/*                                                                          */
/*--------------------------------------------------------------------------*/

INT_PTR
CALLBACK
AboutDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) {
    WORD wMajorVersion = 0;
    WORD wMinorVersion = 0;
    WORD wBuildNumber = 0;
    WORD wRevisionNumber = 0;
    WCHAR szVersion[CCH_VERSION] = { 0 };

    switch (wMsg) {
        case WM_INITDIALOG:
            if (GetProductVersion(&wMajorVersion, &wMinorVersion, &wBuildNumber, &wRevisionNumber)) {
                if (SUCCEEDED(StringCchPrintf(
                        szVersion, CCH_VERSION, L"Version %d.%d.%d.%d", (int)wMajorVersion, (int)wMinorVersion,
                        (int)wBuildNumber, (int)wRevisionNumber))) {
                    SetDlgItemText(hDlg, IDD_VERTEXT, szVersion);
                }
            }
            return TRUE;
        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam)) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, IDOK);
                    return TRUE;
            }
    }
    return FALSE;
}

DWORD
WINAPI
FormatDrive(IN PVOID ThreadParameter) {
    // Not supported and should never be called.
    exit(-1);
    return 0;
}

DWORD
WINAPI
CopyDiskette(IN PVOID ThreadParameter) {
    // Not supported and should never be called.
    exit(-1);
    return 0;
}

/*-------------------------  CancelDlgProc
 *
 *  DESCRIPTION:
 *    dialog procedure for the modeless dialog. two main purposes
 *    here:
 *
 *      1. if the user chooses CANCEL we set bCancel to TRUE
 *         which will end the PeekMessage background processing loop
 *
 *      2. handle the private FS_CANCELUPDATE message and draw
 *         a "gas gauge" indication of how FAR the background job
 *         has progressed
 *
 *  ARGUMENTS:
 *      stock dialog proc arguments
 *
 *  RETURN VALUE:
 *      stock dialog proc return value - BOOL
 *
 *  GLOBALS READ:
 *      none
 *
 *  GLOBALS WRITTEN:
 *      CancelInfo structure
 *
 *  MESSAGES:
 *      WM_COMMAND      - handle IDCANCEL by setting bCancel to TRUE
 *                        and calling DestroyWindow to end the dialog
 *
 *      WM_INITDIALOG   - set control text, get coordinates of gas gauge,
 *                        disable main window so we look modal
 *
 *      WM_PAINT        - draw the "gas gauge" control
 *
 *      FS_CANCELUPDATE - the percentage done has changed, so update
 *                        nPercentDrawn and force a repaint
 *
 *  NOTES:
 *
 *    The bCancel global variable is used to communicate
 *    with the main window. If the user chooses to cancel
 *    we set bCancel to TRUE.
 *
 *    When we get the private message FS_CANCELUPDATE
 *    we update the "gas gauge" control that indicates
 *    what percentage of the rectangles have been drawn
 *    so FAR. This shows that we can draw in the dialog
 *    as the looping operation progresses.  (FS_CANCELUPDATE is sent
 *    first to hwndFrame, which sets %completed then sends message to us.)
 *
 */

INT_PTR
CALLBACK
CancelDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static RECT rectGG;  // GasGauge rectangle
    DWORD Ignore;
    WCHAR szTemp[128];
    static BOOL bLastQuick;

    switch (message) {
        case WM_COMMAND:

            switch (GET_WM_COMMAND_ID(wParam, lParam)) {
                case IDCANCEL:

                    DestroyCancelWindow();
                    CancelInfo.bCancel = TRUE;
                    return TRUE;

                case IDD_HIDE:
                    DestroyCancelWindow();
                    return TRUE;
            }
            return TRUE;

        case WM_INITDIALOG: {
            CancelInfo.hCancelDlg = hDlg;
            bLastQuick = TRUE;

            switch (CancelInfo.eCancelType) {
                case _CANCEL_INFO::CANCEL_FORMAT:

                    //
                    // Formatting disk requires that we release any notification
                    // requests on this drive.
                    //
                    NotifyPause(CancelInfo.Info.Format.iFormatDrive, DRIVE_REMOVABLE);

                    break;
                case _CANCEL_INFO::CANCEL_COPY:

                    //
                    // Pause notifications on dest drive.
                    //
                    NotifyPause(CancelInfo.Info.Copy.iDestDrive, DRIVE_REMOVABLE);

                    if (CancelInfo.Info.Copy.bFormatDest) {
                        LoadString(hAppInstance, IDS_FORMATTINGDEST, szTemp, COUNTOF(szTemp));
                    } else {
                        LoadString(hAppInstance, IDS_COPYINGDISKTITLE, szTemp, COUNTOF(szTemp));
                    }
                    SetWindowText(hDlg, szTemp);

                    break;
                default:
                    break;
            }

            if (!CancelInfo.hThread) {
                switch (CancelInfo.eCancelType) {
                    case _CANCEL_INFO::CANCEL_FORMAT:
                        CancelInfo.hThread = CreateThread(
                            NULL,  // Security
                            0L,    // Stack Size
                            FormatDrive, NULL, 0L, &Ignore);
                        break;
                    case _CANCEL_INFO::CANCEL_COPY:
                        CancelInfo.hThread = CreateThread(
                            NULL,  // Security
                            0L,    // Stack Size
                            CopyDiskette, NULL, 0L, &Ignore);
                        break;
                    default:
                        break;
                }
            }

            // Get the coordinates of the gas gauge static control rectangle,
            // and convert them to dialog client area coordinates
            GetClientRect(GetDlgItem(hDlg, IDD_GASGAUGE), &rectGG);
            ClientToScreen(GetDlgItem(hDlg, IDD_GASGAUGE), (LPPOINT)&rectGG.left);
            ClientToScreen(GetDlgItem(hDlg, IDD_GASGAUGE), (LPPOINT)&rectGG.right);
            ScreenToClient(hDlg, (LPPOINT)&rectGG.left);
            ScreenToClient(hDlg, (LPPOINT)&rectGG.right);

            return TRUE;
        }

        case WM_PAINT: {
            HDC hDC;
            PAINTSTRUCT ps;
            WCHAR buffer[32];
            SIZE size;
            int xText, yText;
            int nDivideRects;
            RECT rectDone, rectLeftToDo;

            // The gas gauge is drawn by drawing a text string stating
            // what percentage of the job is done into the middle of
            // the gas gauge rectangle, and by separating that rectangle
            // into two parts: rectDone (the left part, filled in blue)
            // and rectLeftToDo(the right part, filled in white).
            // nDivideRects is the x coordinate that divides these two rects.
            //
            // The text in the blue rectangle is drawn white, and vice versa
            // This is easy to do with ExtTextOut()!

            hDC = BeginPaint(hDlg, &ps);

            //
            // If formatting quick, set this display
            //
            if (CancelInfo.Info.Format.fQuick && _CANCEL_INFO::CANCEL_FORMAT == CancelInfo.eCancelType) {
                LoadString(hAppInstance, IDS_QUICKFORMATTINGTITLE, buffer, COUNTOF(buffer));
                SendDlgItemMessage(hDlg, IDD_TEXT1, WM_SETTEXT, 0, (LPARAM)szNULL);

                bLastQuick = TRUE;

            } else {
                if (bLastQuick) {
                    LoadString(hAppInstance, IDS_PERCENTCOMPLETE, buffer, COUNTOF(buffer));
                    SendDlgItemMessage(hDlg, IDD_TEXT1, WM_SETTEXT, 0, (LPARAM)buffer);

                    bLastQuick = FALSE;
                }

                wsprintf(buffer, SZ_PERCENTFORMAT, CancelInfo.nPercentDrawn);
            }

            GetTextExtentPoint32(hDC, buffer, lstrlen(buffer), &size);
            xText = rectGG.left + ((rectGG.right - rectGG.left) - size.cx) / 2;
            yText = rectGG.top + ((rectGG.bottom - rectGG.top) - size.cy) / 2;

            nDivideRects = ((rectGG.right - rectGG.left) * CancelInfo.nPercentDrawn) / 100;

            // Paint in the "done so FAR" rectangle of the gas
            // gauge with blue background and white text
            SetRect(&rectDone, rectGG.left, rectGG.top, rectGG.left + nDivideRects, rectGG.bottom);
            SetTextColor(hDC, RGB(255, 255, 255));
            SetBkColor(hDC, RGB(0, 0, 255));

            ExtTextOut(hDC, xText, yText, ETO_CLIPPED | ETO_OPAQUE, &rectDone, buffer, lstrlen(buffer), NULL);

            // Paint in the "still left to do" rectangle of the gas
            // gauge with white background and blue text
            SetRect(&rectLeftToDo, rectGG.left + nDivideRects, rectGG.top, rectGG.right, rectGG.bottom);
            SetTextColor(hDC, RGB(0, 0, 255));
            SetBkColor(hDC, RGB(255, 255, 255));

            ExtTextOut(hDC, xText, yText, ETO_CLIPPED | ETO_OPAQUE, &rectLeftToDo, buffer, lstrlen(buffer), NULL);

            EndPaint(hDlg, &ps);

            return TRUE;
        }

        case FS_CANCELUPDATE:

            InvalidateRect(hDlg, &rectGG, TRUE);
            UpdateWindow(hDlg);
            return TRUE;

        default:
            return FALSE;
    }
}

/////////////////////////////////////////////////////////////////////
//
// Name:     ProgressDialogProc
//
// Synopsis: Modal dialog box for mouse move/copy progress
//
//
//
//
//
//
// Return:
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

INT_PTR
CALLBACK
ProgressDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) {
    static PCOPYINFO pCopyInfo;
    WCHAR szTitle[MAXTITLELEN];

    switch (wMsg) {
        case WM_INITDIALOG:

            hdlgProgress = hDlg;
            pCopyInfo = (PCOPYINFO)lParam;

            // Set the destination directory in the dialog.
            // use IDD_TONAME 'cause IDD_TO gets disabled....

            // The dialog title defaults to "Moving..."
            if (pCopyInfo->dwFunc == FUNC_COPY) {
                LoadString(hAppInstance, IDS_COPYINGTITLE, szTitle, COUNTOF(szTitle));

                SetWindowText(hdlgProgress, szTitle);

            } else {
                SetDlgItemText(hdlgProgress, IDD_TOSTATUS, szNULL);
            }

            //
            // Move/Copy things.
            //

            if (WFMoveCopyDriver(pCopyInfo)) {
                //
                // Error message!!
                //

                EndDialog(hDlg, GetLastError());
            }
            break;

        case FS_COPYDONE:

            //
            // Only cancel out if pCopyInfo == lParam
            // This indicates that the proper thread quit.
            //
            // wParam holds return value
            //

            if (lParam == (LPARAM)pCopyInfo) {
                EndDialog(hDlg, wParam);
            }
            break;

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam)) {
                case IDCANCEL:

                    pCopyInfo->bUserAbort = TRUE;

                    //
                    // What should be the return value??
                    //
                    EndDialog(hDlg, 0);

                    break;

                default:
                    return FALSE;
            }
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

// update all the windows and things after drives have been connected
// or disconnected.

void UpdateConnections(BOOL bUpdateDriveList) {
    HWND hwnd, hwndNext, hwndDrive, hwndTree;
    int i;
    DRIVE drive;
    HCURSOR hCursor;
    LPWSTR lpszVol;
    LPWSTR lpszOldVol;

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    if (bUpdateDriveList) {
        UpdateDriveList();
    }

    // close all windows that have the current drive set to
    // the one we just disconnected

    for (hwnd = GetWindow(hwndMDIClient, GW_CHILD); hwnd; hwnd = hwndNext) {
        hwndNext = GetWindow(hwnd, GW_HWNDNEXT);

        // ignore the titles and search window
        if (GetWindow(hwnd, GW_OWNER) || hwnd == hwndSearch)
            continue;

        drive = (DRIVE)GetWindowLongPtr(hwnd, GWL_TYPE);

        //
        // IsValidDisk uses GetDriveType which was updated if
        // bUpdateDriveList == TRUE.
        //

        if (IsValidDisk(drive)) {
            //
            // Invalidate cache to get real one in case the user reconnected
            // d: from \\popcorn\public to \\rastaman\ntwin
            //
            // Previously used MDI window title to determine if the volume
            // has changed.  Now we will just check DriveInfo structure
            // (bypass status bits).
            //

            //
            // Now only do this for remote drives!
            //

            if (IsRemoteDrive(drive)) {
                R_NetCon(drive);

                if (!WFGetConnection(drive, &lpszVol, FALSE, ALTNAME_REG)) {
                    lpszOldVol = (LPWSTR)GetWindowLongPtr(hwnd, GWL_VOLNAME);

                    if (lpszOldVol && lpszVol) {
                        if (lstrcmpi(lpszVol, lpszOldVol)) {
                            //
                            // updatedrivelist/initdrivebitmaps called above;
                            // don't do here
                            //
                            RefreshWindow(hwnd, FALSE, TRUE);
                        }
                    }
                }
            }

        } else {
            //
            // this drive has gone away
            //
            if (IsLastWindow()) {
                // disconnecting the last drive
                // set this guy to the first non floppy / cd rom

                for (i = 0; i < cDrives; i++) {
                    if (!IsRemovableDrive(rgiDrive[i]) && !IsCDRomDrive(rgiDrive[i])) {
                        SendMessage(hwndDriveBar, FS_SETDRIVE, i, 0L);
                        break;
                    }
                }
            } else if ((hwndTree = HasTreeWindow(hwnd)) && GetWindowLongPtr(hwndTree, GWL_READLEVEL)) {
                //
                // abort tree walk
                //
                bCancelTree = TRUE;

            } else {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
            }
        }
    }

    // why is this here?  Move it further, right redisplay if at all.
    // Reuse hwndDrive as the current window open!

    hwndDrive = (HWND)SendMessage(hwndMDIClient, WM_MDIGETACTIVE, 0, 0L);

    i = (int)GetWindowLongPtr(hwndDrive, GWL_TYPE);

    if (TYPE_SEARCH == i) {
        i = DRIVEID(SearchInfo.szSearch);
    }

    SwitchDriveSelection(hwndDrive);

    MDIClientSizeChange(NULL, DRIVEBAR_FLAG); /* update/resize drive bar */

    ShowCursor(FALSE);
    SetCursor(hCursor);
}

/////////////////////////////////////////////////////////////////////
//
// Name:     CancelDlgQuit
//
// Synopsis: Quits the cancel modeless dialog (status for diskcopy/format)
//
// IN: void
//
// Return:   void
//
// Assumes:  Called from worker thread only; CancelInfo.hThread valid
//
// Effects:  Kills calling thread
//
//
// Notes:
//
/////////////////////////////////////////////////////////////////////

void CancelDlgQuit() {
    //
    // Close thread if successful
    //

    if (CancelInfo.hThread) {
        CloseHandle(CancelInfo.hThread);
        CancelInfo.hThread = NULL;
    } else {
        //
        // BUGBUG
        //
    }

    //
    // At this point, when we call FS_CANCELEND,
    // the other thread thinks that this one has died since
    // CancelInfo.hThread is NULL.
    // This is exactly what we want, since we will very shortly
    // exit after the SendMessage.
    //
    SendMessage(hwndFrame, FS_CANCELEND, 0, 0L);

    ExitThread(0L);
}

void DestroyCancelWindow() {
    if (!CancelInfo.hCancelDlg)
        return;

    if (CancelInfo.bModal) {
        EndDialog(CancelInfo.hCancelDlg, 0);
    } else {
        DestroyWindow(CancelInfo.hCancelDlg);
    }
    CancelInfo.hCancelDlg = NULL;
}

//
// GetProductVersion
// Gets the product version values for the current module
//
// Parameters:
//   pwMajor    - [OUT] A pointer to the major version number
//   pwMinor    - [OUT] A pointer to the minor version number
//   pwBuild    - [OUT] A pointer to the build number
//   pwRevision - [OUT] A pointer to the revision number
//
// Returns TRUE if successful
//
BOOL GetProductVersion(WORD* pwMajor, WORD* pwMinor, WORD* pwBuild, WORD* pwRevision) {
    BOOL success = FALSE;
    WCHAR szCurrentModulePath[MAXPATHLEN];
    DWORD cchPath;
    DWORD cbVerInfo;
    LPVOID pFileVerInfo;
    UINT uLen;
    VS_FIXEDFILEINFO* pFixedFileInfo;

    cchPath = GetModuleFileName(NULL, szCurrentModulePath, MAXPATHLEN);

    if (cchPath && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        cbVerInfo = GetFileVersionInfoSize(szCurrentModulePath, NULL);

        if (cbVerInfo) {
            pFileVerInfo = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbVerInfo);

            if (pFileVerInfo) {
                if (GetFileVersionInfo(szCurrentModulePath, 0, cbVerInfo, pFileVerInfo)) {
                    // Get the pointer to the VS_FIXEDFILEINFO structure
                    if (VerQueryValue(pFileVerInfo, L"\\", (LPVOID*)&pFixedFileInfo, &uLen)) {
                        if (pFixedFileInfo && uLen) {
                            *pwMajor = HIWORD(pFixedFileInfo->dwProductVersionMS);
                            *pwMinor = LOWORD(pFixedFileInfo->dwProductVersionMS);
                            *pwBuild = HIWORD(pFixedFileInfo->dwProductVersionLS);
                            *pwRevision = LOWORD(pFixedFileInfo->dwProductVersionLS);

                            success = TRUE;
                        }
                    }
                }

                HeapFree(GetProcessHeap(), 0, pFileVerInfo);
            }
        }
    }

    return success;
}
