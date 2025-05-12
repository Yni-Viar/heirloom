#include "bookmark.h"
#include "stringconstants.h"
#include "resource.h"
#include <ShlObj.h>
#include <CommCtrl.h>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt>

namespace fs = std::filesystem;

BookmarkList& BookmarkList::instance() {
    static BookmarkList instance{};
    return instance;
}

Bookmark::Bookmark(std::wstring name, std::wstring path) : _name(std::move(name)), _path(std::move(path)) {}

static std::wstring GetBookmarksFilePath() {
    WCHAR appDataPath[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        return L"";
    }

    std::wstring bookmarksPath = appDataPath;
    bookmarksPath += kRoamINIPath;
    bookmarksPath += L"\\bookmarks.cfg";

    return bookmarksPath;
}

// Utility function to convert between UTF-16 (std::wstring) and UTF-8 (for file I/O)
static std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();

    // Determine required buffer size
    int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);

    // Allocate buffer of required size
    std::string strTo(size_needed, 0);

    // Perform the conversion
    WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);

    return strTo;
}

// Utility function to convert from UTF-8 to UTF-16 (std::wstring)
static std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty())
        return std::wstring();

    // Determine required buffer size
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);

    // Allocate buffer of required size
    std::wstring wstrTo(size_needed, 0);

    // Perform the conversion
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), &wstrTo[0], size_needed);

    return wstrTo;
}

std::vector<const Bookmark*> BookmarkList::read() {
    std::vector<const Bookmark*> bookmarks;
    std::wstring filePath = GetBookmarksFilePath();

    // Ensure directory exists
    std::wstring dirPath = filePath.substr(0, filePath.find_last_of(L"\\"));
    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }

    // Check if file exists, if not return empty list
    if (!fs::exists(filePath)) {
        return bookmarks;
    }

    // Open the file with UTF-8 encoding
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return bookmarks;
    }

    // Read the file content
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Convert to wide string for processing
        std::wstring wideLine = Utf8ToWide(line);

        // Find the first equals sign
        size_t equalsPos = wideLine.find(L'=');
        if (equalsPos == std::wstring::npos) {
            continue;  // Skip invalid lines
        }

        // Extract name and path
        std::wstring name = wideLine.substr(0, equalsPos);
        std::wstring path = wideLine.substr(equalsPos + 1);

        // Create new bookmark
        Bookmark* newBookmark = new Bookmark(name, path);
        bookmarks.push_back(newBookmark);
    }

    file.close();

    // Sort bookmarks by name, case insensitive
    std::sort(bookmarks.begin(), bookmarks.end(), [](const Bookmark* a, const Bookmark* b) {
        // Create lowercase copies of the strings for comparison
        std::wstring nameA = a->name();
        std::wstring nameB = b->name();

        // Convert both strings to lowercase
        std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::towlower);
        std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::towlower);

        return nameA < nameB;
    });

    return bookmarks;
}

void BookmarkList::add(const std::wstring& name, const std::wstring& path) {
    // Validate the bookmark name and path
    if (name.empty()) {
        throw std::invalid_argument("Bookmark name cannot be empty.");
    }
    if (path.empty()) {
        throw std::invalid_argument("Bookmark path cannot be empty.");
    }
    if (name.find(L'=') != std::wstring::npos) {
        throw std::invalid_argument("Bookmark name cannot contain the '=' character.");
    }

    // Get path to the file
    std::wstring filePath = GetBookmarksFilePath();
    if (filePath.empty()) {
        throw std::runtime_error("Failed to get bookmarks file path.");
    }

    // Ensure directory exists
    std::wstring dirPath = filePath.substr(0, filePath.find_last_of(L"\\"));
    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }

    // Check for duplicate name
    auto existingBookmarks = read();
    for (const auto& bookmark : existingBookmarks) {
        if (bookmark->name() == name) {
            // Clean up memory before throwing
            for (auto bookmark : existingBookmarks) {
                delete bookmark;
            }
            throw std::invalid_argument("A bookmark with this name already exists.");
        }
    }

    // Clean up memory
    for (auto bookmark : existingBookmarks) {
        delete bookmark;
    }

    // Open file in append mode with UTF-8 encoding
    std::ofstream file(filePath, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open bookmarks file for writing.");
    }

    // Construct the new line
    std::wstring newLine = name + L"=" + path;

    // Convert to UTF-8 for file writing
    std::string utf8Line = WideToUtf8(newLine);

    // Write the new bookmark line
    file << utf8Line << std::endl;
    file.close();
}

