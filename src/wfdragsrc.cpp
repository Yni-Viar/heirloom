/********************************************************************

   wfdragsrc.cpp

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#include "winfile.h"
#include "wfdrop.h"
#include "wfdragsrc.h"

#include <ole2.h>
#include <shlobj.h>
#include <strsafe.h>
#include <shlwapi.h>
#include <shobjidl.h>  // For shell item identifiers

// IUnknown methods for WF_IDropSource
ULONG STDMETHODCALLTYPE WF_IDropSource::AddRef(void) {
    return InterlockedIncrement(&m_lRefCount);
}

ULONG STDMETHODCALLTYPE WF_IDropSource::Release(void) {
    LONG count = InterlockedDecrement(&m_lRefCount);
    if (count == 0) {
        delete this;
        return 0;
    }
    return count;
}

HRESULT STDMETHODCALLTYPE WF_IDropSource::QueryInterface(REFIID riid, void** ppvObject) {
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropSource)) {
        AddRef();
        *ppvObject = this;
        return S_OK;
    } else {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

// IDropSource methods
HRESULT STDMETHODCALLTYPE WF_IDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) {
    // Cancel drag if the escape key was pressed
    if (fEscapePressed) {
        return DRAGDROP_S_CANCEL;
    }
    // End drag operation when the left mouse button is released
    else if (!(grfKeyState & MK_LBUTTON)) {
        return DRAGDROP_S_DROP;
    }
    // Continue dragging
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IDropSource::GiveFeedback(DWORD dwEffect) {
    // Use standard cursor
    return DRAGDROP_S_USEDEFAULTCURSORS;
}

// Constructor
WF_IDropSource::WF_IDropSource() {
    m_lRefCount = 1;
}

// Create a new IDropSource object
HRESULT CreateDropSource(WF_IDropSource** ppDropSource) {
    if (ppDropSource == NULL)
        return E_INVALIDARG;

    *ppDropSource = new WF_IDropSource();
    return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;
}

// DataObject implementation
WF_IDataObject::WF_IDataObject() {
    m_lRefCount = 1;
    m_nNumFormats = 0;
    m_pFormatEtc = NULL;
    m_pStgMedium = NULL;
}

WF_IDataObject::~WF_IDataObject() {
    // Free resources
    if (m_pFormatEtc) {
        for (LONG i = 0; i < m_nNumFormats; i++) {
            // Free STGMEDIUM
            if (m_pStgMedium[i].pUnkForRelease == NULL) {
                ReleaseStgMedium(&m_pStgMedium[i]);
            }
        }
        delete[] m_pFormatEtc;
        delete[] m_pStgMedium;
    }
}

// IUnknown implementation
ULONG STDMETHODCALLTYPE WF_IDataObject::AddRef() {
    return InterlockedIncrement(&m_lRefCount);
}

ULONG STDMETHODCALLTYPE WF_IDataObject::Release() {
    LONG count = InterlockedDecrement(&m_lRefCount);
    if (count == 0) {
        delete this;
        return 0;
    }
    return count;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::QueryInterface(REFIID riid, void** ppvObject) {
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDataObject)) {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

// IDataObject implementation
HRESULT STDMETHODCALLTYPE WF_IDataObject::GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium) {
    if (pFormatEtc == NULL || pMedium == NULL)
        return E_INVALIDARG;

    // Find the requested format
    LONG idx = IndexOfFormat(pFormatEtc);
    if (idx == -1)
        return DV_E_FORMATETC;

    // Copy the medium
    return CopyMedium(pMedium, &m_pStgMedium[idx], &m_pFormatEtc[idx]);
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::QueryGetData(FORMATETC* pFormatEtc) {
    if (pFormatEtc == NULL)
        return E_INVALIDARG;

    // Check if this format is supported
    LONG idx = IndexOfFormat(pFormatEtc);
    return (idx != -1) ? S_OK : DV_E_FORMATETC;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::GetCanonicalFormatEtc(FORMATETC* pFormatEtcIn, FORMATETC* pFormatEtcOut) {
    if (pFormatEtcOut == NULL)
        return E_INVALIDARG;

    // Irrelevant for this implementation
    pFormatEtcOut->ptd = NULL;
    return DATA_S_SAMEFORMATETC;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease) {
    // For our implementation, this is not needed as we only create data objects
    return E_NOTIMPL;
}

// Helper to copy a STGMEDIUM
HRESULT WF_IDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc) {
    *pMedDest = *pMedSrc;

    if (pMedSrc->pUnkForRelease) {
        // Source has release object, so can just copy
        pMedSrc->pUnkForRelease->AddRef();
        return S_OK;
    }

    switch (pMedSrc->tymed) {
        case TYMED_HGLOBAL:
            pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, 0);
            if (!pMedDest->hGlobal)
                return E_OUTOFMEMORY;
            break;

        case TYMED_GDI:
            pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, 0);
            if (!pMedDest->hBitmap)
                return E_OUTOFMEMORY;
            break;

        case TYMED_MFPICT:
            pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, 0);
            if (!pMedDest->hMetaFilePict)
                return E_OUTOFMEMORY;
            break;

        case TYMED_ENHMF:
            pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, 0);
            if (!pMedDest->hEnhMetaFile)
                return E_OUTOFMEMORY;
            break;

        case TYMED_FILE:
            pMedDest->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, 0);
            if (!pMedDest->lpszFileName)
                return E_OUTOFMEMORY;
            break;

        case TYMED_ISTREAM:
            pMedDest->pstm = pMedSrc->pstm;
            pMedSrc->pstm->AddRef();
            break;

        case TYMED_ISTORAGE:
            pMedDest->pstg = pMedSrc->pstg;
            pMedSrc->pstg->AddRef();
            break;

        default:
            return E_FAIL;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc) {
    if (ppEnumFormatEtc == NULL)
        return E_POINTER;

    *ppEnumFormatEtc = NULL;

    if (dwDirection == DATADIR_GET) {
        // Create an enumerator for our formats
        WF_IEnumFORMATETC* pEnum = WF_IEnumFORMATETC_new(m_nNumFormats, m_pFormatEtc);
        if (pEnum == NULL)
            return E_OUTOFMEMORY;

        *ppEnumFormatEtc = pEnum;
        return S_OK;
    }

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WF_IDataObject::DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection) {
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::DUnadvise(DWORD dwConnection) {
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT STDMETHODCALLTYPE WF_IDataObject::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise) {
    return OLE_E_ADVISENOTSUPPORTED;
}

// Find the index of a format in our format array
LONG WF_IDataObject::IndexOfFormat(FORMATETC* pFormatEtc) {
    for (LONG i = 0; i < m_nNumFormats; i++) {
        if ((m_pFormatEtc[i].cfFormat == pFormatEtc->cfFormat) && (m_pFormatEtc[i].dwAspect == pFormatEtc->dwAspect) &&
            (m_pFormatEtc[i].tymed & pFormatEtc->tymed)) {
            return i;
        }
    }
    return -1;
}

// Add a format to the data object
HRESULT WF_IDataObject::AddFormatEtc(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease) {
    // Validate parameters
    if (pFormatEtc == NULL || pMedium == NULL)
        return E_INVALIDARG;

    // Resize the format array
    FORMATETC* pNewFormatEtc = new FORMATETC[m_nNumFormats + 1];
    STGMEDIUM* pNewStgMedium = new STGMEDIUM[m_nNumFormats + 1];

    if (pNewFormatEtc == NULL || pNewStgMedium == NULL) {
        delete[] pNewFormatEtc;
        delete[] pNewStgMedium;
        return E_OUTOFMEMORY;
    }

    // Copy existing formats
    for (LONG i = 0; i < m_nNumFormats; i++) {
        pNewFormatEtc[i] = m_pFormatEtc[i];
        pNewStgMedium[i] = m_pStgMedium[i];
    }

    // Add the new format
    pNewFormatEtc[m_nNumFormats] = *pFormatEtc;

    if (fRelease) {
        // Just take ownership of the medium
        pNewStgMedium[m_nNumFormats] = *pMedium;
    } else {
        // Make a copy of the medium
        HRESULT hr = CopyMedium(&pNewStgMedium[m_nNumFormats], pMedium, pFormatEtc);
        if (FAILED(hr)) {
            delete[] pNewFormatEtc;
            delete[] pNewStgMedium;
            return hr;
        }
    }

    // Replace the old format arrays with the new ones
    if (m_pFormatEtc) {
        delete[] m_pFormatEtc;
        delete[] m_pStgMedium;
    }

    m_pFormatEtc = pNewFormatEtc;
    m_pStgMedium = pNewStgMedium;
    m_nNumFormats++;

    return S_OK;
}

// Create a data object with CF_HDROP format for the selected files
HRESULT CreateDataObject(LPTSTR pszFiles, POINT pt, WF_IDataObject** ppDataObject) {
    if (ppDataObject == NULL || pszFiles == NULL)
        return E_INVALIDARG;

    // Create a new data object
    WF_IDataObject* pDataObject = new WF_IDataObject();
    if (pDataObject == NULL)
        return E_OUTOFMEMORY;

    // Create HDROP from the file list
    HDROP hDrop = CreateDropFiles(pt, FALSE, pszFiles);
    if (hDrop == NULL) {
        pDataObject->Release();
        return E_OUTOFMEMORY;
    }

    // Set up the CF_HDROP format structure
    FORMATETC fmtetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgmed = { TYMED_HGLOBAL };
    stgmed.hGlobal = hDrop;
    stgmed.pUnkForRelease = NULL;

    // Add the format to the data object
    HRESULT hr = pDataObject->AddFormatEtc(&fmtetc, &stgmed, TRUE);
    if (FAILED(hr)) {
        pDataObject->Release();
        GlobalFree(hDrop);
        return hr;
    }

    // Add FileGroupDescriptorW format - some applications need this
    // First, parse the file list to get individual file paths
    WCHAR szTemp[MAX_PATH];
    WCHAR* pszNext = pszFiles;
    FILEGROUPDESCRIPTORW* pFileGroupDesc = NULL;
    size_t maxFiles = 64;  // Reasonable limit
    size_t fileCount = 0;

    // Allocate memory for the file descriptors
    pFileGroupDesc = (FILEGROUPDESCRIPTORW*)GlobalAlloc(
        GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(FILEGROUPDESCRIPTORW) + (maxFiles - 1) * sizeof(FILEDESCRIPTORW));

    if (pFileGroupDesc) {
        pFileGroupDesc->cItems = 0;

        // Parse file list
        while (pszNext && *pszNext && fileCount < maxFiles) {
            // Skip leading spaces
            while (*pszNext == L' ')
                pszNext++;

            // Check for quoted file name
            BOOL bQuoted = (*pszNext == L'"');
            if (bQuoted)
                pszNext++;

            // Copy file path
            size_t i = 0;
            while (*pszNext && i < MAX_PATH - 1 && ((!bQuoted && *pszNext != L' ') || (bQuoted && *pszNext != L'"'))) {
                szTemp[i++] = *pszNext++;
            }
            szTemp[i] = L'\0';

            // Skip closing quote and spaces
            if (bQuoted && *pszNext == L'"')
                pszNext++;
            while (*pszNext == L' ')
                pszNext++;

            if (i > 0) {
                // Remove quotes from file name
                WCHAR fileName[MAX_PATH];
                WCHAR* pFileName = PathFindFileName(szTemp);
                StringCchCopy(fileName, MAX_PATH, pFileName);

                // Fill file descriptor
                FILEDESCRIPTORW* pFileDesc = &pFileGroupDesc->fgd[fileCount];
                pFileDesc->dwFlags = FD_ATTRIBUTES | FD_FILESIZE | FD_PROGRESSUI;

                // Get the file attributes
                DWORD dwAttrs = GetFileAttributes(szTemp);
                if (dwAttrs != INVALID_FILE_ATTRIBUTES) {
                    pFileDesc->dwFileAttributes = dwAttrs;
                } else {
                    pFileDesc->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                }

                // Get the file size
                WIN32_FILE_ATTRIBUTE_DATA fad;
                if (GetFileAttributesEx(szTemp, GetFileExInfoStandard, &fad)) {
                    pFileDesc->nFileSizeLow = fad.nFileSizeLow;
                    pFileDesc->nFileSizeHigh = fad.nFileSizeHigh;
                }

                // Copy the file name
                StringCchCopy(pFileDesc->cFileName, MAX_PATH, fileName);

                fileCount++;
                pFileGroupDesc->cItems++;
            }
        }

        if (fileCount > 0) {
            // Lock the descriptor so we can get a pointer to the memory
            FILEGROUPDESCRIPTORW* pLockedDesc = (FILEGROUPDESCRIPTORW*)GlobalLock(pFileGroupDesc);
            GlobalUnlock(pFileGroupDesc);

            // Add FileGroupDescriptorW format
            CLIPFORMAT cf_filedescriptorw = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
            FORMATETC fmtDesc = { cf_filedescriptorw, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stgDesc = { TYMED_HGLOBAL };
            stgDesc.hGlobal = pFileGroupDesc;
            stgDesc.pUnkForRelease = NULL;

            hr = pDataObject->AddFormatEtc(&fmtDesc, &stgDesc, TRUE);
            if (FAILED(hr)) {
                GlobalFree(pFileGroupDesc);
            }
        } else {
            GlobalFree(pFileGroupDesc);
        }
    }

    // Add preferred drop effect (COPY or MOVE)
    CLIPFORMAT cf_preferreddropeffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    FORMATETC fmtetcEffect = { cf_preferreddropeffect, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    DWORD dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DWORD));
    if (hGlobal) {
        DWORD* pdwEffect = (DWORD*)GlobalLock(hGlobal);
        if (pdwEffect) {
            *pdwEffect = dwEffect;
            GlobalUnlock(hGlobal);

            STGMEDIUM stgmedEffect = { TYMED_HGLOBAL };
            stgmedEffect.hGlobal = hGlobal;
            stgmedEffect.pUnkForRelease = NULL;

            hr = pDataObject->AddFormatEtc(&fmtetcEffect, &stgmedEffect, TRUE);
            if (FAILED(hr)) {
                GlobalFree(hGlobal);
            }
        } else {
            GlobalFree(hGlobal);
        }
    }

    // Add CF_TEXT format with file names
    LPTSTR pszText = NULL;
    size_t cchText = 0;
    pszNext = pszFiles;

    // First calculate the required size
    while (pszNext && *pszNext) {
        // Skip leading spaces
        while (*pszNext == L' ')
            pszNext++;

        // Check for quoted file name
        BOOL bQuoted = (*pszNext == L'"');
        if (bQuoted)
            pszNext++;

        // Copy file path
        size_t i = 0;
        while (*pszNext && ((!bQuoted && *pszNext != L' ') || (bQuoted && *pszNext != L'"'))) {
            i++;
            pszNext++;
        }

        if (i > 0) {
            cchText += i + 2;  // path + CRLF
        }

        // Skip closing quote and spaces
        if (bQuoted && *pszNext == L'"')
            pszNext++;
        while (*pszNext == L' ')
            pszNext++;
    }

    if (cchText > 0) {
        // Allocate memory for the text
        HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (cchText + 1) * sizeof(WCHAR));
        if (hText) {
            LPTSTR pszDest = (LPTSTR)GlobalLock(hText);
            if (pszDest) {
                pszNext = pszFiles;
                while (pszNext && *pszNext) {
                    // Skip leading spaces
                    while (*pszNext == L' ')
                        pszNext++;

                    // Check for quoted file name
                    BOOL bQuoted = (*pszNext == L'"');
                    if (bQuoted)
                        pszNext++;

                    // Copy file path
                    size_t i = 0;
                    while (*pszNext && i < MAX_PATH - 1 &&
                           ((!bQuoted && *pszNext != L' ') || (bQuoted && *pszNext != L'"'))) {
                        szTemp[i++] = *pszNext++;
                    }
                    szTemp[i] = L'\0';

                    // Skip closing quote and spaces
                    if (bQuoted && *pszNext == L'"')
                        pszNext++;
                    while (*pszNext == L' ')
                        pszNext++;

                    if (i > 0) {
                        StringCchCat(pszDest, cchText + 1, szTemp);
                        StringCchCat(pszDest, cchText + 1, L"\r\n");
                    }
                }

                GlobalUnlock(hText);

                // Add CF_UNICODETEXT format
                FORMATETC fmtText = { CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                STGMEDIUM stgText = { TYMED_HGLOBAL };
                stgText.hGlobal = hText;
                stgText.pUnkForRelease = NULL;

                hr = pDataObject->AddFormatEtc(&fmtText, &stgText, TRUE);
                if (FAILED(hr)) {
                    GlobalFree(hText);
                }
            } else {
                GlobalFree(hText);
            }
        }
    }

    *ppDataObject = pDataObject;
    return S_OK;
}

// Perform a drag and drop operation with the selected files
HRESULT WFDoDragDrop(HWND hwnd, LPTSTR pszFiles, POINT pt, DWORD* pdwEffect) {
    if (!pszFiles || !*pszFiles || !pdwEffect)
        return E_INVALIDARG;

    // Initialize output parameter
    *pdwEffect = DROPEFFECT_NONE;

    HRESULT hr;

    // Create HDROP for the files
    HDROP hDrop = CreateDropFiles(pt, FALSE, pszFiles);
    if (hDrop == NULL)
        return E_OUTOFMEMORY;

    // Create an HGLOBAL with CF_HDROP format
    FORMATETC fmtHDrop = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgHDrop = { TYMED_HGLOBAL };
    stgHDrop.hGlobal = hDrop;

    // Create standard OLE IDropSource
    IDropSource* pDropSource = NULL;
    hr = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropSource, (void**)&pDropSource);

    if (FAILED(hr) || !pDropSource) {
        // Fall back to a basic implementation
        hr = CreateDropSource((WF_IDropSource**)&pDropSource);
        if (FAILED(hr) || !pDropSource) {
            GlobalFree(hDrop);
            return hr;
        }
    }

    // Create a data object with the OLE API
    IDataObject* pDataObject = NULL;

    // Try to use the shell data object if available
    BOOL bShellDataObjectCreated = FALSE;

    // Check if Shell32.dll is available and has SHCreateDataObject
    HINSTANCE hShell32 = LoadLibrary(L"shell32.dll");
    if (hShell32) {
        typedef HRESULT(WINAPI * SHCreateDataObjectFunc)(
            PCIDLIST_ABSOLUTE pidlFolder, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, IDataObject * pdtInner, REFIID riid,
            void** ppv);

        SHCreateDataObjectFunc pSHCreateDataObject =
            (SHCreateDataObjectFunc)GetProcAddress(hShell32, "SHCreateDataObject");

        if (pSHCreateDataObject) {
            // We don't have PIDLs, so create an empty shell data object and set the HDROP
            hr = pSHCreateDataObject(NULL, 0, NULL, NULL, IID_IDataObject, (void**)&pDataObject);
            if (SUCCEEDED(hr) && pDataObject) {
                // Set the HDROP format on the data object
                hr = pDataObject->SetData(&fmtHDrop, &stgHDrop, TRUE);
                if (FAILED(hr)) {
                    pDataObject->Release();
                    pDataObject = NULL;
                } else {
                    bShellDataObjectCreated = TRUE;
                }
            }
        }

        FreeLibrary(hShell32);
    }

    // If shell data object failed, create our own
    if (!pDataObject) {
        WF_IDataObject* pCustomDataObj = NULL;
        hr = CreateDataObject(pszFiles, pt, &pCustomDataObj);
        if (FAILED(hr) || !pCustomDataObj) {
            pDropSource->Release();
            GlobalFree(hDrop);
            return hr;
        }

        // Get the IDataObject interface
        hr = pCustomDataObj->QueryInterface(IID_IDataObject, (void**)&pDataObject);
        pCustomDataObj->Release();  // Release the custom object

        if (FAILED(hr) || !pDataObject) {
            pDropSource->Release();
            GlobalFree(hDrop);
            return E_NOINTERFACE;
        }
    }

    // Perform the drag and drop operation
    DWORD dwAllowedEffects = DROPEFFECT_COPY | DROPEFFECT_MOVE;

    try {
        hr = ::DoDragDrop(pDataObject, pDropSource, dwAllowedEffects, pdwEffect);
    } catch (...) {
        hr = E_UNEXPECTED;
        *pdwEffect = DROPEFFECT_NONE;
    }

    // Clean up
    pDataObject->Release();
    pDropSource->Release();

    return hr;
}

// Implementation of FormatEtc enumerator
WF_IEnumFORMATETC::WF_IEnumFORMATETC() {
    m_lRefCount = 1;
    m_nIndex = 0;
    m_nNumFormats = 0;
    m_pFormatEtc = NULL;
}

WF_IEnumFORMATETC::~WF_IEnumFORMATETC() {
    if (m_pFormatEtc)
        delete[] m_pFormatEtc;
}

ULONG STDMETHODCALLTYPE WF_IEnumFORMATETC::AddRef() {
    return InterlockedIncrement(&m_lRefCount);
}

ULONG STDMETHODCALLTYPE WF_IEnumFORMATETC::Release() {
    LONG count = InterlockedDecrement(&m_lRefCount);

    if (count == 0) {
        delete this;
        return 0;
    }

    return count;
}

HRESULT STDMETHODCALLTYPE WF_IEnumFORMATETC::QueryInterface(REFIID riid, void** ppvObject) {
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumFORMATETC)) {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE WF_IEnumFORMATETC::Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) {
    ULONG celtFetched = 0;

    if (pceltFetched != NULL)
        *pceltFetched = 0;

    if (rgelt == NULL)
        return E_INVALIDARG;

    while (celtFetched < celt && m_nIndex < m_nNumFormats) {
        rgelt[celtFetched] = m_pFormatEtc[m_nIndex];
        celtFetched++;
        m_nIndex++;
    }

    if (pceltFetched != NULL)
        *pceltFetched = celtFetched;

    return (celtFetched == celt) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE WF_IEnumFORMATETC::Skip(ULONG celt) {
    m_nIndex = min(m_nIndex + celt, m_nNumFormats);
    return (m_nIndex >= m_nNumFormats) ? S_FALSE : S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IEnumFORMATETC::Reset() {
    m_nIndex = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WF_IEnumFORMATETC::Clone(IEnumFORMATETC** ppEnum) {
    if (ppEnum == NULL)
        return E_INVALIDARG;

    WF_IEnumFORMATETC* pEnum = WF_IEnumFORMATETC_new(m_nNumFormats, m_pFormatEtc);
    if (pEnum == NULL)
        return E_OUTOFMEMORY;

    pEnum->m_nIndex = m_nIndex;
    *ppEnum = pEnum;

    return S_OK;
}

// Create a new enumerator with the given formats
WF_IEnumFORMATETC* WF_IEnumFORMATETC_new(UINT nNumFormats, FORMATETC* pFormatEtc) {
    WF_IEnumFORMATETC* pEnum = new WF_IEnumFORMATETC();
    if (pEnum == NULL)
        return NULL;

    pEnum->m_nNumFormats = nNumFormats;

    if (nNumFormats > 0) {
        pEnum->m_pFormatEtc = new FORMATETC[nNumFormats];
        if (pEnum->m_pFormatEtc == NULL) {
            delete pEnum;
            return NULL;
        }

        for (UINT i = 0; i < nNumFormats; i++)
            pEnum->m_pFormatEtc[i] = pFormatEtc[i];
    }

    return pEnum;
}