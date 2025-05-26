#include "progman/pch.h"
#include "progman/NewShortcutDialog.h"
#include "progman/resource.h"
#include "libprogman/window_data.h"

using namespace progman;
using namespace libprogman;

INT_PTR CALLBACK NewShortcutDialog::dialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    NewShortcutDialog* dialog = nullptr;

    if (message == WM_INITDIALOG) {
        dialog = reinterpret_cast<NewShortcutDialog*>(lParam);
        setWindowData(hwnd, dialog);
        dialog->initializeDialog(hwnd);
        return TRUE;
    }

    dialog = getWindowData<NewShortcutDialog>(hwnd);
    if (!dialog) {
        return FALSE;
    }

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    dialog->handleOkButton(hwnd);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;

                case IDC_BROWSE:
                    dialog->handleBrowseButton(hwnd);
                    return TRUE;

                case IDC_PATH_EDIT:
                    if (HIWORD(wParam) == EN_SETFOCUS) {
                        dialog->selectPathRadio(hwnd);
                    }
                    return TRUE;
            }
            break;

        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->idFrom == IDC_APPLICATIONS_LIST) {
                switch (((LPNMHDR)lParam)->code) {
                    case NM_CLICK:
                    case LVN_ITEMCHANGED:
                        dialog->selectApplicationRadio(hwnd);
                        return TRUE;
                    case NM_DBLCLK:
                        dialog->selectApplicationRadio(hwnd);
                        dialog->handleOkButton(hwnd);
                        return TRUE;
                }
            }
            break;

        case WM_CLOSE:
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
    }

    return FALSE;
}

NewShortcutDialog::NewShortcutDialog(
    HWND parentWindow,
    HINSTANCE hInstance,
    std::filesystem::path folder,
    immer::vector<std::shared_ptr<libprogman::Shortcut>> installedApps,
    libprogman::ShortcutFactory* shortcutFactory)
    : parentWindow_(parentWindow),
      hInstance_(hInstance),
      folder_(folder),
      installedApps_(installedApps),
      shortcutFactory_(shortcutFactory) {}

void NewShortcutDialog::showDialog() {
    dialogHandle_ = nullptr;

    INT_PTR result = DialogBoxParam(
        hInstance_, MAKEINTRESOURCE(IDD_NEW_SHORTCUT), parentWindow_, dialogProc, reinterpret_cast<LPARAM>(this));

    if (result == -1) {
        THROW_IF_WIN32_BOOL_FALSE(FALSE);
    }
}

void NewShortcutDialog::initializeDialog(HWND hwnd) {
    dialogHandle_ = hwnd;

    // Initialize list view
    HWND listView = GetDlgItem(hwnd, IDC_APPLICATIONS_LIST);
    // Add LVS_NOCOLUMNHEADER style to hide column headers
    LONG_PTR style = GetWindowLongPtr(listView, GWL_STYLE);
    SetWindowLongPtr(listView, GWL_STYLE, style | LVS_NOCOLUMNHEADER);
    ListView_SetView(listView, LVS_REPORT);

    // Add a single column that takes the full width
    RECT listRect;
    GetClientRect(listView, &listRect);
    LVCOLUMN column = { 0 };
    column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    column.cx = listRect.right - listRect.left - GetSystemMetrics(SM_CXVSCROLL);
    column.pszText = const_cast<LPWSTR>(L"Name");
    column.iSubItem = 0;
    ListView_InsertColumn(listView, 0, &column);

    // Get DPI for the current window
    int dpi = GetDpiForWindow(hwnd);
    float dpiScale = static_cast<float>(dpi) / 96.0f;

    // Scale icon size based on DPI
    int iconSize = static_cast<int>(16 * dpiScale);

    // Create image list with DPI-scaled icons
    HIMAGELIST imageList =
        ImageList_Create(iconSize, iconSize, ILC_COLOR32 | ILC_MASK, 0, static_cast<int>(installedApps_.size()));

    ListView_SetImageList(listView, imageList, LVSIL_SMALL);

    // Select Application radio by default
    CheckDlgButton(hwnd, IDC_INSTALLED_APPLICATION_RADIO, BST_CHECKED);

    // Populate applications list
    populateApplicationsList(hwnd);
}