void BookmarkList::remove(const std::wstring& name) {
    // Read existing bookmarks
    auto bookmarks = read();

    // Find the bookmark to remove
    bool found = false;
    for (const auto& bookmark : bookmarks) {
        if (bookmark->name() == name) {
            found = true;
            break;
        }
    }

    // If not found, clean up and return
    if (!found) {
        for (auto bookmark : bookmarks) {
            delete bookmark;
        }
        return;
    }

    // Get path to the file
    std::wstring filePath = GetBookmarksFilePath();
    if (filePath.empty()) {
        // Clean up before throwing
        for (auto bookmark : bookmarks) {
            delete bookmark;
        }
        throw std::runtime_error("Failed to get bookmarks file path.");
    }

    // Open temporary file for writing
    std::wstring tempFilePath = filePath + L".tmp";
    std::ofstream tempFile(tempFilePath, std::ios::out | std::ios::binary);
    if (!tempFile.is_open()) {
        // Clean up before throwing
        for (auto bookmark : bookmarks) {
            delete bookmark;
        }
        throw std::runtime_error("Failed to create temporary file for bookmarks.");
    }

    // Write all bookmarks except the one to remove
    for (const auto& bookmark : bookmarks) {
        if (bookmark->name() != name) {
            std::wstring line = bookmark->name() + L"=" + bookmark->path();
            tempFile << WideToUtf8(line) << std::endl;
        }
    }

    tempFile.close();

    // Clean up memory
    for (auto bookmark : bookmarks) {
        delete bookmark;
    }

    // Replace original file with temporary file
    try {
        fs::rename(tempFilePath, filePath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to update bookmarks file: ") + e.what());
    }
}

void BookmarkList::rename(const std::wstring& oldName, const std::wstring& newName) {
    // Validate the new bookmark name
    if (newName.empty()) {
        throw std::invalid_argument("New bookmark name cannot be empty.");
    }
    if (newName.find(L'=') != std::wstring::npos) {
        throw std::invalid_argument("Bookmark name cannot contain the '=' character.");
    }

    // Read existing bookmarks
    auto bookmarks = read();

    // Find the bookmark to rename and check for duplicate names
    bool foundOld = false;
    std::wstring pathToKeep;

    for (const auto& bookmark : bookmarks) {
        // Check for duplicate name
        if (bookmark->name() == newName) {
            // Clean up memory before throwing
            for (auto bookmark : bookmarks) {
                delete bookmark;
            }
            throw std::invalid_argument("A bookmark with this name already exists.");
        }

        // Find the bookmark to rename
        if (bookmark->name() == oldName) {
            foundOld = true;
            pathToKeep = bookmark->path();
        }
    }

    // If not found, clean up and throw
    if (!foundOld) {
        for (auto bookmark : bookmarks) {
            delete bookmark;
        }
        throw std::invalid_argument("The bookmark to rename does not exist.");
    }

    // Clean up memory
    for (auto bookmark : bookmarks) {
        delete bookmark;
    }

    // Get path to the file
    std::wstring filePath = GetBookmarksFilePath();
    if (filePath.empty()) {
        throw std::runtime_error("Failed to get bookmarks file path.");
    }

    // Open temporary file for writing
    std::wstring tempFilePath = filePath + L".tmp";
    std::ofstream tempFile(tempFilePath, std::ios::out | std::ios::binary);
    if (!tempFile.is_open()) {
        throw std::runtime_error("Failed to create temporary file for bookmarks.");
    }

    // Reopen the original file for reading
    std::ifstream origFile(filePath, std::ios::in | std::ios::binary);
    if (!origFile.is_open()) {
        tempFile.close();
        throw std::runtime_error("Failed to open bookmarks file for reading.");
    }

    // Process each line
    std::string line;
    while (std::getline(origFile, line)) {
        std::wstring wideLine = Utf8ToWide(line);
        size_t equalsPos = wideLine.find(L'=');

        if (equalsPos != std::wstring::npos) {
            std::wstring name = wideLine.substr(0, equalsPos);
            std::wstring path = wideLine.substr(equalsPos + 1);

            // Replace the name if it matches oldName
            if (name == oldName) {
                std::wstring newLine = newName + L"=" + path;
                tempFile << WideToUtf8(newLine) << std::endl;
            } else {
                tempFile << line << std::endl;
            }
        } else {
            // Pass through invalid lines
            tempFile << line << std::endl;
        }
    }

    origFile.close();
    tempFile.close();

    // Replace original file with temporary file
    try {
        fs::rename(tempFilePath, filePath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to update bookmarks file: ") + e.what());
    }
}

EditBookmarkDialog::EditBookmarkDialog(std::wstring path, std::wstring name, bool isNew)
    : _path(std::move(path)), _name(std::move(name)), _originalName(_name), _isNew(isNew) {}

BOOL EditBookmarkDialog::onInitDialog(HWND hDlg) {
    // Set the target path text
    SetDlgItemTextW(hDlg, IDC_BOOKMARK_TARGET, _path.c_str());

    // Set the bookmark name
    SetDlgItemTextW(hDlg, IDC_BOOKMARK_NAME, _name.c_str());

    // Select all text in the name field
    SendDlgItemMessageW(hDlg, IDC_BOOKMARK_NAME, EM_SETSEL, 0, -1);

    // Set focus to the name field
    SetFocus(GetDlgItem(hDlg, IDC_BOOKMARK_NAME));
    return FALSE;  // Return FALSE since we set focus manually
}

BOOL EditBookmarkDialog::onOk(HWND hDlg) {
    // Get the bookmark name from the edit control
    WCHAR name[MAX_PATH];
    GetDlgItemTextW(hDlg, IDC_BOOKMARK_NAME, name, MAX_PATH);

    // Check if the name is empty
    if (name[0] == L'\0') {
        MessageBoxW(hDlg, L"Bookmark name cannot be empty.", L"Edit Bookmark", MB_OK | MB_ICONWARNING);
        return TRUE;
    }

    try {
        if (_isNew) {
            // Add new bookmark
            BookmarkList::instance().add(name, _path);
        } else {
            // Rename existing bookmark if name has changed
            if (_originalName != name) {
                BookmarkList::instance().rename(_originalName, name);
            }
        }
        // Success - close the dialog
        EndDialog(hDlg, IDOK);
    } catch (const std::invalid_argument& e) {
        // Convert error message to wide string
        std::string errorMsg = e.what();
        std::wstring wideErrorMsg(errorMsg.begin(), errorMsg.end());

        // Show the error message
        MessageBoxW(hDlg, wideErrorMsg.c_str(), L"Invalid Bookmark Name", MB_OK | MB_ICONWARNING);

        // Keep dialog open
        return TRUE;
    } catch (const std::exception& e) {
        // Convert error message to wide string
        std::string errorMsg = e.what();
        std::wstring wideErrorMsg(errorMsg.begin(), errorMsg.end());

        // Show the error message
        MessageBoxW(hDlg, wideErrorMsg.c_str(), L"Failed to Update Bookmark", MB_OK | MB_ICONERROR);

        // Close the dialog since this is likely a more serious error
        EndDialog(hDlg, IDCANCEL);
    }
    return TRUE;
}

BOOL EditBookmarkDialog::onCommand(HWND hDlg, WPARAM wParam) {
    switch (LOWORD(wParam)) {
        case IDOK:
            return onOk(hDlg);

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    return FALSE;
}

// Dialog procedure for the Edit Bookmark dialog
INT_PTR CALLBACK EditBookmarkDialog::EditBookmarkDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    EditBookmarkDialog* dlg = nullptr;

    if (message == WM_INITDIALOG) {
        // Store the pointer to our object
        dlg = reinterpret_cast<EditBookmarkDialog*>(lParam);
        SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(dlg));
        return dlg->onInitDialog(hDlg);
    }

    // Retrieve the pointer to our dialog object
    dlg = reinterpret_cast<EditBookmarkDialog*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
    if (!dlg) {
        return FALSE;
    }

    switch (message) {
        case WM_COMMAND:
            return dlg->onCommand(hDlg, wParam);
    }

    return FALSE;
}

void EditBookmarkDialog::showDialog(HWND owner) {
    DialogBoxParamW(
        GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_BOOKMARK_EDIT), owner, EditBookmarkDialogProc,
        reinterpret_cast<LPARAM>(this));
}

