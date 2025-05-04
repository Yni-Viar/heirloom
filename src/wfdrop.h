/**************************************************************************

   wfdrop.h

   Include for WINFILE program

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

**************************************************************************/

#ifndef WFDROP_INC
#define WFDROP_INC
#include <ole2.h>

// data object:

class WF_IDataObject : public IDataObject {
public:
    int ref_count;
    FORMATETC *m_pFormatEtc;
    STGMEDIUM *m_pStgMedium;
    LONG       m_nNumFormats;
    LONG m_lRefCount;
};

class WF_IEnumFORMATETC : public IEnumFORMATETC {
public:
    int ref_count;
    int ix;
    LONG        m_lRefCount;        // Reference count for this COM interface
    ULONG       m_nIndex;           // current enumerator index
    ULONG       m_nNumFormats;      // number of FORMATETC members
    FORMATETC * m_pFormatEtc;
};

class WF_IDropSource : public IDropSource {
public:
    LONG        m_lRefCount;
};

class WF_IDropTarget : public IDropTarget {
public:
    WF_IDropTarget(HWND hwnd);
    virtual ~WF_IDropTarget() = default;

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;

    // IDropTarget methods
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    virtual HRESULT STDMETHODCALLTYPE DragLeave() override;
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

    LONG  m_lRefCount;
    HWND  m_hWnd;
    BOOL  m_fAllowDrop;
    DWORD m_iItemSelected;
    IDataObject *m_pDataObject;
};

typedef WF_IDataObject* LPWBDATAOBJECT;
typedef WF_IEnumFORMATETC* LPWBFORMATETC;

WF_IEnumFORMATETC *WF_IEnumFORMATETC_new (UINT, FORMATETC *);

void RegisterDropWindow(HWND hwnd, WF_IDropTarget **ppDropTarget);
void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget);

LPWSTR QuotedDropList(IDataObject *pDataObj);
LPWSTR QuotedContentList(IDataObject *pDataObj);
HDROP CreateDropFiles(POINT pt, BOOL fNC, LPTSTR pszFiles);

#endif
