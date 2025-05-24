#include "progman/pch.h"
#include "progman/MinimizedFolderListControl.h"
#include "progman/resource.h"
#include "libprogman/window_data.h"
#include <commctrl.h>

namespace progman {

// Using system metrics instead of hard-coded pixel values
int getDpiScaledValue(HWND hwnd, int value) {
    // Get the DPI for the window
    UINT dpi = GetDpiForWindow(hwnd);
    // Default DPI is 96
    return MulDiv(value, dpi, 96);
}

// Base values at 96 DPI
constexpr int BASE_ICON_SIZE = 32;
constexpr int BASE_PADDING = 8;

LRESULT CALLBACK MinimizedFolderListControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* control = libprogman::getWindowData<MinimizedFolderListControl>(hwnd);
    if (!control) {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    switch (message) {
        case WM_ERASEBKGND: {
            // Fill with appworkspace color
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
            return TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NOTIFY: {
            LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
            if (nmhdr->code == NM_DBLCLK) {
                LPNMITEMACTIVATE lpnmitem = reinterpret_cast<LPNMITEMACTIVATE>(lParam);
                LVITEM item = {};
                item.mask = LVIF_TEXT;
                item.iItem = lpnmitem->iItem;
                wchar_t buffer[MAX_PATH] = {};
                item.pszText = buffer;
                item.cchTextMax = MAX_PATH;
                ListView_GetItem(nmhdr->hwndFrom, &item);

                // Remove the item from the list
                ListView_DeleteItem(nmhdr->hwndFrom, lpnmitem->iItem);

                // Call the restore callback
                if (control) {
                    control->onRestore_(buffer);
                }
                return 0;
            }
            break;
        }
        case WM_DPICHANGED: {
            // Update the control when DPI changes
            if (control->window_ && control->listView_) {
                // Recreate the image list with the new DPI scaling
                int iconSize = getDpiScaledValue(hwnd, BASE_ICON_SIZE);

                HIMAGELIST imageList = ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK, 1, 1);
                HICON folderIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_FOLDER));
                ImageList_AddIcon(imageList, folderIcon);

                // Replace the existing image list
                HIMAGELIST oldImageList = ListView_SetImageList(control->listView_, imageList, LVSIL_NORMAL);
                if (oldImageList) {
                    ImageList_Destroy(oldImageList);
                }

                // Force a layout update
                if (HWND parent = GetParent(control->window_)) {
                    control->autoSize(parent);
                }
            }
            break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

MinimizedFolderListControl::MinimizedFolderListControl(
    HINSTANCE instance,
    HWND parent,
    std::function<void(std::wstring)> onRestore)
    : onRestore_(std::move(onRestore)) {
    // Register window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MinimizedFolderListControlProc;
    wcex.hInstance = instance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;  // We'll handle background ourselves
    wcex.lpszClassName = L"MinimizedFolderListControl";
    RegisterClassEx(&wcex);

    // Create the container window
    window_ = CreateWindowEx(
        0, L"MinimizedFolderListControl", L"", WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, parent, nullptr, instance,
        nullptr);

    if (!window_) {
        THROW_LAST_ERROR();
    }

    libprogman::setWindowData(window_, this);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Create the ListView control
    listView_ = CreateWindowEx(
        0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL | LVS_NOCOLUMNHEADER, 0,
        0, 100, 100, window_, nullptr, instance, nullptr);

    if (!listView_) {
        THROW_LAST_ERROR();
    }

    // Enable alphabetical sorting
    ListView_SetExtendedListViewStyle(listView_, LVS_EX_AUTOSIZECOLUMNS);

    // Set up the image list with folder icon with DPI scaling
    int iconSize = getDpiScaledValue(window_, BASE_ICON_SIZE);
    HIMAGELIST imageList = ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK, 1, 1);
    HICON folderIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_FOLDER));

    if (!folderIcon) {
        // Try loading a system icon as fallback
        folderIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_FOLDER));
        if (!folderIcon) {
            folderIcon = LoadIcon(nullptr, IDI_APPLICATION);
        }
    }

    ImageList_AddIcon(imageList, folderIcon);
    ListView_SetImageList(listView_, imageList, LVSIL_NORMAL);

    // Set background color to match MDI workspace
    ListView_SetBkColor(listView_, GetSysColor(COLOR_APPWORKSPACE));
    ListView_SetTextBkColor(listView_, GetSysColor(COLOR_APPWORKSPACE));

    // Make sure no scrollbars are shown
    ListView_SetExtendedListViewStyle(listView_, LVS_EX_AUTOSIZECOLUMNS);
}

