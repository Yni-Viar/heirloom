#pragma once

// This header includes <windows.h> with the proper preprocessor defines for Windows 10+ support.
// Don't include anything else here.

#define WIN32_LEAN_AND_MEAN
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define NTDDI_VERSION NTDDI_WIN10
#include <SDKDDKVer.h>

#include <windows.h>
