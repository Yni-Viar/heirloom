#include "progman/pch.h"
#include "progman/MinimizedFolderListControl.h"
#include "progman/resource.h"
#include "libprogman/window_data.h"

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
constexpr int BASE_SPLITTER_HEIGHT = 8;
constexpr int BASE_MIN_CONTROL_HEIGHT = 64;
constexpr int DEFAULT_HEIGHT = 64;  // Default height in pixels at 96 DPI

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

            // Paint the splitter if needed
            control->paintSplitter(hdc);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEMOVE: {
            // Extract mouse coordinates
            int y = GET_Y_LPARAM(lParam);

            // Start tracking mouse if not already doing so
            if (!control->isTrackingMouse_) {
                control->initMouseTracking();
            }

            // Handle dragging
            if (control->isDragging_) {
                // Get the MDI client window
                HWND mdiClient = GetParent(hwnd);
                if (!mdiClient)
                    break;

                // Get current mouse position in screen coordinates
                POINT ptCurrent = { 0, y };
                ClientToScreen(hwnd, &ptCurrent);

                // Calculate delta from the fixed start position in screen coordinates
                int deltaY = ptCurrent.y - control->dragStartScreen_.y;

                // Calculate new height (drag up = bigger, drag down = smaller)
                int newHeight = control->initialHeight_ - deltaY;

                // Get MDI client rect
                RECT mdiRect;
                GetClientRect(mdiClient, &mdiRect);
                int mdiHeight = mdiRect.bottom - mdiRect.top;

                // Enforce minimum and maximum constraints
                int minHeight = getDpiScaledValue(hwnd, BASE_MIN_CONTROL_HEIGHT);
                int maxHeight = mdiHeight / 2;  // Maximum 50% of MDI client

                int constrainedHeight = newHeight;
                if (constrainedHeight < minHeight) {
                    constrainedHeight = minHeight;
                } else if (constrainedHeight > maxHeight) {
                    constrainedHeight = maxHeight;
                }

                // Only update if height actually changed to reduce flickering
                if (control->controlHeight_ != constrainedHeight) {
                    // Store the new height
                    control->controlHeight_ = constrainedHeight;

                    // Get MDI dimensions
                    RECT mdiRect;
                    GetClientRect(mdiClient, &mdiRect);
                    int mdiWidth = mdiRect.right - mdiRect.left;
                    int mdiHeight = mdiRect.bottom - mdiRect.top;

                    // Position at the bottom of the MDI client
                    int yPos = mdiHeight - constrainedHeight;

                    // Update control position directly without going through autoSize()
                    // Always keep the window at the bottom of the z-order
                    SetWindowPos(
                        hwnd, HWND_BOTTOM, 0, yPos, mdiWidth, constrainedHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);

                    // Update the ListView position
                    SetWindowPos(
                        control->listView_, nullptr, 0, control->splitterHeight_, mdiWidth,
                        constrainedHeight - control->splitterHeight_, SWP_NOZORDER);
                }

                return 0;
            }
            // Check for hover state
            else if (ListView_GetItemCount(control->listView_) > 0) {
                bool inSplitter = control->isPointInSplitter(y);

                // Update cursor and hover state if changed
                if (inSplitter != control->isSplitterHover_) {
                    control->isSplitterHover_ = inSplitter;

                    // Change cursor if in splitter area
                    if (inSplitter) {
                        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                    } else {
                        SetCursor(LoadCursor(nullptr, IDC_ARROW));
                    }

                    // Force repaint to update splitter appearance
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            // Only handle when we have items
            if (ListView_GetItemCount(control->listView_) == 0) {
                break;
            }

            // Check if click is in splitter area
            int y = GET_Y_LPARAM(lParam);
            if (control->isPointInSplitter(y)) {
                // Start dragging
                control->isDragging_ = true;
                control->dragStartY_ = y;
                control->initialHeight_ = control->controlHeight_;

                // Store the initial position in screen coordinates
                POINT ptStart = { 0, y };
                ClientToScreen(hwnd, &ptStart);
                control->dragStartScreen_ = ptStart;

                // Capture mouse
                SetCapture(hwnd);

                // Update cursor
                SetCursor(LoadCursor(nullptr, IDC_SIZENS));

                // Update splitter appearance
                InvalidateRect(hwnd, nullptr, FALSE);

                return 0;
            }

            // For any other click in the control, ensure it stays at the bottom of z-order
            SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;
        }
        case WM_LBUTTONUP: {
            // End dragging if we were dragging
            if (control->isDragging_) {
                control->isDragging_ = false;
                ReleaseCapture();

                // Update splitter appearance
                InvalidateRect(hwnd, nullptr, FALSE);

                return 0;
            }
            break;
        }
        case WM_MOUSELEAVE: {
            // Reset hover state
            if (control->isSplitterHover_) {
                control->isSplitterHover_ = false;
                control->isTrackingMouse_ = false;

                // Update splitter appearance
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            break;
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

                // Ensure the control stays at the bottom of the z-order after processing
                SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                return 0;
            } else if (nmhdr->code == LVN_ENDLABELEDIT) {
                // Handle end of label editing for folder rename
                NMLVDISPINFOW* pDispInfo = reinterpret_cast<NMLVDISPINFOW*>(lParam);
                if (pDispInfo->item.pszText) {  // If not null, user confirmed the edit
                    // Get the old folder name
                    wchar_t oldName[MAX_PATH] = {};
                    ListView_GetItemText(nmhdr->hwndFrom, pDispInfo->item.iItem, 0, oldName, MAX_PATH);
                    
                    // Get the new folder name
                    std::wstring newName = pDispInfo->item.pszText;
                    
                    // Call the rename callback
                    if (control && control->onRename_) {
                        control->onRename_(oldName, newName);
                        return TRUE;  // Accept the new name
                    }
                }
                return FALSE;  // Reject the new name if no callback or no text provided
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

                // Update splitter height based on new DPI
                control->splitterHeight_ = getDpiScaledValue(hwnd, BASE_SPLITTER_HEIGHT);

                // Force a layout update
                if (HWND parent = GetParent(control->window_)) {
                    control->autoSize(parent);
                }
            }
            break;
        }
        case WM_ACTIVATE: {
            // If the window is being activated, force it back to the bottom
            if (LOWORD(wParam) != WA_INACTIVE) {
                SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

                // Return 0 to allow the message to be processed
                return 0;
            }
            break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Add a subclass procedure for the ListView to intercept messages
LRESULT CALLBACK
ListViewSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    MinimizedFolderListControl* control = reinterpret_cast<MinimizedFolderListControl*>(dwRefData);

    switch (uMsg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            // Process the click but make sure parent window stays at bottom of z-order
            if (control && control->window_) {
                SetWindowPos(control->window_, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            break;

        case WM_RBUTTONDOWN: {
            // Handle right-click on a folder item
            LVHITTESTINFO hit = {};
            hit.pt.x = GET_X_LPARAM(lParam);
            hit.pt.y = GET_Y_LPARAM(lParam);
            
            int item = ListView_HitTest(hwnd, &hit);
            if (item != -1) {
                // Select the item that was right-clicked
                ListView_SetItemState(hwnd, item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
                
                // Get the folder name
                wchar_t buffer[MAX_PATH] = {};
                ListView_GetItemText(hwnd, item, 0, buffer, MAX_PATH);
                
                // Convert to screen coordinates
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ClientToScreen(hwnd, &pt);
                
                // Load and show the context menu
                HMENU hMenuResource = LoadMenuW(GetModuleHandle(nullptr), MAKEINTRESOURCEW(IDR_FOLDER_MENU));
                HMENU hMenu = GetSubMenu(hMenuResource, 0);
                
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
                
                DestroyMenu(hMenuResource);
                
                // Handle the selected command
                switch (cmd) {
                    case IDM_OPEN:
                        // Restore the folder (same as double-click)
                        if (control && control->onRestore_) {
                            control->onRestore_(buffer);
                        }
                        // Remove the item from the list
                        ListView_DeleteItem(hwnd, item);
                        break;
                    case IDM_RENAME:
                        // Start editing the label
                        ListView_EditLabel(hwnd, item);
                        break;
                    case IDM_DELETE:
                        // Confirm and delete the folder
                        if (control && control->onDelete_) {
                            std::wstring message = L"Are you sure you want to delete the folder \"" + std::wstring(buffer) + L"\"?";
                            if (MessageBoxW(hwnd, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                                control->onDelete_(buffer);
                                // Remove the item from the list
                                ListView_DeleteItem(hwnd, item);
                            }
                        }
                        break;
                }
            }
            
            // Ensure the control stays at the bottom of z-order
            if (control && control->window_) {
                SetWindowPos(control->window_, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return 0;
        }

        case WM_KEYDOWN: {
            // Handle F2 and Ctrl+D keys
            if (wParam == VK_F2) {
                // Start editing the selected item
                int selectedIndex = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                if (selectedIndex != -1) {
                    ListView_EditLabel(hwnd, selectedIndex);
                }
                return 0;
            } else if (wParam == 'D' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                // Handle Ctrl+D for delete
                int selectedIndex = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
                if (selectedIndex != -1 && control && control->onDelete_) {
                    // Get the folder name
                    wchar_t buffer[MAX_PATH] = {};
                    ListView_GetItemText(hwnd, selectedIndex, 0, buffer, MAX_PATH);
                    
                    // Confirm and delete the folder
                    std::wstring message = L"Are you sure you want to delete the folder \"" + std::wstring(buffer) + L"\"?";
                    if (MessageBoxW(hwnd, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        control->onDelete_(buffer);
                        // Remove the item from the list
                        ListView_DeleteItem(hwnd, selectedIndex);
                    }
                }
                return 0;
            }
            break;
        }

        case WM_NCDESTROY:
            // Remove the subclass when the window is destroyed
            RemoveWindowSubclass(hwnd, ListViewSubclassProc, uIdSubclass);
            return 0;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
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
        WS_EX_NOACTIVATE, L"MinimizedFolderListControl", L"", WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, parent, nullptr,
        instance, nullptr);

    if (!window_) {
        THROW_LAST_ERROR();
    }

    libprogman::setWindowData(window_, this);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Create the ListView control with WS_EX_NOACTIVATE to prevent it from activating
    listView_ = CreateWindowEx(
        WS_EX_NOACTIVATE, WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_EDITLABELS, 0, 0, 100, 100,
        window_, nullptr, instance, nullptr);

    if (!listView_) {
        THROW_LAST_ERROR();
    }

    // Set up subclass procedure for the ListView to keep the parent at the bottom of z-order
    SetWindowSubclass(listView_, ListViewSubclassProc, 0, reinterpret_cast<DWORD_PTR>(this));

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

    // Initialize splitter height with DPI scaling
    splitterHeight_ = getDpiScaledValue(window_, BASE_SPLITTER_HEIGHT);

    // Initialize with default height
    controlHeight_ = getDpiScaledValue(window_, DEFAULT_HEIGHT);
}

// Set the splitter position and update the layout
void MinimizedFolderListControl::setSplitterPosition(int height) {
    // Only accept valid heights
    if (height <= 0) {
        return;
    }

    controlHeight_ = height;

    // Update layout if window exists
    if (window_) {
        HWND parent = GetParent(window_);
        if (parent) {
            autoSize(parent);
        }
    }
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

    // Ensure the control stays at the bottom of the z-order
    if (HWND parent = GetParent(window_)) {
        SetWindowPos(window_, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

int MinimizedFolderListControl::autoSize(HWND mdiClient) {
    if (!mdiClient || !window_) {
        return 0;
    }

    // Get MDI client dimensions
    RECT mdiRect;
    GetClientRect(mdiClient, &mdiRect);
    int mdiWidth = mdiRect.right - mdiRect.left;
    int mdiHeight = mdiRect.bottom - mdiRect.top;

    // Check if we have any items
    int itemCount = ListView_GetItemCount(listView_);
    if (itemCount == 0) {
        // Hide if no items
        ShowWindow(window_, SW_HIDE);
        return 0;
    }

    // Use the stored control height or initialize it if not set
    if (controlHeight_ <= 0) {
        controlHeight_ = getDpiScaledValue(window_, BASE_MIN_CONTROL_HEIGHT);
    }

    // Position flush against left, bottom, and right edges
    int yPos = mdiHeight - controlHeight_;

    // Always ensure the window is at the bottom of the z-order
    SetWindowPos(window_, HWND_BOTTOM, 0, yPos, mdiWidth, controlHeight_, SWP_SHOWWINDOW | SWP_NOACTIVATE);

    // Position the ListView below the splitter
    SetWindowPos(listView_, nullptr, 0, splitterHeight_, mdiWidth, controlHeight_ - splitterHeight_, SWP_NOZORDER);

    return controlHeight_;
}

// Initialize mouse tracking for hover effects
void MinimizedFolderListControl::initMouseTracking() {
    if (!isTrackingMouse_ && window_) {
        TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = window_;

        if (TrackMouseEvent(&tme)) {
            isTrackingMouse_ = true;
        }
    }
}

// Check if a point is within the splitter area
bool MinimizedFolderListControl::isPointInSplitter(int y) const {
    // The splitter is at the top of the control with height splitterHeight_
    return (y >= 0 && y <= splitterHeight_);
}

// Handle splitter painting
void MinimizedFolderListControl::paintSplitter(HDC hdc) {
    // Only paint the splitter if we have items
    if (ListView_GetItemCount(listView_) == 0) {
        return;
    }

    RECT rect;
    GetClientRect(window_, &rect);
    rect.bottom = splitterHeight_;

    // Determine the color based on state
    COLORREF splitterColor;

    if (isDragging_) {
        // Darker when being dragged
        splitterColor =
            RGB(std::max(0, GetRValue(GetSysColor(COLOR_APPWORKSPACE)) - 30),
                std::max(0, GetGValue(GetSysColor(COLOR_APPWORKSPACE)) - 30),
                std::max(0, GetBValue(GetSysColor(COLOR_APPWORKSPACE)) - 30));
    } else if (isSplitterHover_) {
        // Lighter when hovered
        splitterColor =
            RGB(std::min(255, GetRValue(GetSysColor(COLOR_APPWORKSPACE)) + 20),
                std::min(255, GetGValue(GetSysColor(COLOR_APPWORKSPACE)) + 20),
                std::min(255, GetBValue(GetSysColor(COLOR_APPWORKSPACE)) + 20));
    } else {
        // Same as background when not interacting
        splitterColor = GetSysColor(COLOR_APPWORKSPACE);
    }

    HBRUSH brush = CreateSolidBrush(splitterColor);
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
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

bool MinimizedFolderListControl::restoreMinimizedFolder(const std::wstring& folderName, bool maximize) {
    // First check if the folder is in our list
    int itemCount = ListView_GetItemCount(listView_);
    bool found = false;
    int foundIndex = -1;

    for (int i = 0; i < itemCount; i++) {
        LVITEM item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        wchar_t buffer[MAX_PATH] = {};
        item.pszText = buffer;
        item.cchTextMax = MAX_PATH;

        if (ListView_GetItem(listView_, &item)) {
            if (folderName == buffer) {
                found = true;
                foundIndex = i;
                break;
            }
        }
    }

    if (found && foundIndex >= 0) {
        // Remove the item from the list
        ListView_DeleteItem(listView_, foundIndex);

        // Call the restore callback with the folder name
        // The maximize flag will be handled by ProgramManagerWindow
        if (onRestore_) {
            onRestore_(folderName);
        }

        // Return true to indicate success
        return true;
    }

    // Return false if the folder wasn't found in our list
    return false;
}

}  // namespace progman
