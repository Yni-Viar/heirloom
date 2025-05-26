#include "progman/pch.h"
#include "progman/resource.h"
#include "progman/FolderWindow.h"
#include "libprogman/constants.h"
#include "libprogman/string_util.h"
#include "libprogman/window_data.h"
#include "libprogman/window_state.h"

namespace progman {

constexpr WCHAR kFolderWindowClass[] = L"ProgmanFolderWindowClass";

// Window procedure for the folder window
LRESULT CALLBACK FolderWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    FolderWindow* pThis = nullptr;

    if (message == WM_NCCREATE) {
        auto* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        auto* mcs = reinterpret_cast<MDICREATESTRUCT*>(pCreate->lpCreateParams);
        pThis = reinterpret_cast<FolderWindow*>(mcs->lParam);
        if (!pThis) {
            throw std::runtime_error("Failed to get FolderWindow pointer from WM_NCCREATE.");
        }
        pThis->window_ = hwnd;
        libprogman::setWindowData(hwnd, pThis);
    } else {
        pThis = libprogman::getWindowData<FolderWindow>(hwnd);
    }

    if (pThis) {
        return pThis->handleMessage(hwnd, message, wParam, lParam);
    }

    return DefMDIChildProcW(hwnd, message, wParam, lParam);
}

// Helper function to get the ini file path
std::optional<std::filesystem::path> FolderWindow::getIniFilePath() const {
    if (!folder_ || folder_->path().empty()) {
        return std::nullopt;
    }
    return folder_->path() / L"window.ini";
}

// Helper function to save window state
void FolderWindow::saveWindowState(HWND hwnd) {
    auto iniFilePath = getIniFilePath();
    if (iniFilePath) {
        libprogman::saveWindowState(hwnd, *iniFilePath);
        // Also save whether the window is minimized to the list
        libprogman::saveWindowMinimizedState(*iniFilePath, isMinimized_);
    }
}

// Helper function to restore window state
void FolderWindow::restoreWindowState(HWND hwnd) {
    auto iniFilePath = getIniFilePath();
    if (iniFilePath) {
        libprogman::restoreWindowState(hwnd, *iniFilePath);
    }
}

// Public method to save window state
void FolderWindow::saveState() {
    if (window_) {
        saveWindowState(window_);
    }
}

// Set minimized state
void FolderWindow::setMinimized(bool minimized) {
    isMinimized_ = minimized;
}

// Get current minimized state
bool FolderWindow::isMinimized() const {
    return isMinimized_;
}

// Check if the window was minimized on save
bool FolderWindow::wasMinimizedOnSave() const {
    auto iniFilePath = getIniFilePath();
    if (iniFilePath) {
        return libprogman::getWindowMinimizedState(*iniFilePath);
    }
    return false;
}

// Helper function to get the appropriate icon size based on DPI
int getIconSizeForDpi(HWND hwnd) {
    // Get the DPI for the window
    UINT dpi = GetDpiForWindow(hwnd);

    // Calculate scaling factor (96 is the baseline DPI)
    double scalingFactor = static_cast<double>(dpi) / 96.0;

    // Base icon size is 32x32, scale it based on DPI
    return static_cast<int>(32 * scalingFactor);
}

// Comparison function for sorting ListView items by name
static int CALLBACK CompareListViewItems(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/) {
    libprogman::Shortcut* shortcut1 = reinterpret_cast<libprogman::Shortcut*>(lParam1);
    libprogman::Shortcut* shortcut2 = reinterpret_cast<libprogman::Shortcut*>(lParam2);

    if (!shortcut1 || !shortcut2) {
        return 0;
    }

    // Compare by name (case-insensitive)
    return _wcsicmp(shortcut1->name().c_str(), shortcut2->name().c_str());
}

