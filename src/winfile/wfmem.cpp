/********************************************************************

   wfmem.c

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#define BLOCK_SIZE_GRANULARITY 1024     // must be larger than XDTA
#define ALIGNBLOCK(x) (((x) + 7) & ~7)  // quad word align for Alpha

#include <windows.h>
#include "wfdocb.h"
#include "wfmem.h"

LPXDTALINK
MemNew() {
    LPXDTALINK lpStart;
    LPXDTAHEAD lpHead;

    lpStart = (LPXDTALINK)LocalAlloc(LMEM_FIXED, BLOCK_SIZE_GRANULARITY);

    if (!lpStart)
        return NULL;

    //
    // Initialize the link structure
    //
    lpStart->next = NULL;
    lpStart->dwSize = BLOCK_SIZE_GRANULARITY;
    lpStart->dwNextFree = LINKHEADSIZE;

    //
    // Create the XDTAHEAD block and initialize
    //
    lpHead = MemLinkToHead(lpStart);

    //
    // Initialize winfile specific data
    //
    lpHead->dwEntries = 0;
    lpHead->dwTotalCount = 0;
    lpHead->qTotalSize.HighPart = 0;
    lpHead->qTotalSize.LowPart = 0;
    lpHead->alpxdtaSorted = NULL;
    lpHead->fdwStatus = 0;

    //
    // lpHead->dwAlternateFileNameExtent = 0;
    // lpHead->iError = 0;
    //

    return lpStart;
}

void MemDelete(LPXDTALINK lpStart) {
    LPXDTALINK lpLink;
    LPXDTAHEAD lpHead;
    LPXDTA* plpxdta;

    if (!lpStart)
        return;

    lpHead = MemLinkToHead(lpStart);
    plpxdta = lpHead->alpxdtaSorted;

    if (plpxdta)
        LocalFree(plpxdta);

    while (lpStart) {
        lpLink = lpStart->next;
        LocalFree(lpStart);

        lpStart = lpLink;
    }

    return;
}

/////////////////////////////////////////////////////////////////////
//
// Name:     MemAdd
//
// Synopsis: Allocates space (if nec) to the memory structure
//
// lpLast               _must_ be the last link of the mem struct
// cchFileName          char count of name (not including NULL)
// cchAlternateFileName char count of altname (not including NULL)
//
// Return:
//
//
// Assumes:
//
//    lpLast is really the last item block
//
// Effects:
//
//    Sets up internal pointers
//
// Notes:
//
// Invariants:
//
//    lpLast*->dwNextFree maintained
//    lpLast*->next linked list maintained
//
/////////////////////////////////////////////////////////////////////

LPXDTA
MemAdd(LPXDTALINK* plpLast, UINT cchFileName, UINT cchAlternateFileName) {
    LPXDTA lpxdta;
    UINT cbSpace;
    LPXDTALINK lpLast = *plpLast;
    DWORD dwNewSize;

    cbSpace = ALIGNBLOCK((cchFileName + cchAlternateFileName + 2) * sizeof(WCHAR) + sizeof(XDTA));

    if (cbSpace + lpLast->dwNextFree > lpLast->dwSize) {
        //
        // Must allocate a new block
        //
        dwNewSize = lpLast->dwSize * 2;

        if (dwNewSize < lpLast->dwSize)
            dwNewSize = lpLast->dwSize;

        lpLast->next = (LPXDTALINK)LocalAlloc(LMEM_FIXED, dwNewSize);

        if (!lpLast->next)
            return NULL;

        lpLast = *plpLast = lpLast->next;

        lpLast->dwSize = dwNewSize;

        lpLast->next = NULL;
        lpLast->dwNextFree = ALIGNBLOCK(sizeof(XDTALINK));
    }

    //
    // We have enough space in this link now
    // Update everything
    //

    lpxdta = (LPXDTA)((PBYTE)lpLast + lpLast->dwNextFree);

    lpLast->dwNextFree += cbSpace;
    lpxdta->dwSize = cbSpace;
    lpxdta->cchFileNameOffset = cchFileName + 1;

    return lpxdta;
}

/////////////////////////////////////////////////////////////////////
//
// Name:     MemClone
//
// Synopsis: returns a copy of the specified block
//
//
// Return:   Full Copy or NULL
//
// Assumes:  lpStart stable and safe
//
// Effects:
//
// Notes:
//
/////////////////////////////////////////////////////////////////////

LPXDTALINK
MemClone(LPXDTALINK lpStart) {
    LPXDTALINK lpStartCopy;
    LPXDTALINK lpPrev;
    LPXDTALINK lpLink;
    LPXDTALINK lpNext;
    SIZE_T dwSize;

    for (lpPrev = NULL, lpStartCopy = NULL; lpStart; lpStart = lpNext) {
        lpNext = lpStart->next;

        dwSize = LocalSize((HLOCAL)lpStart);

        lpLink = (LPXDTALINK)LocalAlloc(LMEM_FIXED, dwSize);
        if (!lpLink) {
            MemDelete(lpStartCopy);
            return NULL;
        }

        CopyMemory((PBYTE)lpLink, (PBYTE)lpStart, dwSize);

        if (!lpStartCopy) {
            lpStartCopy = lpLink;

            //
            // MUST set lpxdtaDst->head.alpxdtaSorted to NULL,
            // otherwise it will attempt to use the original one's space
            //
            MemLinkToHead(lpStartCopy)->alpxdtaSorted = NULL;
        }

        //
        // Setup link (must null out last one since we can free it at
        // any time if memory alloc fails).
        //
        lpLink->next = NULL;

        if (lpPrev) {
            lpPrev->next = lpLink;
        }
        lpPrev = lpLink;
    }

    return lpStartCopy;
}

LPXDTA
MemNext(LPXDTALINK* plpLink, LPXDTA lpxdta) {
    LPXDTALINK lpLinkCur = *plpLink;

    if ((PBYTE)lpxdta + lpxdta->dwSize - (PBYTE)lpLinkCur == (int)lpLinkCur->dwNextFree) {
        *plpLink = lpLinkCur->next;
        return (LPXDTA)(((PBYTE)*plpLink) + sizeof(XDTALINK));
    } else {
        return (LPXDTA)((PBYTE)lpxdta + lpxdta->dwSize);
    }
}
