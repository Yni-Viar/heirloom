#include "progman/pch.h"
#include "progman/resource.h"
#include "progman/ProgramManagerWindow.h"
#include "libprogman/window_data.h"

namespace progman {

constexpr WCHAR kClassName[] = L"ProgmanWindowClass";

ProgramManagerWindow::ProgramManagerWindow(HINSTANCE hInstance, libprogman::ShortcutManager* shortcutManager)
    : hInstance_(hInstance), shortcutManager_(shortcutManager) {
    registerWindowClass();

    hwnd_ = CreateWindowW(
        kClassName, L"Program Manager", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr,
        hInstance_, this);

    if (!hwnd_) {
        DWORD error = GetLastError();
        std::string errorMsg = "Failed to create Program Manager window. Error code: " + std::to_string(error);
        throw std::runtime_error(errorMsg);
    }

    createMdiClient();

    // Load the initial folder windows
    shortcutManager_->refresh();
    syncFolderWindows();
}

void ProgramManagerWindow::registerWindowClass() {
    // Check if class already exists
    WNDCLASSW wndClass;
    if (GetClassInfoW(hInstance_, kClassName, &wndClass)) {
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
        std::string errorMsg = "Failed to register window class. Error code: " + std::to_string(error);
        throw std::runtime_error(errorMsg);
    }
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
        std::string errorMsg = "Failed to create MDI client window. Error code: " + std::to_string(error);
        throw std::runtime_error(errorMsg);
    }

    // Resize the MDI client to fill the main window
    RECT rcClient;
    GetClientRect(hwnd_, &rcClient);
    MoveWindow(mdiClient_, 0, 0, rcClient.right, rcClient.bottom, TRUE);
}

void ProgramManagerWindow::show(int nCmdShow) {
    // Force window to be shown with SW_SHOWNORMAL if nCmdShow is 0
    if (nCmdShow == 0) {
        nCmdShow = SW_SHOWNORMAL;
    }

    ShowWindow(hwnd_, nCmdShow);
    UpdateWindow(hwnd_);

    // Additional call to force visibility
    SetForegroundWindow(hwnd_);
}

LRESULT CALLBACK ProgramManagerWindow::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ProgramManagerWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<ProgramManagerWindow*>(pCreate->lpCreateParams);
        if (pThis) {
            libprogman::setWindowData(hwnd, pThis);
        }
    } else {
        pThis = libprogman::getWindowData<ProgramManagerWindow>(hwnd);
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
                case ID_FILE_EXIT:
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

void ProgramManagerWindow::refresh() {
    // Refresh the shortcuts from disk.
    shortcutManager_->refresh();

    // Sync the FolderWindows with the updated data
    syncFolderWindows();
}

void ProgramManagerWindow::syncFolderWindows() {
    auto currentFolders = shortcutManager_->folders();

    // First pass: Update existing windows or mark them for removal
    std::vector<std::wstring> foldersToRemove;

    for (auto& [folderName, folderWindow] : folderWindows_) {
        auto folder = currentFolders.find(folderName);
        if (folder != nullptr) {
            // Folder still exists, update it
            folderWindow->setFolder(*folder);
        } else {
            // Folder no longer exists, mark for removal
            folderWindow->close();
            foldersToRemove.push_back(folderName);
        }
    }

    // Remove windows for folders that no longer exist
    for (const auto& folderName : foldersToRemove) {
        folderWindows_.erase(folderName);
    }

    // Second pass: Create windows for new folders
    for (const auto& [folderName, folder] : currentFolders) {
        if (folderWindows_.find(folderName) == folderWindows_.end()) {
            // New folder, create a window for it
            auto folderWindow = std::make_unique<FolderWindow>(hInstance_, mdiClient_, folder);
            folderWindow->show();
            folderWindows_[folderName] = std::move(folderWindow);
        }
    }
}

}  // namespace progman