// Constructor
FolderWindow::FolderWindow(
    HINSTANCE instance,
    HWND mdiClient,
    std::shared_ptr<libprogman::ShortcutFolder> folder,
    libprogman::ShortcutManager* shortcutManager)
    : instance_(instance), folder_(folder), shortcutManager_(shortcutManager) {
    // Register window class if needed
    WNDCLASSEXW wcex = {};
    if (!GetClassInfoExW(instance_, kFolderWindowClass, &wcex)) {
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = FolderWindowProc;
        wcex.hInstance = instance_;
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = kFolderWindowClass;
        wcex.hIcon = LoadIconW(instance_, MAKEINTRESOURCEW(IDI_FOLDER));

        if (!RegisterClassExW(&wcex)) {
            THROW_LAST_ERROR();
        }
    }

    // Setup MDICREATESTRUCT
    MDICREATESTRUCTW mcs = {};
    mcs.szClass = kFolderWindowClass;
    mcs.szTitle = folder_->name().c_str();
    mcs.hOwner = instance_;
    mcs.x = mcs.y = CW_USEDEFAULT;
    mcs.cx = mcs.cy = CW_USEDEFAULT;
    // Make sure we include the system menu and all window styles for proper MDI behavior
    mcs.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    mcs.lParam = reinterpret_cast<LPARAM>(this);

    // Send the MDI create message directly
    window_ = reinterpret_cast<HWND>(SendMessageW(mdiClient, WM_MDICREATE, 0, reinterpret_cast<LPARAM>(&mcs)));

    if (!window_) {
        THROW_LAST_ERROR();
    }

    // Create ListView control
    createListView();

    // Populate the ListView with shortcuts
    refreshListView();

    // Restore window position if available
    restoreWindowState(window_);
}

void FolderWindow::setFolder(std::shared_ptr<libprogman::ShortcutFolder> folder) {
    folder_ = folder;

    // Update window title
    SetWindowTextW(window_, folder_->name().c_str());

    // Refresh the ListView
    refreshListView();
}

void FolderWindow::show() {
    ShowWindow(window_, SW_SHOW);
    UpdateWindow(window_);

    // When showing the window, it's no longer minimized to the list
    isMinimized_ = false;
}

void FolderWindow::close() {
    if (window_) {
        // Save window position before closing
        saveWindowState(window_);

        SendMessageW(GetParent(window_), WM_MDIDESTROY, reinterpret_cast<WPARAM>(window_), 0);
        window_ = nullptr;
        listView_ = nullptr;
    }
}

void FolderWindow::createListView() {
    RECT clientRect;
    GetClientRect(window_, &clientRect);

    // Create ListView control
    listView_ = CreateWindowExW(
        0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL | LVS_EDITLABELS, 0, 0,
        clientRect.right, clientRect.bottom, window_, nullptr, instance_, nullptr);

    if (!listView_) {
        THROW_LAST_ERROR();
    }

    // Configure the ListView
    ListView_SetExtendedListViewStyle(listView_, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);

    // Get the appropriate icon size based on DPI
    int iconSize = getIconSizeForDpi(window_);

    // Create appropriately sized image list for the current DPI scaling
    HIMAGELIST hImageList = ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK, 30, 10);
    if (hImageList) {
        // Set the image list to the ListView
        ListView_SetImageList(listView_, hImageList, LVSIL_NORMAL);
    } else {
        // Fallback to old behavior if image list creation fails
        HIMAGELIST hSystemImageList =
            ImageList_Create(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), ILC_COLOR32 | ILC_MASK, 30, 10);
        ListView_SetImageList(listView_, hSystemImageList, LVSIL_NORMAL);
    }
}

void FolderWindow::refreshListView() {
    if (!listView_ || !folder_) {
        return;
    }

    // Clear existing items
    ListView_DeleteAllItems(listView_);

    // Populate with shortcuts
    int itemIndex = 0;
    for (const auto& [name, shortcut] : folder_->shortcuts()) {
        // Add icon to the image list
        HIMAGELIST hImageList = ListView_GetImageList(listView_, LVSIL_NORMAL);
        int iconIndex = ImageList_AddIcon(hImageList, shortcut->icon().get());

        // Add item to ListView
        LVITEMW lvItem = {};
        lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        lvItem.iItem = itemIndex++;
        lvItem.iSubItem = 0;
        lvItem.pszText = const_cast<LPWSTR>(name.c_str());
        lvItem.iImage = iconIndex;
        lvItem.lParam = reinterpret_cast<LPARAM>(shortcut.get());

        ListView_InsertItem(listView_, &lvItem);
    }

    // Sort items by name
    ListView_SortItems(listView_, CompareListViewItems, 0);
}

void FolderWindow::setOnMinimizeCallback(std::function<void(const std::wstring&)> callback) {
    onMinimizeCallback_ = std::move(callback);
}

std::wstring FolderWindow::getName() const {
    return folder_ ? folder_->name() : L"";
}

