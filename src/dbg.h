/********************************************************************

   dbg.h

   Debugging utilities header

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#if DBG

extern WCHAR szAsrtFmt[];
extern unsigned long BreakFlags;
extern unsigned long TraceFlags;

DWORD DbgPrint(LPWSTR Format, ...);
void DbgAssert(LPCWSTR file, int line);
void DbgTrace(DWORD tf, LPWSTR lpstr);
void DbgBreak(DWORD bf, LPCWSTR file, int line);
void DbgPrint1(DWORD tf, LPCWSTR fmt, LPWSTR p1);
void DbgEnter(LPCWSTR funName);
void DbgLeave(LPCWSTR funName);
void DbgTraceMessage(LPWSTR funName, LPWSTR msgName);
void DbgTraceDefMessage(LPWSTR funName, WORD msgId);

// BreakFlags flags

#define BF_SPECIAL 0x00800000

#define BF_WM_CREATE 0x02000000
#define BF_DEFMSGTRACE 0x04000000
#define BF_MSGTRACE 0x08000000

#define BF_PARMTRACE 0x20000000
#define BF_PROCTRACE 0x40000000
#define BF_START 0x80000000

#define ASSERT(fOk) \
    if (!(fOk))     \
    DbgAssert(TEXT(__FILE__), __LINE__)
#define FBREAK(bf) DbgBreak(bf, TEXT(__FILE__), __LINE__)
#define TRACE(tf, lpstr) DbgTrace(tf, lpstr)
#define PRINT(tf, fmt, p1) DbgPrint1(tf, fmt, (LPWSTR)(p1))
#define MSG(funName, msgName) DbgTraceMessage(funName, msgName)
#define DEFMSG(funName, wMsgId) DbgTraceDefMessage(funName, wMsgId)

#define ENTER(funName) DbgEnter(funName)
#define LEAVE(funName) DbgLeave(funName)

#else  // !DBG

#define ASSERT(fOk)
#define FBREAK(bf)
#define TRACE(tf, lpstr)
#define PRINT(tf, fmt, p1)
#define MSG(funName, msgName)
#define DEFMSG(funName, wMsgId)
#define ENTER(funName)
#define LEAVE(funName)

#endif  // DBG
