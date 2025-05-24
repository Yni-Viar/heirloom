#include "progman/pch.h"
#include "progman/resource.h"
#include "progman/ProgramManagerWindow.h"
#include "progman/NewFolderDialog.h"
#include "libprogman/window_data.h"
#include "libprogman/string_util.h"
#include "libprogman/window_state.h"

namespace progman {

constexpr WCHAR kClassName[] = L"ProgmanWindowClass";

// Get the path to the window state INI file
std::filesystem::path getWindowStateFilePath() {
    WCHAR appDataPath[MAX_PATH] = { 0 };
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appDataPath))) {
        throw std::runtime_error("Failed to get APPDATA folder path");
    }

    std::filesystem::path path(appDataPath);
    path /= L"Heirloom Program Manager";

    // Create the directory if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    if (ec) {
        throw std::runtime_error("Failed to create settings directory");
    }

    return path / L"window.ini";
}

ProgramManagerWindow::ProgramManagerWindow(HINSTANCE hInstance, libprogman::ShortcutManager* shortcutManager)
    : hInstance_(hInstance), shortcutManager_(shortcutManager) {
    registerWindowClass();

    // Create the main frame window with explicit MDI frame styles
    hwnd_ = CreateWindowW(
        kClassName, L"Program Manager", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT,
        CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance_, this);

    if (!hwnd_) {
        DWORD error = GetLastError();
        std::string errorMsg = "Failed to create Program Manager window. Error code: " + std::to_string(error);
        throw std::runtime_error(errorMsg);
    }

    createMdiClient();

    // Create the minimized folder list control, passing the MDI client as the parent
    minimizedFolderList_ = std::make_unique<MinimizedFolderListControl>(
        hInstance_, mdiClient_, [this](std::wstring folderName) { restoreMinimizedFolder(folderName); });

    // Position the control
    minimizedFolderList_->autoSize(mdiClient_);

    // Load the initial folder windows
    shortcutManager_->refresh();
    syncFolderWindows();

    // Window state is now restored in the show() method
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

    // Get the main window's menu
    HMENU hMainMenu = GetMenu(hwnd_);

    // Find the Window menu directly by position (usually after File menu)
    int windowMenuPos = -1;
    int menuCount = GetMenuItemCount(hMainMenu);
    for (int i = 0; i < menuCount; i++) {
        WCHAR buffer[256] = { 0 };
        GetMenuStringW(hMainMenu, i, buffer, _countof(buffer), MF_BYPOSITION);

        if (wcscmp(buffer, L"&Window") == 0) {
            windowMenuPos = i;
            break;
        }
    }

    if (windowMenuPos >= 0) {
        ccs.hWindowMenu = GetSubMenu(hMainMenu, windowMenuPos);
    }

    ccs.idFirstChild = 1;

    // Create a standard MDI client
    mdiClient_ = CreateWindowW(
        L"MDICLIENT", nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hwnd_, nullptr, hInstance_, &ccs);

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
    // First, try to restore the saved window state
    bool stateRestored = false;
    try {
        libprogman::restoreWindowState(hwnd_, getWindowStateFilePath());
        stateRestored = true;
    } catch (const std::exception& e) {
        // Just log the error - this is not critical
        OutputDebugStringA(e.what());
    }

    // Only use the provided nCmdShow if we couldn't restore the saved state
    if (!stateRestored) {
        // Force window to be shown with SW_SHOWNORMAL if nCmdShow is 0
        if (nCmdShow == 0) {
            nCmdShow = SW_SHOWNORMAL;
        }
        ShowWindow(hwnd_, nCmdShow);
    }

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

                // Resize the minimized folder list
                if (minimizedFolderList_) {
                    minimizedFolderList_->autoSize(mdiClient_);
                }
            }
            return 0;

        case WM_COMMAND:
            // Handle menu commands
            switch (LOWORD(wParam)) {
                case ID_FILE_EXIT:
                    DestroyWindow(hwnd);
                    return 0;

                case ID_FILE_NEWFOLDER: {
                    // Show the new folder dialog
                    NewFolderDialog dialog(hwnd, hInstance_, shortcutManager_);
                    if (dialog.show()) {
                        // Refresh the UI if a folder was created
                        refresh();
                    }
                }
                    return 0;

                case ID_FILE_DELETE:
                    handleDeleteCommand();
                    return 0;

                // Handle Window menu commands
                case ID_WINDOW_CASCADE:
                    SendMessageW(mdiClient_, WM_MDICASCADE, 0, 0);
                    return 0;

                case ID_WINDOW_TILE:
                    SendMessageW(mdiClient_, WM_MDITILE, MDITILE_HORIZONTAL, 0);
                    return 0;

                case ID_WINDOW_ARRANGE:
                    SendMessageW(mdiClient_, WM_MDIICONARRANGE, 0, 0);
                    return 0;
            }
            break;

        case WM_INITMENUPOPUP: {
            // Update the File menu item states when it's about to be shown
            HMENU hMenu = reinterpret_cast<HMENU>(wParam);
            if (HIWORD(lParam) == 0) {  // This is the File menu
                FolderWindow* activeFolder = getActiveFolderWindow();
                // Enable delete only if there's an active folder window
                EnableMenuItem(hMenu, ID_FILE_DELETE, activeFolder ? MF_ENABLED : MF_GRAYED);
            }
            break;
        }

        case WM_DESTROY:
            // Save the window state before destroying
            try {
                libprogman::saveWindowState(hwnd, getWindowStateFilePath());
            } catch (const std::exception& e) {
                // Just log or ignore - this is not critical
                OutputDebugStringA(e.what());
            }
            PostQuitMessage(0);
            return 0;
    }

    // Use DefFrameProc instead of DefWindowProc for proper MDI handling
    return DefFrameProcW(hwnd, mdiClient_, uMsg, wParam, lParam);
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
            // Set the minimize callback
            folderWindow->setOnMinimizeCallback([this](const std::wstring& name) {
                // Add to minimized list
                minimizedFolderList_->addMinimizedFolder(name);
                // Update the layout
                minimizedFolderList_->autoSize(mdiClient_);
            });
            folderWindow->show();
            folderWindows_[folderName] = std::move(folderWindow);
        }
    }
}

