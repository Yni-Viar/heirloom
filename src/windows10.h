#pragma once

// This header includes <windows.h> with the proper preprocessor defines for Windows 10+ support.
// Don't include anything else here.

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#ifdef WINVER
#undef WINVER
#endif

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define NTDDI_VERSION NTDDI_WIN10
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
