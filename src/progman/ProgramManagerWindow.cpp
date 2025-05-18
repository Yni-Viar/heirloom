#include "progman/pch.h"
#include "progman/resource.h"
#include "progman/ProgramManagerWindow.h"

namespace progman {

constexpr WCHAR kClassName[] = L"ProgmanWindowClass";

void ProgramManagerWindow::registerWindowClass() {
    hInstance_ = GetModuleHandle(nullptr);
    if (!hInstance_) {
        MessageBoxW(nullptr, L"Failed to get module handle", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Check if class already exists
    WNDCLASSW wndClass;
    if (GetClassInfoW(hInstance_, kClassName, &wndClass)) {
        OutputDebugStringW(L"Window class already registered");
        return;
    }

    windowClass_.cbSize = sizeof(WNDCLASSEX);
    windowClass_.style = CS_HREDRAW | CS_VREDRAW;
    windowClass_.lpfnWndProc = windowProc;
    windowClass_.cbClsExtra = 0;
    windowClass_.cbWndExtra = 0;
    windowClass_.hInstance = hInstance_;

    // Try loading the application icon
    HICON hIcon = LoadIcon(hInstance_, MAKEINTRESOURCE(IDI_PROGMAN));
    if (hIcon) {
        windowClass_.hIcon = hIcon;
        windowClass_.hIconSm = hIcon;
    } else {
        // Use system icon if application icon can't be loaded
        windowClass_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        windowClass_.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    }

    windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass_.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);

    windowClass_.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    windowClass_.lpszClassName = kClassName;

    if (!RegisterClassExW(&windowClass_)) {
        DWORD error = GetLastError();
        std::wstring errorMsg = L"Failed to register window class. Error code: " + std::to_wstring(error);
        MessageBoxW(nullptr, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
    }
}

void ProgramManagerWindow::create() {
    registerWindowClass();

    if (!hInstance_) {
        return;
    }

    // Create a simple window first - just basic styles
    hwnd_ = CreateWindowW(
        kClassName, L"Program Manager", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr,
        hInstance_, this);

    if (!hwnd_) {
        DWORD error = GetLastError();
        std::wstring errorMsg = L"Failed to create Program Manager window. Error code: " + std::to_wstring(error);
        MessageBoxW(nullptr, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create MDI client after basic window is working
    createMdiClient();
}

void ProgramManagerWindow::createMdiClient() {
    CLIENTCREATESTRUCT ccs{};
    ccs.hWindowMenu = nullptr;
    ccs.idFirstChild = 1;

    // Create a standard MDI client
    mdiClient_ =
        CreateWindowW(L"MDICLIENT", nullptr, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd_, nullptr, hInstance_, &ccs);

    if (!mdiClient_) {
        DWORD error = GetLastError();
        MessageBoxW(nullptr, L"Failed to create MDI client window", L"Error", MB_OK | MB_ICONERROR);
    }

    // Resize the MDI client to fill the main window
    RECT rcClient;
    GetClientRect(hwnd_, &rcClient);
    MoveWindow(mdiClient_, 0, 0, rcClient.right, rcClient.bottom, TRUE);
}

void ProgramManagerWindow::show(int nCmdShow) {
    OutputDebugStringW((L"Showing window with command: " + std::to_wstring(nCmdShow)).c_str());

    // Force window to be shown with SW_SHOWNORMAL if nCmdShow is 0
    if (nCmdShow == 0) {
        nCmdShow = SW_SHOWNORMAL;
    }

    ShowWindow(hwnd_, nCmdShow);
    UpdateWindow(hwnd_);

    // Additional call to force visibility
    SetForegroundWindow(hwnd_);

    OutputDebugStringW(L"Window should be visible now");
}

LRESULT CALLBACK ProgramManagerWindow::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ProgramManagerWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<ProgramManagerWindow*>(pCreate->lpCreateParams);
        if (pThis) {
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        }
    } else {
        pThis = reinterpret_cast<ProgramManagerWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->handleMessage(hwnd, uMsg, wParam, lParam);
    } else {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT ProgramManagerWindow::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            // Resize the MDI client window
            if (mdiClient_) {
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                MoveWindow(mdiClient_, 0, 0, rcClient.right, rcClient.bottom, TRUE);
            }
            return 0;

        case WM_COMMAND:
            // Handle menu commands
            switch (LOWORD(wParam)) {
                case IDM_FILE_EXIT:
                    // Handle File > Exit
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    // For a standard window (not a frame window)
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

}  // namespace progman