void ProgramManagerWindow::restoreMinimizedFolder(const std::wstring& folderName) {
    auto it = folderWindows_.find(folderName);
    if (it != folderWindows_.end()) {
        // Show the window
        it->second->show();

        // Make it the active MDI window
        HWND folderHwnd = it->second->window_;
        SendMessage(mdiClient_, WM_MDIACTIVATE, reinterpret_cast<WPARAM>(folderHwnd), 0);

        // Update the minimized folder list layout
        minimizedFolderList_->autoSize(mdiClient_);
    }
}

FolderWindow* ProgramManagerWindow::getActiveFolderWindow() const {
    HWND activeChildWindow = reinterpret_cast<HWND>(SendMessageW(mdiClient_, WM_MDIGETACTIVE, 0, 0));
    if (!activeChildWindow) {
        return nullptr;
    }

    // Find the FolderWindow instance corresponding to this HWND
    for (const auto& [folderName, folderWindow] : folderWindows_) {
        if (folderWindow->window_ == activeChildWindow) {
            return folderWindow.get();
        }
    }

    return nullptr;
}

void ProgramManagerWindow::handleDeleteCommand() {
    FolderWindow* activeFolder = getActiveFolderWindow();
    if (!activeFolder) {
        return;
    }

    if (activeFolder->hasSelectedItem()) {
        // There's a selected shortcut to delete
        libprogman::Shortcut* shortcut = activeFolder->getSelectedShortcut();
        if (!shortcut) {
            return;
        }

        // Confirm with the user
        std::wstring message = L"Are you sure you want to delete the shortcut \"" + shortcut->name() + L"\"?";
        if (MessageBoxW(hwnd_, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            // Delete the shortcut
            shortcut->deleteFile();
            // The filesystem watcher will pick up the change and update the UI
        }
    } else {
        // No item selected, delete the folder itself
        std::wstring folderName = activeFolder->getName();

        // Get the folder object
        try {
            auto folderPtr = shortcutManager_->folder(folderName);

            // Confirm with the user
            std::wstring message = L"Are you sure you want to delete the folder \"" + folderName + L"\"?";
            if (MessageBoxW(hwnd_, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                // Delete the folder
                shortcutManager_->deleteFolder(folderPtr.get());
                // The filesystem watcher will pick up the change and update the UI
            }
        } catch (const std::exception& e) {
            // Show error message if the folder couldn't be found
            std::wstring errorMsg = L"Error deleting folder: " + libprogman::utf8ToWide(e.what());
            MessageBoxW(hwnd_, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

}  // namespace progman
