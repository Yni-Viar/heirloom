#include "winfile.h"
#include "wfdpi.h"

// Globals for DPI handling
UINT g_dpi = 96;
FLOAT g_scale = 1.0f;

// Default reference DPI value
#define DEFAULT_DPI 96

// Win10 SDK declarations if not available
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

// GetDpiForMonitor function type definition for dynamically loading from Shcore.dll
typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR hmonitor, INT dpiType, UINT *dpiX, UINT *dpiY);
typedef HRESULT (WINAPI *GetDpiForWindowFunc)(HWND hwnd);

extern void ResizeControls(void);

// Initialize DPI awareness
VOID InitDPIAwareness(VOID)
{
    // Load Shcore.dll which contains the DPI functions on Windows 8.1+
    HMODULE hShcore = LoadLibrary(TEXT("Shcore.dll"));
    
    if (hShcore) {
        // Initialize default DPI setting (96) in case we can't get the actual value
        HDC hdc = GetDC(NULL);
        if (hdc) {
            g_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
        
        FreeLibrary(hShcore);
    } else {
        // On pre-8.1 Windows versions, use the primary screen DC to get DPI
        HDC hdc = GetDC(NULL);
        if (hdc) {
            g_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }
    
    g_scale = GetDpiScaleFactor(g_dpi);
}

// Get the scale factor for a given DPI value
FLOAT GetDpiScaleFactor(UINT dpi)
{
    return (FLOAT)dpi / (FLOAT)DEFAULT_DPI;
}

// Scale a value by the current global DPI factor
INT ScaleByDpi(INT value)
{
    return (INT)((FLOAT)value * g_scale + 0.5f);
}

// Scale a value by a specific DPI value
INT ScaleValueForDpi(INT value, UINT dpi)
{
    FLOAT scale = GetDpiScaleFactor(dpi);
    return (INT)((FLOAT)value * scale + 0.5f);
}

// Handle WM_DPICHANGED message
VOID HandleDpiChange(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // Get the new DPI value
    UINT newDpi = HIWORD(wParam);
    FLOAT newScale = GetDpiScaleFactor(newDpi);
    
    // Store the new DPI and scale values
    g_dpi = newDpi;
    g_scale = newScale;
    
    // Get the suggested window size and position
    RECT* prcNewWindow = (RECT*)lParam;
    
    // Resize the window
    SetWindowPos(hwnd, 
                 NULL, 
                 prcNewWindow->left, 
                 prcNewWindow->top, 
                 prcNewWindow->right - prcNewWindow->left, 
                 prcNewWindow->bottom - prcNewWindow->top, 
                 SWP_NOZORDER | SWP_NOACTIVATE);
    
    // Refresh UI elements to reflect new DPI settings
    InvalidateRect(hwnd, NULL, TRUE);
    
    // Resize controls
    if (hwndFrame == hwnd) {
        ResizeControls();
    }
}

// Scale system metric based on current DPI
INT ScaledSystemMetric(INT nIndex)
{
    INT value = GetSystemMetrics(nIndex);
    return ScaleByDpi(value);
}

// Load an icon resource with proper scaling for the current DPI
HICON LoadIconForCurrentDPI(HINSTANCE hInstance, LPCWSTR lpIconName)
{
    // Calculate appropriate icon size based on current DPI
    // Standard icon sizes are 16x16, 32x32, 48x48
    int dpi = (int)g_dpi;
    int iconSize;
    
    // Choose appropriate icon size based on DPI
    if (dpi <= 96) {
        iconSize = 16; // Standard size for 96 DPI
    } else if (dpi <= 144) {
        iconSize = 32; // Medium size for 120-144 DPI
    } else {
        iconSize = 48; // Large size for 192+ DPI
    }
    
    // Load the icon with proper DPI scaling
    HICON hIcon = (HICON)LoadImage(
        hInstance,
        lpIconName,
        IMAGE_ICON,
        iconSize,
        iconSize,
        LR_DEFAULTCOLOR | LR_DEFAULTSIZE
    );
    
    // If LoadImage fails, try without the size specification
    if (!hIcon) {
        hIcon = (HICON)LoadImage(
            hInstance,
            lpIconName,
            IMAGE_ICON,
            0,
            0,
            LR_DEFAULTCOLOR | LR_DEFAULTSIZE
        );
    }
    
    // If that still fails, fall back to LoadIcon
    if (!hIcon) {
        hIcon = LoadIcon(hInstance, lpIconName);
    }
    
    return hIcon;
}

void InitDpiScaling(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    if (hdc) {
        int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        
        // Use the larger of the two DPI values
        int dpi = max(dpiX, dpiY);
        
        // Calculate scaling factor (96 is the default DPI)
        g_scale = (float)dpi / 96.0f;
        
        ReleaseDC(hwnd, hdc);
    }
} 