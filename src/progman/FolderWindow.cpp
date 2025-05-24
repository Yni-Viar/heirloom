#include "progman/pch.h"
#include "progman/resource.h"
#include "progman/FolderWindow.h"
#include "libprogman/string_util.h"
#include "libprogman/window_data.h"

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

// Constructor
FolderWindow::FolderWindow(HINSTANCE instance, HWND mdiClient, std::shared_ptr<libprogman::ShortcutFolder> folder)
    : folder_(folder) {
    // Register window class if needed
    WNDCLASSEXW wcex = {};
    if (!GetClassInfoExW(GetModuleHandleW(nullptr), kFolderWindowClass, &wcex)) {
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = FolderWindowProc;
        wcex.hInstance = GetModuleHandleW(nullptr);
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = kFolderWindowClass;
        wcex.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDI_FOLDER));

        if (!RegisterClassExW(&wcex)) {
            THROW_LAST_ERROR();
        }
    }

    // Setup MDICREATESTRUCT
    MDICREATESTRUCTW mcs = {};
    mcs.szClass = kFolderWindowClass;
    mcs.szTitle = folder_->name().c_str();
    mcs.hOwner = instance;
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
}

void FolderWindow::close() {
    if (window_) {
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
        0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL, 0, 0,
        clientRect.right, clientRect.bottom, window_, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!listView_) {
        THROW_LAST_ERROR();
    }

    // Configure the ListView
    ListView_SetExtendedListViewStyle(listView_, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);

    // Set system image list for large icons
    HIMAGELIST hSystemImageList =
        ImageList_Create(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), ILC_COLOR32 | ILC_MASK, 30, 10);

    ListView_SetImageList(listView_, hSystemImageList, LVSIL_NORMAL);
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
}

void FolderWindow::setOnMinimizeCallback(std::function<void(const std::wstring&)> callback) {
    onMinimizeCallback_ = std::move(callback);
}

std::wstring FolderWindow::getName() const {
    return folder_ ? folder_->name() : L"";
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
            // Intercept close command and treat it like minimize
            // As with the old school Program Manager, the user can't destroy a group simply by closing it
            if (onMinimizeCallback_ && folder_) {
                onMinimizeCallback_(folder_->name());
            }
            // Don't let the window actually close, just hide it
            ShowWindow(hwnd, SW_HIDE);
            return 0;  // Handled, don't pass to default proc

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

                                HMENU hMenu = CreatePopupMenu();
                                AppendMenuW(hMenu, MF_STRING, 1, L"&Open");
                                AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                                AppendMenuW(hMenu, MF_STRING, 2, L"&Properties");

                                int cmd = TrackPopupMenu(
                                    hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);

                                DestroyMenu(hMenu);

                                if (cmd == 1) {
                                    shortcut->launch();
                                } else if (cmd == 2) {
                                    shortcut->showPropertiesWindow();
                                }
                            }
                        }
                        return 0;
                    }
                }
            }
            break;
        }

        case WM_DESTROY:
            return 0;
    }

    return DefMDIChildProcW(hwnd, message, wParam, lParam);
}

}  // namespace progman