HWND MinimizedFolderListControl::hwnd() const {
    return window_;
}

void MinimizedFolderListControl::addMinimizedFolder(std::wstring name) {
    // Insert item with error checking
    LVITEM item = {};
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iItem = 0;  // Insert at the beginning, sorting will reposition it
    item.iSubItem = 0;
    item.pszText = const_cast<LPWSTR>(name.c_str());
    item.cchTextMax = static_cast<int>(name.length());
    item.iImage = 0;  // Index of the folder icon in the image list

    // Store the name string as item data - this memory needs to persist
    wchar_t* itemText = _wcsdup(name.c_str());
    item.lParam = reinterpret_cast<LPARAM>(itemText);

    ListView_InsertItem(listView_, &item);

    // Sort the items alphabetically
    ListView_SortItems(
        listView_,
        [](LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/) -> int {
            const wchar_t* text1 = reinterpret_cast<const wchar_t*>(lParam1);
            const wchar_t* text2 = reinterpret_cast<const wchar_t*>(lParam2);

            return _wcsicmp(text1, text2);
        },
        0);

    // Force layout update
    RECT clientRect;
    GetClientRect(window_, &clientRect);
    SetWindowPos(listView_, nullptr, 0, 0, clientRect.right, clientRect.bottom, SWP_NOZORDER);
}

int MinimizedFolderListControl::autoSize(HWND mdiClient) const {
    if (!mdiClient || !window_) {
        return 0;
    }

    // Get MDI client dimensions
    RECT mdiRect;
    GetClientRect(mdiClient, &mdiRect);
    int mdiWidth = mdiRect.right - mdiRect.left;
    int mdiHeight = mdiRect.bottom - mdiRect.top;

    // Calculate required height based on item count
    int itemCount = ListView_GetItemCount(listView_);
    if (itemCount == 0) {
        // Hide if no items
        ShowWindow(window_, SW_HIDE);
        return 0;
    }

    // Get DPI-scaled values
    int iconSize = getDpiScaledValue(window_, BASE_ICON_SIZE);
    int padding = getDpiScaledValue(window_, BASE_PADDING);

    // Calculate how many icons can fit per row
    int itemsPerRow = std::max(1, mdiWidth / (iconSize + padding * 2));
    int rows = (itemCount + itemsPerRow - 1) / itemsPerRow;  // Ceiling division
    int requiredHeight = rows * (iconSize + padding * 3);

    // Position flush against left, bottom, and right edges
    int yPos = mdiHeight - requiredHeight;
    SetWindowPos(window_, HWND_BOTTOM, 0, yPos, mdiWidth, requiredHeight, SWP_SHOWWINDOW);

    // Size the ListView to fill the entire container
    SetWindowPos(listView_, nullptr, 0, 0, mdiWidth, requiredHeight, SWP_NOZORDER);

    return requiredHeight;
}

// Add destructor to cleanup allocated memory for item text
MinimizedFolderListControl::~MinimizedFolderListControl() {
    // Free memory allocated for item text
    int itemCount = ListView_GetItemCount(listView_);
    for (int i = 0; i < itemCount; i++) {
        LVITEM item = {};
        item.mask = LVIF_PARAM;
        item.iItem = i;

        if (ListView_GetItem(listView_, &item) && item.lParam) {
            free(reinterpret_cast<void*>(item.lParam));
        }
    }
}

}  // namespace progman
