/********************************************************************

   wfdpi.h

   DPI Awareness Support for Windows File Manager

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#ifndef _WFDPI_H
#define _WFDPI_H

// DPI awareness functions
extern UINT g_dpi;
extern FLOAT g_scale;

// Initialize DPI awareness for the application
void InitDPIAwareness(void);

// Scale a value by the DPI scale factor
int ScaleByDpi(int value);

// Scale a value by a specific DPI value
int ScaleValueForDpi(int value, UINT dpi);

// Get the DPI scale factor (1.0 for standard 96 DPI, 1.25 for 120 DPI, etc.)
FLOAT GetDpiScaleFactor(UINT dpi);

// Handle window DPI change event
void HandleDpiChange(HWND hwnd, WPARAM wParam, LPARAM lParam);

// Scale system metric value by current DPI
int ScaledSystemMetric(int nIndex);

#endif  // _WFDPI_H