void NewShortcutDialog::populateApplicationsList(HWND hwnd) {
    HWND listView = GetDlgItem(hwnd, IDC_APPLICATIONS_LIST);

    // Get DPI for the current window
    int dpi = GetDpiForWindow(hwnd);
    float dpiScale = static_cast<float>(dpi) / 96.0f;

    // Add each app to the list
    int itemIndex = 0;
    for (const auto& app : installedApps_) {
        // Add icon to image list - using DPI-aware icon if possible
        wil::shared_hicon icon = app->icon();

        // For high DPI, we'll extract the icon directly using a better size
        if (dpiScale > 1.0f) {
            // Try to extract a larger icon from the executable file
            std::filesystem::path exePath = app->path();
            if (std::filesystem::exists(exePath)) {
                int scaledSize = static_cast<int>(32 * dpiScale);
                HICON hIcon = nullptr;

                // Use ExtractIconEx to get a better size icon
                if (ExtractIconEx(exePath.c_str(), 0, &hIcon, nullptr, 1) > 0 && hIcon) {
                    // Replace the icon with this higher resolution one
                    icon.reset(hIcon);
                }
            }
        }

        int imageIndex = ImageList_AddIcon(ListView_GetImageList(listView, LVSIL_SMALL), icon.get());

        // Add item to list view
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT | LVIF_IMAGE;
        item.iItem = itemIndex++;
        item.iSubItem = 0;
        item.iImage = imageIndex;

        std::wstring name = app->name();
        item.pszText = const_cast<LPWSTR>(name.c_str());

        ListView_InsertItem(listView, &item);
    }

    // Set full row select style and disable grid lines for a cleaner look
    DWORD exStyle = ListView_GetExtendedListViewStyle(listView);
    exStyle |= LVS_EX_FULLROWSELECT;
    ListView_SetExtendedListViewStyle(listView, exStyle);
}

void NewShortcutDialog::handleOkButton(HWND hwnd) {
    if (!validateInputs(hwnd)) {
        return;
    }

    createShortcut(hwnd);
    EndDialog(hwnd, IDOK);
}

bool NewShortcutDialog::validateInputs(HWND hwnd) {
    bool isApplicationRadioChecked = (IsDlgButtonChecked(hwnd, IDC_INSTALLED_APPLICATION_RADIO) == BST_CHECKED);

    if (isApplicationRadioChecked) {
        // Verify that an application is selected
        HWND listView = GetDlgItem(hwnd, IDC_APPLICATIONS_LIST);
        int selectedIndex = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

        if (selectedIndex == -1) {
            MessageBox(hwnd, L"Please select an application.", L"New Shortcut", MB_OK | MB_ICONWARNING);
            return false;
        }
    } else {
        // Verify that a path is entered
        wchar_t path[MAX_PATH] = { 0 };
        GetDlgItemText(hwnd, IDC_PATH_EDIT, path, MAX_PATH);

        if (wcslen(path) == 0) {
            MessageBox(hwnd, L"Please enter a path or browse for a file.", L"New Shortcut", MB_OK | MB_ICONWARNING);
            return false;
        }
    }

    return true;
}

void NewShortcutDialog::createShortcut(HWND hwnd) {
    bool isApplicationRadioChecked = (IsDlgButtonChecked(hwnd, IDC_INSTALLED_APPLICATION_RADIO) == BST_CHECKED);

    try {
        if (isApplicationRadioChecked) {
            // Create shortcut to selected application
            HWND listView = GetDlgItem(hwnd, IDC_APPLICATIONS_LIST);
            int selectedIndex = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

            if (selectedIndex >= 0 && selectedIndex < static_cast<int>(installedApps_.size())) {
                auto selectedApp = installedApps_[selectedIndex];
                std::filesystem::path targetPath = selectedApp->path();
                std::filesystem::path lnkPath = folder_ / (selectedApp->name() + L".lnk");
                shortcutFactory_->create(lnkPath, targetPath);
            }
        } else {
            // Create shortcut to specified path
            wchar_t path[MAX_PATH] = { 0 };
            GetDlgItemText(hwnd, IDC_PATH_EDIT, path, MAX_PATH);

            std::filesystem::path targetPath(path);
            std::wstring filename = targetPath.filename().wstring();
            std::filesystem::path lnkPath = folder_ / (filename + L".lnk");
            shortcutFactory_->create(lnkPath, targetPath);
        }
    } catch (const std::exception& e) {
        std::wstring errorMsg = L"Failed to create shortcut: " + std::wstring(e.what(), e.what() + strlen(e.what()));
        MessageBox(hwnd, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
    }
}

void NewShortcutDialog::handleBrowseButton(HWND hwnd) {
    selectPathRadio(hwnd);

    wchar_t filePath[MAX_PATH] = { 0 };

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) {
        SetDlgItemText(hwnd, IDC_PATH_EDIT, filePath);
    }
}

void NewShortcutDialog::selectApplicationRadio(HWND hwnd) {
    CheckDlgButton(hwnd, IDC_INSTALLED_APPLICATION_RADIO, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_PATH_RADIO, BST_UNCHECKED);
}

void NewShortcutDialog::selectPathRadio(HWND hwnd) {
    CheckDlgButton(hwnd, IDC_INSTALLED_APPLICATION_RADIO, BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_PATH_RADIO, BST_CHECKED);
}
