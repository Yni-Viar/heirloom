/********************************************************************

   wfmem.h

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#pragma once

typedef struct _XDTAHEAD* LPXDTAHEAD;
typedef struct _XDTA* LPXDTA;
typedef struct _XDTALINK* LPXDTALINK;

typedef struct _XDTALINK {
    LPXDTALINK next;
    DWORD dwSize;
    DWORD dwNextFree;
    DWORD dwPad; /* quad word align for Alpha */
} XDTALINK;

#define LPXDTA_STATUS_READING 0x1  // Reading by ReadDirLevel
#define LPXDTA_STATUS_CLOSE 0x2    // ReadDirLevel must free

typedef struct _XDTAHEAD {
    DWORD dwEntries;
    DWORD dwTotalCount;
    LARGE_INTEGER qTotalSize;
    LPXDTA* alpxdtaSorted;

    DWORD dwAlternateFileNameExtent;
    DWORD fdwStatus;

    DWORD dwPad; /* quad word align for Alpha */

} XDTAHEAD;

typedef struct _XDTA {
    DWORD dwSize;
    DWORD dwAttrs;
    FILETIME ftLastWriteTime;
    LARGE_INTEGER qFileSize;
    UINT cchFileNameOffset;

    BYTE byBitmap;
    BYTE byType;

    PDOCBUCKET pDocB;

    WCHAR cFileNames[1];
} XDTA;

#define LINKHEADSIZE (sizeof(XDTALINK) + sizeof(XDTAHEAD))

LPXDTALINK MemNew();
void MemDelete(LPXDTALINK lpStart);

LPXDTALINK MemClone(LPXDTALINK lpStart);
LPXDTA MemAdd(LPXDTALINK* plpLast, UINT cchFileName, UINT cchAlternateFileName);
LPXDTA MemNext(LPXDTALINK* plpLink, LPXDTA lpxdta);

#define MemFirst(lpStart) ((LPXDTA)(((PBYTE)lpStart) + LINKHEADSIZE))
#define MemGetAlternateFileName(lpxdta) (&lpxdta->cFileNames[lpxdta->cchFileNameOffset])
#define MemGetFileName(lpxdta) (lpxdta->cFileNames)
#define MemLinkToHead(link) ((LPXDTAHEAD)((PBYTE)link + sizeof(XDTALINK)))

#define pdtaNext(pdta) ((LPXDTA)((PBYTE)(pdta) + ((LPXDTA)pdta)->dwSize))
