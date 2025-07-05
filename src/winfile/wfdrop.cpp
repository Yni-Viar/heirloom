/********************************************************************

   wfdrop.c

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#define INITGUID

#include "winfile.h"
#include "wfdrop.h"
#include "treectl.h"
#include "wfutil.h"
#include "wfcopy.h"
#include "stringconstants.h"

#include <ole2.h>
#include <shlobj.h>
#include <strsafe.h>

#ifndef GUID_DEFINED
DEFINE_OLEGUID(IID_IUnknown, 0x00000000L, 0, 0);
DEFINE_OLEGUID(IID_IDropSource, 0x00000121, 0, 0);
DEFINE_OLEGUID(IID_IDropTarget, 0x00000122, 0, 0);
#endif

HRESULT CreateDropTarget(HWND hwnd, WF_IDropTarget** ppDropTarget);
void DropData(WF_IDropTarget* This, IDataObject* pDataObject, DWORD dwEffect);

// Helper for debug output
void DebugLogDrop(LPCWSTR format, ...) {
    WCHAR buffer[512];
    va_list args;
    va_start(args, format);
    StringCchVPrintfW(buffer, ARRAYSIZE(buffer), format, args);
    va_end(args);
    OutputDebugStringW(buffer);
}

void PaintRectItem(WF_IDropTarget* This, POINTL* ppt) {
    HWND hwndLB;
    DWORD iItem;
    POINT pt;
    BOOL fTree;

    // could be either tree control or directory list box
    hwndLB = GetDlgItem(This->m_hWnd, IDCW_LISTBOX);
    fTree = FALSE;
    if (hwndLB == NULL) {
        hwndLB = GetDlgItem(This->m_hWnd, IDCW_TREELISTBOX);
        fTree = TRUE;

        if (hwndLB == NULL)
            return;
    }

    if (ppt != NULL) {
        pt.x = ppt->x;
        pt.y = ppt->y;
        ScreenToClient(hwndLB, &pt);

        iItem = (DWORD)SendMessage(hwndLB, LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));
        iItem &= 0xffff;
        if (This->m_iItemSelected != -1 && This->m_iItemSelected == iItem)
            return;
    }

    // unpaint old item
    if (This->m_iItemSelected != -1) {
        if (fTree)
            RectTreeItem(hwndLB, This->m_iItemSelected, FALSE);
        else {
            // Modern drag-drop: Just invalidate the item instead of using DSRectItem
            RECT rc;
            SendMessage(hwndLB, LB_GETITEMRECT, This->m_iItemSelected, (LPARAM)&rc);
            InvalidateRect(hwndLB, &rc, TRUE);
        }

        This->m_iItemSelected = (DWORD)-1;
    }

    // if new item, paint it.
    if (ppt != NULL) {
        if (fTree) {
            if (RectTreeItem(hwndLB, iItem, TRUE))
                This->m_iItemSelected = iItem;
        } else {
            // Modern drag-drop: Use a simple approach to highlight the item
            RECT rc;
            if (SendMessage(hwndLB, LB_GETITEMRECT, iItem, (LPARAM)&rc) != LB_ERR) {
                HDC hdc = GetDC(hwndLB);
                if (hdc) {
                    HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                    if (hbr) {
                        FrameRect(hdc, &rc, hbr);
                        DeleteObject(hbr);
                    }
                    ReleaseDC(hwndLB, hdc);
                    This->m_iItemSelected = iItem;
                }
            }
        }
    }
}

LPWSTR QuotedDropList(IDataObject* pDataObject) {
    HDROP hdrop;
    DWORD cFiles, iFile, cchFiles;
    LPWSTR szFiles = NULL, pch;
    FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed;

    DebugLogDrop(L"QuotedDropList called\n");

    HRESULT hr = pDataObject->GetData(&fmtetc, &stgmed);
    DebugLogDrop(L"  GetData for CF_HDROP result: 0x%08X\n", hr);

    if (hr == S_OK) {
        // Yippie! the data is there, so go get it!
        hdrop = (HDROP)stgmed.hGlobal;

        cFiles = DragQueryFile(hdrop, 0xffffffff, NULL, 0);
        if (cFiles == 0) {
            DebugLogDrop(L"  No files in HDROP\n");
            ReleaseStgMedium(&stgmed);
            return NULL;
        }

        DebugLogDrop(L"  HDROP contains %d files\n", cFiles);

        cchFiles = 0;
        for (iFile = 0; iFile < cFiles; iFile++)
            cchFiles += DragQueryFile(hdrop, iFile, NULL, 0) + 1 + 2;

        pch = szFiles = (LPWSTR)LocalAlloc(LMEM_FIXED, cchFiles * sizeof(WCHAR));
        if (!szFiles) {
            DebugLogDrop(L"  Failed to allocate memory for file list\n");
            ReleaseStgMedium(&stgmed);
            return NULL;
        }

        for (iFile = 0; iFile < cFiles; iFile++) {
            DWORD cchFile;

            *pch++ = CHAR_DQUOTE;

            cchFile = DragQueryFile(hdrop, iFile, pch, cchFiles);
            pch += cchFile;
            *pch++ = CHAR_DQUOTE;

            if (iFile + 1 < cFiles)
                *pch++ = CHAR_SPACE;
            else
                *pch = CHAR_NULL;

            cchFiles -= cchFile + 1 + 2;
        }

        // release the data using the COM API
        ReleaseStgMedium(&stgmed);
        DebugLogDrop(L"  Built file list successfully\n");
    } else {
        DebugLogDrop(L"  Failed to get HDROP data\n");
    }

    return szFiles;
}

HDROP CreateDropFiles(POINT pt, BOOL fNC, LPWSTR pszFiles) {
    HANDLE hDrop;
    LPBYTE lpList;
    SIZE_T cbList;
    LPWSTR szSrc;

    LPDROPFILES lpdfs;
    WCHAR szFile[MAXPATHLEN];

    cbList = sizeof(DROPFILES) + sizeof(WCHAR);

    szSrc = pszFiles;
    while (szSrc = GetNextFile(szSrc, szFile, COUNTOF(szFile))) {
        QualifyPath(szFile);

        cbList += (wcslen(szFile) + 1) * sizeof(WCHAR);
    }

    // Add extra space for a double null terminator
    cbList += sizeof(WCHAR);

    hDrop = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, cbList);
    if (!hDrop)
        return NULL;

    lpdfs = (LPDROPFILES)GlobalLock(hDrop);

    lpdfs->pFiles = sizeof(DROPFILES);
    lpdfs->pt = pt;
    lpdfs->fNC = fNC;
    lpdfs->fWide = TRUE;

    lpList = (LPBYTE)lpdfs + sizeof(DROPFILES);
    szSrc = pszFiles;

    while (szSrc = GetNextFile(szSrc, szFile, COUNTOF(szFile))) {
        QualifyPath(szFile);

        lstrcpy((LPWSTR)lpList, szFile);

        lpList += (wcslen(szFile) + 1) * sizeof(WCHAR);
    }

    // Add a final NULL to create a double-NULL terminator
    *((WCHAR*)lpList) = L'\0';

    GlobalUnlock(hDrop);

    return (HDROP)hDrop;
}

#define BLOCK_SIZE 512

// Create all intermediate directories in a path
static BOOL CreateIntermediateDirectories(LPCWSTR szPath) {
    WCHAR szDirPath[MAXPATHLEN];
    WCHAR* p;

    // Make a copy we can modify
    lstrcpy(szDirPath, szPath);

    // Find the last backslash in the path
    p = wcsrchr(szDirPath, L'\\');
    if (!p)
        return TRUE;  // No directory part

    *p = L'\0';  // Truncate to get directory path

    // Check if the directory already exists
    if (GetFileAttributes(szDirPath) != INVALID_FILE_ATTRIBUTES)
        return TRUE;

    // Recursively create parent directories
    if (!CreateIntermediateDirectories(szDirPath))
        return FALSE;

    // Create this directory
    return CreateDirectory(szDirPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

static HRESULT StreamToFile(IStream* stream, WCHAR* szFile) {
    byte buffer[BLOCK_SIZE];
    DWORD bytes_read;
    DWORD bytes_written;
    HRESULT hr;
    HANDLE hFile;

    // Create intermediate directories if needed
    CreateIntermediateDirectories(szFile);

    hFile = CreateFile(
        szFile, FILE_READ_DATA | FILE_WRITE_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        do {
            hr = stream->Read(buffer, BLOCK_SIZE, &bytes_read);
            bytes_written = 0;
            if (SUCCEEDED(hr) && bytes_read) {
                if (!WriteFile(hFile, buffer, bytes_read, &bytes_written, NULL)) {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    bytes_written = 0;
                }
            }
        } while (S_OK == hr && bytes_written != 0);
        CloseHandle(hFile);
        if (FAILED(hr))
            DeleteFile(szFile);
        else
            hr = S_OK;
    } else
        hr = HRESULT_FROM_WIN32(GetLastError());

    return hr;
}

LPWSTR QuotedContentList(IDataObject* pDataObject) {
    FILEGROUPDESCRIPTOR* file_group_descriptor = NULL;
    FILEDESCRIPTOR file_descriptor;
    HRESULT hr;
    LPWSTR szFiles = NULL;

    // Try UTF-16 format first
    unsigned short cp_format_descriptorw = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
    unsigned short cp_format_contentsw = RegisterClipboardFormat(CFSTR_FILECONTENTS);

    // Set up format structure for the descriptor and contents
    FORMATETC descriptor_formatw = { cp_format_descriptorw, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    FORMATETC contents_formatw = { cp_format_contentsw, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM };

    // Check for UTF-16 descriptor format type
    hr = pDataObject->QueryGetData(&descriptor_formatw);
    if (hr == S_OK) {
        // Check for contents format type
        hr = pDataObject->QueryGetData(&contents_formatw);
        if (hr == S_OK) {
            // Get the descriptor information
            STGMEDIUM sm_desc = { 0 };
            STGMEDIUM sm_content = { 0 };
            unsigned int file_index;
            size_t cchTempPath, cchFiles;
            WCHAR szTempPath[MAXPATHLEN + 1];

            hr = pDataObject->GetData(&descriptor_formatw, &sm_desc);
            if (hr != S_OK)
                goto try_ansi_format;

            file_group_descriptor = (FILEGROUPDESCRIPTOR*)GlobalLock(sm_desc.hGlobal);

            GetTempPath(MAXPATHLEN, szTempPath);
            cchTempPath = wcslen(szTempPath);

            // calc total size of file names
            cchFiles = 0;
            for (file_index = 0; file_index < file_group_descriptor->cItems; file_index++) {
                file_descriptor = file_group_descriptor->fgd[file_index];
                cchFiles += 1 + cchTempPath + 1 + wcslen(file_descriptor.cFileName) + 2;
            }

            szFiles = (LPWSTR)LocalAlloc(LMEM_FIXED, cchFiles * sizeof(WCHAR));
            if (!szFiles) {
                GlobalUnlock(sm_desc.hGlobal);
                ReleaseStgMedium(&sm_desc);
                return NULL;
            }
            szFiles[0] = '\0';

            // For each file, get the name and copy the stream to a file
            for (file_index = 0; file_index < file_group_descriptor->cItems; file_index++) {
                file_descriptor = file_group_descriptor->fgd[file_index];
                contents_formatw.lindex = file_index;
                memset(&sm_content, 0, sizeof(sm_content));
                hr = pDataObject->GetData(&contents_formatw, &sm_content);

                if (hr == S_OK) {
                    // Dump stream to a file
                    WCHAR szTempFile[MAXPATHLEN * 2 + 1];

                    lstrcpy(szTempFile, szTempPath);
                    AddBackslash(szTempFile);
                    lstrcat(szTempFile, file_descriptor.cFileName);

                    hr = StreamToFile(sm_content.pstm, szTempFile);

                    if (hr == S_OK) {
                        CheckEsc(szTempFile);

                        if (szFiles[0] != '\0')
                            lstrcat(szFiles, L" ");
                        lstrcat(szFiles, szTempFile);
                    }

                    ReleaseStgMedium(&sm_content);
                }
            }

            GlobalUnlock(sm_desc.hGlobal);
            ReleaseStgMedium(&sm_desc);

            if (szFiles[0] == '\0') {
                // nothing to copy
                MessageBeep(0);
                LocalFree((HLOCAL)szFiles);
                szFiles = NULL;
            }

            return szFiles;
        }
    }

try_ansi_format:
    // Try ANSI format if Unicode format failed
    unsigned short cp_format_descriptor = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
    unsigned short cp_format_contents = RegisterClipboardFormat(CFSTR_FILECONTENTS);

    // Set up format structure for the descriptor and contents
    FORMATETC descriptor_format = { cp_format_descriptor, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    FORMATETC contents_format = { cp_format_contents, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM };

    // Check for descriptor format type
    hr = pDataObject->QueryGetData(&descriptor_format);
    if (hr == S_OK) {
        // Check for contents format type
        hr = pDataObject->QueryGetData(&contents_format);
        if (hr == S_OK) {
            // Get the descriptor information
            STGMEDIUM sm_desc = { 0 };
            STGMEDIUM sm_content = { 0 };
            unsigned int file_index;
            size_t cchTempPath, cchFiles;
            WCHAR szTempPath[MAXPATHLEN + 1];

            hr = pDataObject->GetData(&descriptor_format, &sm_desc);
            if (hr != S_OK)
                return NULL;

            file_group_descriptor = (FILEGROUPDESCRIPTOR*)GlobalLock(sm_desc.hGlobal);

            GetTempPath(MAXPATHLEN, szTempPath);
            cchTempPath = wcslen(szTempPath);

            // calc total size of file names
            cchFiles = 0;
            for (file_index = 0; file_index < file_group_descriptor->cItems; file_index++) {
                file_descriptor = file_group_descriptor->fgd[file_index];
                cchFiles += 1 + cchTempPath + 1 + wcslen(file_descriptor.cFileName) + 2;
            }

            szFiles = (LPWSTR)LocalAlloc(LMEM_FIXED, cchFiles * sizeof(WCHAR));
            if (!szFiles) {
                GlobalUnlock(sm_desc.hGlobal);
                ReleaseStgMedium(&sm_desc);
                return NULL;
            }
            szFiles[0] = '\0';

            // For each file, get the name and copy the stream to a file
            for (file_index = 0; file_index < file_group_descriptor->cItems; file_index++) {
                file_descriptor = file_group_descriptor->fgd[file_index];
                contents_format.lindex = file_index;
                memset(&sm_content, 0, sizeof(sm_content));
                hr = pDataObject->GetData(&contents_format, &sm_content);

                if (hr == S_OK) {
                    // Dump stream to a file
                    WCHAR szTempFile[MAXPATHLEN * 2 + 1];

                    lstrcpy(szTempFile, szTempPath);
                    AddBackslash(szTempFile);
                    lstrcat(szTempFile, file_descriptor.cFileName);

                    hr = StreamToFile(sm_content.pstm, szTempFile);

                    if (hr == S_OK) {
                        CheckEsc(szTempFile);

                        if (szFiles[0] != '\0')
                            lstrcat(szFiles, L" ");
                        lstrcat(szFiles, szTempFile);
                    }

                    ReleaseStgMedium(&sm_content);
                }
            }

            GlobalUnlock(sm_desc.hGlobal);
            ReleaseStgMedium(&sm_desc);

            if (szFiles[0] == '\0') {
                // nothing to copy
                MessageBeep(0);
                LocalFree((HLOCAL)szFiles);
                szFiles = NULL;
            }
        }
    }
    return szFiles;
}

//
//	QueryDataObject private helper routine
//
static BOOL QueryDataObject(IDataObject* pDataObject) {
    FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // Check for the standard HDROP format
    HRESULT hr = pDataObject->QueryGetData(&fmtetc);
    DebugLogDrop(L"QueryDataObject - CF_HDROP: 0x%08X\n", hr);
    if (hr == S_OK)
        return TRUE;

    // Check for CFSTR_FILEDESCRIPTOR format
    unsigned short cp_format_descriptor = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
    FORMATETC descriptor_format = { cp_format_descriptor, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    hr = pDataObject->QueryGetData(&descriptor_format);
    DebugLogDrop(L"QueryDataObject - CFSTR_FILEDESCRIPTOR: 0x%08X\n", hr);
    if (hr == S_OK)
        return TRUE;

    // Check for CFSTR_FILEDESCRIPTORW format
    unsigned short cp_format_descriptorw = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
    FORMATETC descriptor_formatw = { cp_format_descriptorw, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    hr = pDataObject->QueryGetData(&descriptor_formatw);
    DebugLogDrop(L"QueryDataObject - CFSTR_FILEDESCRIPTORW: 0x%08X\n", hr);
    if (hr == S_OK)
        return TRUE;

    return FALSE;
}

//
//	DropEffect private helper routine
//
static DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed) {
    DWORD dwEffect = 0;

    // 1. Default to copy if the destination (pt) is valid and copy is allowed
    if (dwAllowed & DROPEFFECT_COPY)
        dwEffect = DROPEFFECT_COPY;

    // 2. Allow move if it's permitted
    if (dwAllowed & DROPEFFECT_MOVE)
        dwEffect = DROPEFFECT_MOVE;

    // 3. Override based on keyboard state
    if (grfKeyState & MK_CONTROL) {
        dwEffect = dwAllowed & DROPEFFECT_COPY;
    } else if (grfKeyState & MK_SHIFT) {
        dwEffect = dwAllowed & DROPEFFECT_MOVE;
    } else if (grfKeyState & MK_ALT) {
        // Alt suggests a shortcut/link operation
        dwEffect = dwAllowed & DROPEFFECT_LINK;
    }

    // If no valid effect was determined, default to none
    if (dwEffect == 0) {
        dwEffect = DROPEFFECT_NONE;
    }

    DebugLogDrop(L"DropEffect - KeyState: 0x%08X, Allowed: 0x%08X, Result: 0x%08X\n", grfKeyState, dwAllowed, dwEffect);
    return dwEffect;
}

//
// IUnknown methods
//

ULONG STDMETHODCALLTYPE WF_IDropTarget::AddRef() {
    LONG count = InterlockedIncrement(&m_lRefCount);
    DebugLogDrop(L"WF_IDropTarget::AddRef - Count: %d\n", count);
    return count;
}

HRESULT STDMETHODCALLTYPE WF_IDropTarget::QueryInterface(REFIID riid, LPVOID* ppvObject) {
    DebugLogDrop(L"WF_IDropTarget::QueryInterface\n");
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget)) {
        AddRef();
        *ppvObject = this;
        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE WF_IDropTarget::Release() {
    LONG count = InterlockedDecrement(&m_lRefCount);
    DebugLogDrop(L"WF_IDropTarget::Release - Count: %d\n", count);

    if (count == 0) {
        delete this;
        return 0;
    } else {
        return count;
    }
}

//
// IDropTarget methods
//

HRESULT STDMETHODCALLTYPE
WF_IDropTarget::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    DebugLogDrop(L"WF_IDropTarget::DragEnter - KeyState: 0x%08X, Effect: 0x%08X\n", grfKeyState, *pdwEffect);

    // does the dataobject contain data we want?
    m_fAllowDrop = QueryDataObject(pDataObject);
    DebugLogDrop(L"  AllowDrop: %d\n", m_fAllowDrop);

    if (m_fAllowDrop) {
        // get the dropeffect based on keyboard state
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

        SetFocus(m_hWnd);

        PaintRectItem(this, &pt);
    } else {
        *pdwEffect = DROPEFFECT_NONE;
    }

    DebugLogDrop(L"  Final effect: 0x%08X\n", *pdwEffect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    DebugLogDrop(L"WF_IDropTarget::DragOver - KeyState: 0x%08X, Effect: 0x%08X\n", grfKeyState, *pdwEffect);

    if (m_fAllowDrop) {
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        PaintRectItem(this, &pt);
    } else {
        *pdwEffect = DROPEFFECT_NONE;
    }

    DebugLogDrop(L"  Final effect: 0x%08X\n", *pdwEffect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IDropTarget::DragLeave() {
    DebugLogDrop(L"WF_IDropTarget::DragLeave\n");
    PaintRectItem(this, NULL);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
WF_IDropTarget::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    DebugLogDrop(L"WF_IDropTarget::Drop - KeyState: 0x%08X, Effect: 0x%08X\n", grfKeyState, *pdwEffect);

    if (m_fAllowDrop) {
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        DebugLogDrop(L"  Dropping with effect: 0x%08X\n", *pdwEffect);

        DropData(this, pDataObject, *pdwEffect);
    } else {
        *pdwEffect = DROPEFFECT_NONE;
        DebugLogDrop(L"  Drop not allowed\n");
    }

    return S_OK;
}

// Constructor
WF_IDropTarget::WF_IDropTarget(HWND hwnd) {
    m_lRefCount = 1;
    m_hWnd = hwnd;
    m_fAllowDrop = FALSE;
    m_iItemSelected = (DWORD)-1;
    m_pDataObject = NULL;
    DebugLogDrop(L"WF_IDropTarget created for HWND: 0x%p\n", hwnd);
}

void DropData(WF_IDropTarget* This, IDataObject* pDataObject, DWORD dwEffect) {
    // construct a FORMATETC object
    HWND hwndLB;
    BOOL fTree;
    LPWSTR szFiles = NULL;
    WCHAR szDest[MAXPATHLEN];
    WCHAR szSrc[MAXPATHLEN];
    BOOL bSameLocation = FALSE;

    hwndLB = GetDlgItem(This->m_hWnd, IDCW_LISTBOX);
    fTree = FALSE;
    if (hwndLB == NULL) {
        hwndLB = GetDlgItem(This->m_hWnd, IDCW_TREELISTBOX);
        fTree = TRUE;

        if (hwndLB == NULL)
            return;
    }

    // Store the current directory as source for comparison
    SendMessage(This->m_hWnd, FS_GETDIRECTORY, COUNTOF(szSrc), (LPARAM)szSrc);

    // if item selected, add path
    if (fTree) {
        PDNODE pNode;

        // odd
        if (This->m_iItemSelected == -1)
            return;

        if (SendMessage(hwndLB, LB_GETTEXT, This->m_iItemSelected, (LPARAM)&pNode) == LB_ERR)
            return;

        GetTreePath(pNode, szDest);
    } else {
        LPXDTA lpxdta;

        SendMessage(This->m_hWnd, FS_GETDIRECTORY, COUNTOF(szDest), (LPARAM)szDest);

        if (This->m_iItemSelected != -1) {
            SendMessage(hwndLB, LB_GETTEXT, This->m_iItemSelected, (LPARAM)(LPWSTR)&lpxdta);

            // Bug fix 1: Only treat the selected item as a drop target if it's a directory
            if (lpxdta->dwAttrs & FILE_ATTRIBUTE_DIRECTORY) {
                AddBackslash(szDest);
                lstrcat(szDest, MemGetFileName(lpxdta));
            }
            // If it's a file, we just drop to the containing folder (szDest already set above)
        }
    }

    AddBackslash(szDest);
    lstrcat(szDest, kStarDotStar);  // put files in this dir

    CheckEsc(szDest);

    // See if the dataobject contains any TEXT stored as a HGLOBAL
    if ((szFiles = QuotedDropList(pDataObject)) == NULL) {
        szFiles = QuotedContentList(pDataObject);
        dwEffect = DROPEFFECT_MOVE;
    }

    if (szFiles != NULL) {
        // Bug fix 2: Check if source and destination are the same
        // Compare the source directory (current window) with the destination directory
        WCHAR szDestDir[MAXPATHLEN];
        lstrcpy(szDestDir, szDest);

        // Remove the trailing \*.* pattern to get just the directory
        LPWSTR pszPattern = wcsstr(szDestDir, kStarDotStar);
        if (pszPattern) {
            *pszPattern = L'\0';
            // Remove trailing backslash
            int len = lstrlen(szDestDir);
            if (len > 0 && szDestDir[len - 1] == L'\\') {
                szDestDir[len - 1] = L'\0';
            }
        }

        // Compare source and destination directories
        if (lstrcmpi(szSrc, szDestDir) == 0) {
            bSameLocation = TRUE;
        }

        if (!bSameLocation) {
            SetFocus(This->m_hWnd);
            DMMoveCopyHelper(szFiles, szDest, dwEffect == DROPEFFECT_COPY);
        }
        // If same location, silently ignore the drop (no prompt, no action)

        LocalFree((HLOCAL)szFiles);
    }
}

void RegisterDropWindow(HWND hwnd, WF_IDropTarget** ppDropTarget) {
    WF_IDropTarget* pDropTarget;

    DebugLogDrop(L"RegisterDropWindow called for HWND: 0x%p\n", hwnd);

    HRESULT hr = CreateDropTarget(hwnd, &pDropTarget);
    DebugLogDrop(L"  CreateDropTarget result: 0x%08X\n", hr);

    // acquire a strong lock
    hr = CoLockObjectExternal((struct IUnknown*)pDropTarget, TRUE, FALSE);
    DebugLogDrop(L"  CoLockObjectExternal result: 0x%08X\n", hr);

    // tell OLE that the window is a drop target
    hr = RegisterDragDrop(hwnd, (LPDROPTARGET)pDropTarget);
    DebugLogDrop(L"  RegisterDragDrop result: 0x%08X\n", hr);

    *ppDropTarget = pDropTarget;
}

void UnregisterDropWindow(HWND hwnd, IDropTarget* pDropTarget) {
    DebugLogDrop(L"UnregisterDropWindow called for HWND: 0x%p\n", hwnd);

    // remove drag+drop
    HRESULT hr = RevokeDragDrop(hwnd);
    DebugLogDrop(L"  RevokeDragDrop result: 0x%08X\n", hr);

    // remove the strong lock
    hr = CoLockObjectExternal((struct IUnknown*)pDropTarget, FALSE, TRUE);
    DebugLogDrop(L"  CoLockObjectExternal result: 0x%08X\n", hr);

    // release our own reference
    pDropTarget->Release();
}

HRESULT CreateDropTarget(HWND hwnd, WF_IDropTarget** ppDropTarget) {
    DebugLogDrop(L"CreateDropTarget called for HWND: 0x%p\n", hwnd);

    if (ppDropTarget == 0)
        return E_INVALIDARG;

    *ppDropTarget = new WF_IDropTarget(hwnd);

    return (*ppDropTarget) ? S_OK : E_OUTOFMEMORY;
}
