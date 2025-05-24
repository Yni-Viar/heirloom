#include "progman/pch.h"
#include "progman/NewFolderDialog.h"
#include "progman/resource.h"
#include "libprogman/window_data.h"

namespace progman {

NewFolderDialog::NewFolderDialog(HWND parentWindow, HINSTANCE hInstance, libprogman::ShortcutManager* shortcutManager)
    : parentWindow_(parentWindow), hInstance_(hInstance), shortcutManager_(shortcutManager) {}

bool NewFolderDialog::show() {
    // Show the dialog modally
    INT_PTR result = DialogBoxParam(
        hInstance_, MAKEINTRESOURCE(IDD_NEW_FOLDER), parentWindow_, dialogProc, reinterpret_cast<LPARAM>(this));

    // Return true if a folder was created (IDOK)
    return result == IDOK;
}

INT_PTR CALLBACK NewFolderDialog::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    NewFolderDialog* pThis = nullptr;

    if (uMsg == WM_INITDIALOG) {
        // Store the this pointer
        pThis = reinterpret_cast<NewFolderDialog*>(lParam);
        libprogman::setWindowData(hwnd, pThis);
    } else {
        // Retrieve the this pointer
        pThis = libprogman::getWindowData<NewFolderDialog>(hwnd);
    }

    if (pThis) {
        return pThis->handleMessage(hwnd, uMsg, wParam, lParam);
    }

    return FALSE;
}

INT_PTR NewFolderDialog::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG: {
            // Set focus to the name edit control and select all text
            HWND hEdit = GetDlgItem(hwnd, IDC_NAME_EDIT);
            SetFocus(hEdit);

            // Optional: Select all text in the edit control
            SendMessage(hEdit, EM_SETSEL, 0, -1);

            return FALSE;  // We've set focus manually
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    if (validateAndCreateFolder(hwnd)) {
                        EndDialog(hwnd, IDOK);
                    }
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
            }
            break;
    }

    return FALSE;
}

bool NewFolderDialog::validateAndCreateFolder(HWND hwnd) {
    // Get the folder name from the edit control
    WCHAR folderName[MAX_PATH] = { 0 };
    GetDlgItemTextW(hwnd, IDC_NAME_EDIT, folderName, MAX_PATH);

    std::wstring name(folderName);

    // Validate: non-empty
    if (name.empty()) {
        MessageBoxW(hwnd, L"Please enter a folder name.", L"New Folder", MB_OK | MB_ICONEXCLAMATION);
        return false;
    }

    // Validate: not already existing
    if (shortcutManager_->folderOrNull(name) != nullptr) {
        MessageBoxW(hwnd, L"A folder with this name already exists.", L"New Folder", MB_OK | MB_ICONEXCLAMATION);
        return false;
    }

    try {
        // Create the folder
        shortcutManager_->addFolder(name);
        return true;
    } catch (const std::exception&) {
        // Show error message
        MessageBoxW(
            hwnd, std::wstring(L"Failed to create folder: " + std::wstring(name.begin(), name.end())).c_str(), L"Error",
            MB_OK | MB_ICONERROR);
        return false;
    }
}

}  // namespace progman
