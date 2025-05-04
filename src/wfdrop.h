/**************************************************************************

   wfdrop.h

   Include for WINFILE program

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

**************************************************************************/

#ifndef WFDROP_INC
#define WFDROP_INC
#include <ole2.h>

// Data object:

class WF_IDataObject : public IDataObject {
   public:
    WF_IDataObject();
    virtual ~WF_IDataObject();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    // IDataObject methods
    HRESULT STDMETHODCALLTYPE GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium) override;
    HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium) override;
    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* pFormatEtc) override;
    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC* pFormatEtcIn, FORMATETC* pFormatEtcOut) override;
    HRESULT STDMETHODCALLTYPE SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease) override;
    HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc) override;
    HRESULT STDMETHODCALLTYPE
    DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection) override;
    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection) override;
    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** ppEnumAdvise) override;

    // Helper methods
    HRESULT AddFormatEtc(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease);
    LONG IndexOfFormat(FORMATETC* pFormatEtc);
    HRESULT CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);

    LONG m_lRefCount;
    FORMATETC* m_pFormatEtc;
    STGMEDIUM* m_pStgMedium;
    LONG m_nNumFormats;
};

class WF_IEnumFORMATETC : public IEnumFORMATETC {
   public:
    WF_IEnumFORMATETC();
    virtual ~WF_IEnumFORMATETC();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    // IEnumFORMATETC methods
    HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) override;
    HRESULT STDMETHODCALLTYPE Skip(ULONG celt) override;
    HRESULT STDMETHODCALLTYPE Reset() override;
    HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC** ppEnum) override;

    LONG m_lRefCount;
    ULONG m_nIndex;
    ULONG m_nNumFormats;
    FORMATETC* m_pFormatEtc;
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
    virtual HRESULT STDMETHODCALLTYPE
    DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    virtual HRESULT STDMETHODCALLTYPE DragLeave() override;
    virtual HRESULT STDMETHODCALLTYPE
    Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

    LONG m_lRefCount;
    HWND m_hWnd;
    BOOL m_fAllowDrop;
    DWORD m_iItemSelected;
    IDataObject* m_pDataObject;
};

typedef WF_IDataObject* LPWBDATAOBJECT;
typedef WF_IEnumFORMATETC* LPWBFORMATETC;

WF_IEnumFORMATETC* WF_IEnumFORMATETC_new(UINT, FORMATETC*);

void RegisterDropWindow(HWND hwnd, WF_IDropTarget** ppDropTarget);
void UnregisterDropWindow(HWND hwnd, IDropTarget* pDropTarget);

LPWSTR QuotedDropList(IDataObject* pDataObj);
LPWSTR QuotedContentList(IDataObject* pDataObj);
HDROP CreateDropFiles(POINT pt, BOOL fNC, LPTSTR pszFiles);

#endif