ManageBookmarksDialog::ManageBookmarksDialog() {}

void ManageBookmarksDialog::showDialog(HWND owner) {
    DialogBoxParamW(
        GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_BOOKMARKS_MANAGE), owner, ManageBookmarksDialogProc,
        reinterpret_cast<LPARAM>(this));
}

BOOL ManageBookmarksDialog::onInitDialog(HWND hDlg) {
    // Initialize the ListView
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);

    // Set the ListView to single selection mode
    LONG style = GetWindowLong(hListView, GWL_STYLE);
    SetWindowLong(hListView, GWL_STYLE, style | LVS_SINGLESEL);

    // Set extended ListView style for full row selection
    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);

    // Add columns to the ListView
    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;

    // Name column
    lvc.iSubItem = 0;
    lvc.pszText = const_cast<LPWSTR>(L"Name");
    ListView_InsertColumn(hListView, 0, &lvc);

    // Path column
    lvc.iSubItem = 1;
    lvc.pszText = const_cast<LPWSTR>(L"Path");
    ListView_InsertColumn(hListView, 1, &lvc);

    // Resize columns to fill the entire width
    resizeColumns(hListView);

    // Populate ListView with bookmarks
    populateListView(hDlg);

    // Disable edit/delete buttons initially (no selection)
    updateButtonState(hDlg);

    return TRUE;
}

