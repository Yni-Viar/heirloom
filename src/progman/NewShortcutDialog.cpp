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
    ListView_SetView(listView, LVS_SMALLICON);

    // Create image list for small icons
    HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, static_cast<int>(installedApps_.size()));
    ListView_SetImageList(listView, imageList, LVSIL_SMALL);

    // Select Application radio by default
    CheckDlgButton(hwnd, IDC_INSTALLED_APPLICATION_RADIO, BST_CHECKED);

    // Populate applications list
    populateApplicationsList(hwnd);
}

void NewShortcutDialog::populateApplicationsList(HWND hwnd) {
    HWND listView = GetDlgItem(hwnd, IDC_APPLICATIONS_LIST);

    // Add each app to the list
    int itemIndex = 0;
    for (const auto& app : installedApps_) {
        // Add icon to image list
        wil::shared_hicon icon = app->icon();
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
