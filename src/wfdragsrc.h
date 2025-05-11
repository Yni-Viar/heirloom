/**************************************************************************

   wfdragsrc.h

   Include for WINFILE program

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

**************************************************************************/

#pragma once

#include <ole2.h>

// Forward declarations from wfdrop.h
class WF_IDataObject;
class WF_IEnumFORMATETC;

// Drag source implementation
class WF_IDropSource : public IDropSource {
   public:
    WF_IDropSource();
    virtual ~WF_IDropSource() = default;

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    // IDropSource methods
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override;
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect) override;

    LONG m_lRefCount;
};

// Helper functions for drag operations
HRESULT CreateDropSource(WF_IDropSource** ppDropSource);
HRESULT CreateDataObject(LPWSTR pszFiles, POINT pt, WF_IDataObject** ppDataObject);
HRESULT WFDoDragDrop(HWND hwnd, LPWSTR pszFiles, POINT pt, DWORD* pdwEffect);