void ManageBookmarksDialog::populateListView(HWND hDlg) {
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);

    // Clear the ListView
    ListView_DeleteAllItems(hListView);

    // Load bookmarks from the BookmarkList
    auto bookmarkPtrs = BookmarkList::instance().read();

    // Add each bookmark to the ListView
    LVITEMW lvi = { 0 };
    lvi.mask = LVIF_TEXT | LVIF_PARAM;

    for (size_t i = 0; i < bookmarkPtrs.size(); i++) {
        lvi.iItem = static_cast<int>(i);
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPWSTR>(bookmarkPtrs[i]->name().c_str());
        lvi.lParam = static_cast<LPARAM>(i);  // Store index for quick access

        int insertedIndex = ListView_InsertItem(hListView, &lvi);

        // Add the path as subitem
        ListView_SetItemText(hListView, insertedIndex, 1, const_cast<LPWSTR>(bookmarkPtrs[i]->path().c_str()));
    }

    // Clean up memory allocated by BookmarkList::read()
    for (const auto& bookmarkPtr : bookmarkPtrs) {
        delete bookmarkPtr;
    }
}

void ManageBookmarksDialog::updateButtonState(HWND hDlg) {
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);
    int selectedIndex = getSelectedIndex(hListView);

    BOOL enableButtons = (selectedIndex != -1);

    EnableWindow(GetDlgItem(hDlg, IDC_BOOKMARK_EDIT), enableButtons);
    EnableWindow(GetDlgItem(hDlg, IDC_BOOKMARK_DELETE), enableButtons);
}

int ManageBookmarksDialog::getSelectedIndex(HWND hListView) {
    return ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
}

// Helper to get item parameter from ListView
LPARAM GetListViewItemParam(HWND hListView, int index) {
    LVITEMW lvi = { 0 };
    lvi.mask = LVIF_PARAM;
    lvi.iItem = index;
    ListView_GetItem(hListView, &lvi);
    return lvi.lParam;
}

void ManageBookmarksDialog::editSelectedBookmark(HWND hDlg) {
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);
    int selectedIndex = getSelectedIndex(hListView);

    if (selectedIndex != -1) {
        // Get the bookmark data
        WCHAR name[MAX_PATH] = { 0 };
        WCHAR path[MAX_PATH] = { 0 };

        ListView_GetItemText(hListView, selectedIndex, 0, name, MAX_PATH);
        ListView_GetItemText(hListView, selectedIndex, 1, path, MAX_PATH);

        // Create edit dialog with the selected bookmark's data
        EditBookmarkDialog editDlg(path, name, false);
        editDlg.showDialog(hDlg);

        // Refresh the list after editing
        populateListView(hDlg);
    }
}

