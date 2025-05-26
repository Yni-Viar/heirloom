#include "progman/pch.h"
#include "progman/AboutDialog.h"
#include "progman/resource.h"
#include "libprogman/string_util.h"

namespace progman {

AboutDialog::AboutDialog(HWND parentWindow, HINSTANCE hInstance) : parentWindow_(parentWindow), hInstance_(hInstance) {}

void AboutDialog::showDialog() {
    DialogBoxParamW(hInstance_, MAKEINTRESOURCEW(IDD_ABOUT), parentWindow_, dialogProc, reinterpret_cast<LPARAM>(this));
}

INT_PTR CALLBACK AboutDialog::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    AboutDialog* pThis = nullptr;

    if (uMsg == WM_INITDIALOG) {
        pThis = reinterpret_cast<AboutDialog*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

        if (pThis) {
            pThis->initDialog(hwnd);
        }

        return TRUE;
    } else {
        pThis = reinterpret_cast<AboutDialog*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwnd, LOWORD(wParam));
                return TRUE;
            }
            break;
    }

    return FALSE;
}

void AboutDialog::initDialog(HWND hwnd) {
    // Set dialog icon
    HICON hIcon = LoadIconW(hInstance_, MAKEINTRESOURCEW(IDI_PROGMAN));
    if (hIcon) {
        SendMessageW(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
        SendMessageW(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
    }

    // Load and display version information
    loadVersionInfo(hwnd);
}

void AboutDialog::loadVersionInfo(HWND hwnd) {
    // Get the full path to the current executable
    WCHAR exePath[MAX_PATH] = { 0 };
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0) {
        // Failed to get module file name
        SetDlgItemTextW(hwnd, IDC_VERSION_LABEL, L"Version: Unknown");
        return;
    }

    // Get the size of the version info
    DWORD versionInfoSize = GetFileVersionInfoSizeW(exePath, nullptr);
    if (versionInfoSize == 0) {
        // Failed to get version info size
        SetDlgItemTextW(hwnd, IDC_VERSION_LABEL, L"Version: Not available");
        return;
    }

    // Allocate memory for the version info
    std::vector<BYTE> versionInfoBuffer(versionInfoSize);

    // Get the version info
    if (!GetFileVersionInfoW(exePath, 0, versionInfoSize, versionInfoBuffer.data())) {
        // Failed to get version info
        SetDlgItemTextW(hwnd, IDC_VERSION_LABEL, L"Version: Error reading version");
        return;
    }

    // Get the fixed file info structure
    VS_FIXEDFILEINFO* fixedFileInfo = nullptr;
    UINT fixedFileInfoSize = 0;

    if (!VerQueryValueW(
            versionInfoBuffer.data(), L"\\", reinterpret_cast<LPVOID*>(&fixedFileInfo), &fixedFileInfoSize) ||
        fixedFileInfoSize == 0) {
        // Failed to get fixed file info
        SetDlgItemTextW(hwnd, IDC_VERSION_LABEL, L"Version: Format error");
        return;
    }

    // Extract the version components
    WORD majorVersion = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minorVersion = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD buildNumber = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD revisionNumber = LOWORD(fixedFileInfo->dwFileVersionLS);

    // Format the version string
    std::wstring versionString = L"Version " + std::to_wstring(majorVersion) + L"." + std::to_wstring(minorVersion) +
        L"." + std::to_wstring(buildNumber) + L"." + std::to_wstring(revisionNumber);

    // Set the version text
    SetDlgItemTextW(hwnd, IDC_VERSION_LABEL, versionString.c_str());
}

}  // namespace progman
