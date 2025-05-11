/********************************************************************

   dbg.c

   Debugging utilities

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#include "winfile.h"
#include "dbg.h"

#if DBG

WCHAR szAsrtFmt[] = L"Assertion Failure in %s at %d\n";
WCHAR szFmt[300];
unsigned long TraceFlags = BF_START
    //     | BF_PROCTRACE
    //     | BF_PARMTRACE
    //     | BF_MSGTRACE
    //     | BF_DEFMSGTRACE
    ;  // set these to print on TRACEs

unsigned long BreakFlags = 0
    // | BF_START
    ;  // set these to break on TRACEs

void DbgAssert(LPCWSTR file, int line) {
    wsprintf(szFmt, szAsrtFmt, file, line);
    OutputDebugString(szFmt);
    //    DbgPrint(szAsrtFmt, file, line);
    DebugBreak();
}

void DbgTrace(DWORD tf, LPWSTR lpstr) {
    if (tf & TraceFlags) {
        wsprintf(szFmt, L"%s\n", lpstr);
        OutputDebugString(szFmt);
        //        DbgPrint("%s\n", lpstr);
        if (tf & BreakFlags) {
            DebugBreak();
        }
    }
}

void DbgBreak(DWORD bf, LPCWSTR file, int line) {
    if (bf & BreakFlags) {
        wsprintf(szFmt, L"BREAK at %s:%d\n", file, line);
        OutputDebugString(szFmt);
        //        DbgPrint("BREAK at %s:%d\n", file, line);
        DebugBreak();
    }
}

void DbgPrint1(DWORD tf, LPCWSTR fmt, LPWSTR p1) {
    if (tf & TraceFlags) {
        wsprintf(szFmt, fmt, p1);
        OutputDebugString(szFmt);
        //        DbgPrint("%d",GetTickCount());
        OutputDebugString(L"\n");
        //        DbgPrint(fmt, p1);
        //        DbgPrint("\n");
    }
    if (tf & BreakFlags) {
        DebugBreak();
    }
}

void DbgEnter(LPCWSTR funName) {
    DbgPrint1(BF_PROCTRACE, L"> %s ", (LPWSTR)funName);
}

void DbgLeave(LPCWSTR funName) {
    DbgPrint1(BF_PROCTRACE, L" <%s ", (LPWSTR)funName);
}

void DbgTraceMessage(LPWSTR funName, LPWSTR msgName) {
    if (BF_MSGTRACE & TraceFlags) {
        wsprintf(szFmt, L"MSG: %s - %s\n", funName, msgName);
        OutputDebugString(szFmt);
        //        DbgPrint("MSG: %s - %s\n", funName, msgName);
    }
    if (BF_MSGTRACE & BreakFlags) {
        DebugBreak();
    }
}

void DbgTraceDefMessage(LPWSTR funName, WORD msgId) {
    if (BF_DEFMSGTRACE & TraceFlags) {
        wsprintf(szFmt, L"MSG: %s - default(0x%x)\n", funName, msgId);
        OutputDebugString(szFmt);
        //        DbgPrint("MSG: %s - default(0x%x)\n", funName, msgId);
    }
    if (BF_DEFMSGTRACE & BreakFlags) {
        DebugBreak();
    }
}

#endif  // DBG