bool FolderWindow::hasSelectedItem() const {
    if (!listView_) {
        return false;
    }

    // Get the selected item
    return ListView_GetSelectedCount(listView_) > 0;
}

libprogman::Shortcut* FolderWindow::getSelectedShortcut() const {
    if (!listView_) {
        return nullptr;
    }

    // Get the selected item
    int selectedIndex = ListView_GetNextItem(listView_, -1, LVNI_SELECTED);
    if (selectedIndex == -1) {
        return nullptr;
    }

    // Get the shortcut associated with the selected item
    LVITEMW lvItem = {};
    lvItem.mask = LVIF_PARAM;
    lvItem.iItem = selectedIndex;
    ListView_GetItem(listView_, &lvItem);

    return reinterpret_cast<libprogman::Shortcut*>(lvItem.lParam);
}

void FolderWindow::renameSelectedItem() {
    if (!listView_) {
        return;
    }

    int selectedIndex = ListView_GetNextItem(listView_, -1, LVNI_SELECTED);
    if (selectedIndex != -1) {
        // Start editing the label
        ListView_EditLabel(listView_, selectedIndex);
    }
}

void FolderWindow::handleDeleteCommand() {
    if (hasSelectedItem()) {
        // There's a selected shortcut to delete
        libprogman::Shortcut* shortcut = getSelectedShortcut();
        if (!shortcut) {
            return;
        }

        // Confirm with the user
        std::wstring message = L"Are you sure you want to delete the shortcut \"" + shortcut->name() + L"\"?";
        if (MessageBoxW(window_, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            // Delete the shortcut
            shortcut->deleteFile();
            // The filesystem watcher will pick up the change and update the UI
        }
    } else {
        // No item selected, delete the folder itself
        std::wstring folderName = getName();

        // Get the folder object
        try {
            auto folderPtr = shortcutManager_->folder(folderName);
            if (!folderPtr) {
                return;
            }

            // Confirm with the user
            std::wstring message = L"Are you sure you want to delete the folder \"" + folderName + L"\"?";
            if (MessageBoxW(window_, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                // Delete the folder
                shortcutManager_->deleteFolder(folderPtr.get());
                // The filesystem watcher will pick up the change and update the UI
            }
        } catch (const std::exception& e) {
            // Show error message if the folder couldn't be found
            std::wstring errorMsg = L"Error deleting folder: " + libprogman::utf8ToWide(e.what());
            MessageBoxW(window_, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

LRESULT FolderWindow::handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_SIZE: {
            // Resize the ListView to fill the client area
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            if (listView_) {
                MoveWindow(listView_, 0, 0, clientRect.right, clientRect.bottom, TRUE);
            }
            // Continue with default handling for MDI child window
            break;
        }

        case WM_SYSCOMMAND:
            // Completely intercept the minimize command
            if ((wParam & 0xFFF0) == SC_MINIMIZE) {
                // Save window position before minimizing
                saveWindowState(hwnd);

                // Mark as minimized to the list
                isMinimized_ = true;

                // Call our minimize callback if set
                if (onMinimizeCallback_ && folder_) {
                    onMinimizeCallback_(folder_->name());
                }
                // Don't let the window actually minimize, just hide it
                ShowWindow(hwnd, SW_HIDE);
                return 0;  // Handled, don't pass to default proc
            }
            break;

        case WM_CLOSE:
            // Save window position before closing
            saveWindowState(hwnd);

            // Mark as minimized to the list since we're hiding it
            isMinimized_ = true;

            // Intercept close command and treat it like minimize
            // As with the old school Program Manager, the user can't destroy a group simply by closing it
            if (onMinimizeCallback_ && folder_) {
                onMinimizeCallback_(folder_->name());
            }
            // Don't let the window actually close, just hide it
            ShowWindow(hwnd, SW_HIDE);
            return 0;  // Handled, don't pass to default proc

        case libprogman::WM_FOLDERWINDOW_DELETE:
            handleDeleteCommand();
            return 0;

        case libprogman::WM_FOLDERWINDOW_RENAME:
            renameSelectedItem();
            return 0;

        case WM_NOTIFY: {
            NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
            if (nmhdr->hwndFrom == listView_) {
                switch (nmhdr->code) {
                    case NM_DBLCLK: {
                        // Handle double-click on a shortcut
                        NMITEMACTIVATE* nmia = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        if (nmia->iItem != -1) {
                            LVITEMW lvItem = {};
                            lvItem.mask = LVIF_PARAM;
                            lvItem.iItem = nmia->iItem;
                            ListView_GetItem(listView_, &lvItem);

                            libprogman::Shortcut* shortcut = reinterpret_cast<libprogman::Shortcut*>(lvItem.lParam);
                            if (shortcut) {
                                shortcut->launch();
                            }
                        }
                        return 0;
                    }

                    case NM_RETURN: {
                        // Handle Enter key press on a shortcut
                        libprogman::Shortcut* shortcut = getSelectedShortcut();
                        if (shortcut) {
                            shortcut->launch();
                        }
                        return 0;
                    }

                    case NM_RCLICK: {
                        // Handle right-click on a shortcut (for context menu)
                        NMITEMACTIVATE* nmia = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        if (nmia->iItem != -1) {
                            LVITEMW lvItem = {};
                            lvItem.mask = LVIF_PARAM;
                            lvItem.iItem = nmia->iItem;
                            ListView_GetItem(listView_, &lvItem);

                            libprogman::Shortcut* shortcut = reinterpret_cast<libprogman::Shortcut*>(lvItem.lParam);
                            if (shortcut) {
                                POINT pt = nmia->ptAction;
                                ClientToScreen(listView_, &pt);

                                // Load menu from resources
                                HMENU hMenuResource = LoadMenuW(instance_, MAKEINTRESOURCEW(IDR_SHORTCUT_MENU));
                                HMENU hMenu = GetSubMenu(hMenuResource, 0);

                                // Show the context menu
                                int cmd = TrackPopupMenu(
                                    hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);

                                DestroyMenu(hMenuResource);

                                // Handle the selected command
                                switch (cmd) {
                                    case IDM_OPEN:
                                        shortcut->launch();
                                        break;
                                    case IDM_RENAME:
                                        renameSelectedItem();
                                        break;
                                    case IDM_PROPERTIES:
                                        shortcut->showPropertiesWindow();
                                        break;
                                    case IDM_DELETE:
                                        handleDeleteCommand();
                                        break;
                                }
                            }
                        }
                        return 0;
                    }

                    case LVN_ENDLABELEDIT: {
                        // Handle end of label editing
                        NMLVDISPINFOW* pDispInfo = reinterpret_cast<NMLVDISPINFOW*>(lParam);
                        if (pDispInfo->item.pszText) {  // If not null, user confirmed the edit
                            // Get the shortcut associated with the edited item
                            libprogman::Shortcut* shortcut =
                                reinterpret_cast<libprogman::Shortcut*>(pDispInfo->item.lParam);
                            if (shortcut) {
                                // Get the old path
                                std::filesystem::path oldPath = shortcut->path();
                                // Get the directory path
                                std::filesystem::path dirPath = oldPath.parent_path();
                                // Get the file extension
                                std::wstring extension = oldPath.extension().wstring();

                                // Create new path with new filename (preserve original extension)
                                std::filesystem::path newPath =
                                    dirPath / (std::wstring(pDispInfo->item.pszText) + extension);

                                try {
                                    // Rename the file
                                    std::filesystem::rename(oldPath, newPath);
                                    return TRUE;  // Accept the new name
                                } catch (const std::exception&) {
                                    // Renaming failed
                                    return FALSE;  // Reject the new name
                                }
                            }
                        }
                        return FALSE;  // Reject the new name if no text provided
                    }

                    case LVN_KEYDOWN: {
                        // Handle keyboard events from the list view
                        NMLVKEYDOWN* pnkd = reinterpret_cast<NMLVKEYDOWN*>(lParam);
                        if (pnkd->wVKey == VK_F2) {
                            renameSelectedItem();
                            return 0;
                        }
                        break;
                    }
                }
            }
            break;
        }

        case WM_KEYDOWN:
            // Handle key events for the window
            if (wParam == VK_F2) {
                renameSelectedItem();
                return 0;
            }
            break;

        case WM_DESTROY:
            return 0;

        case WM_COMMAND:
            // Handle commands from menus or accelerators
            switch (LOWORD(wParam)) {
                case IDM_RENAME:
                    renameSelectedItem();
                    return 0;
                case IDM_DELETE:
                    handleDeleteCommand();
                    return 0;
            }
            break;
    }

    return DefMDIChildProcW(hwnd, message, wParam, lParam);
}

}  // namespace progman