void ManageBookmarksDialog::deleteSelectedBookmark(HWND hDlg) {
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);
    int selectedIndex = getSelectedIndex(hListView);

    if (selectedIndex != -1) {
        // Get the bookmark name
        WCHAR name[MAX_PATH] = { 0 };
        ListView_GetItemText(hListView, selectedIndex, 0, name, MAX_PATH);

        // Confirm deletion
        std::wstring message = L"Are you sure you want to delete the bookmark '" + std::wstring(name) + L"'?";
        if (MessageBoxW(hDlg, message.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            // Delete the bookmark immediately
            try {
                BookmarkList::instance().remove(name);

                // Refresh the ListView
                populateListView(hDlg);

                // Update UI
                updateButtonState(hDlg);
            } catch (const std::exception& e) {
                // Convert error message to wide string
                std::string errorMsg = e.what();
                std::wstring wideErrorMsg(errorMsg.begin(), errorMsg.end());

                // Show the error message
                MessageBoxW(hDlg, wideErrorMsg.c_str(), L"Failed to Delete Bookmark", MB_OK | MB_ICONERROR);
            }
        }
    }
}

BOOL ManageBookmarksDialog::onClose(HWND hDlg) {
    EndDialog(hDlg, IDOK);
    return TRUE;
}

BOOL ManageBookmarksDialog::onCommand(HWND hDlg, WPARAM wParam) {
    switch (LOWORD(wParam)) {
        case IDOK:
            return onClose(hDlg);

        case IDC_BOOKMARK_EDIT:
            editSelectedBookmark(hDlg);
            return TRUE;

        case IDC_BOOKMARK_DELETE:
            deleteSelectedBookmark(hDlg);
            return TRUE;
    }

    return FALSE;
}

BOOL ManageBookmarksDialog::onNotify(HWND hDlg, WPARAM wParam, LPARAM lParam) {
    NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

    switch (nmhdr->code) {
        case LVN_ITEMCHANGED:
            if (nmhdr->idFrom == IDC_BOOKMARKS_LIST) {
                updateButtonState(hDlg);
                return TRUE;
            }
            break;
        case NM_DBLCLK:
            if (nmhdr->idFrom == IDC_BOOKMARKS_LIST) {
                editSelectedBookmark(hDlg);
                return TRUE;
            }
            break;
    }

    return FALSE;
}

void ManageBookmarksDialog::resizeColumns(HWND hListView) {
    // Get the width of the ListView
    RECT rc;
    GetClientRect(hListView, &rc);
    int totalWidth = rc.right - rc.left;

    // Account for vertical scrollbar if present
    if (GetWindowLong(hListView, GWL_STYLE) & WS_VSCROLL) {
        totalWidth -= GetSystemMetrics(SM_CXVSCROLL);
    }

    // Set name column to 33% and path column to 67% of the total width
    int nameColumnWidth = totalWidth / 3;
    int pathColumnWidth = totalWidth - nameColumnWidth;
    ListView_SetColumnWidth(hListView, 0, nameColumnWidth);
    ListView_SetColumnWidth(hListView, 1, pathColumnWidth);
}

BOOL ManageBookmarksDialog::onSize(HWND hDlg) {
    // Resize the columns when the dialog is resized
    HWND hListView = GetDlgItem(hDlg, IDC_BOOKMARKS_LIST);
    if (hListView) {
        resizeColumns(hListView);
    }
    return TRUE;
}

INT_PTR CALLBACK
ManageBookmarksDialog::ManageBookmarksDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    ManageBookmarksDialog* dlg = nullptr;

    if (message == WM_INITDIALOG) {
        // Store the pointer to our object
        dlg = reinterpret_cast<ManageBookmarksDialog*>(lParam);
        SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(dlg));
        return dlg->onInitDialog(hDlg);
    }

    // Retrieve the pointer to our dialog object
    dlg = reinterpret_cast<ManageBookmarksDialog*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
    if (!dlg) {
        return FALSE;
    }

    switch (message) {
        case WM_COMMAND:
            return dlg->onCommand(hDlg, wParam);

        case WM_NOTIFY:
            return dlg->onNotify(hDlg, wParam, lParam);

        case WM_SIZE:
            return dlg->onSize(hDlg);
    }

    return FALSE;
}
