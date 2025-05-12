#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

// Immutable object corresponding to a bookmark entry in the bookmarks.cfg file.
class Bookmark {
   public:
    Bookmark(std::wstring name, std::wstring path);
    inline const std::wstring& name() const { return _name; }
    inline const std::wstring& path() const { return _path; }

   private:
    std::wstring _name, _path;
};

// Manages a list of bookmarks on disk. Each bookmark is stored in the format "name=path"
// in the bookmarks.cfg file, using UTF-8 encoding to support full Unicode including emoji.
// The name cannot contain '=' character, but the path can contain any characters.
class BookmarkList {
   public:
    static BookmarkList& instance();

    // Reads the bookmarks.cfg file.
    std::vector<const Bookmark*> read();

    // Creates a new bookmark in the bookmarks.cfg file.
    void add(const std::wstring& name, const std::wstring& path);

    // Deletes a bookmark from the bookmarks.cfg file.
    void remove(const std::wstring& name);

    // Renames a bookmark in the bookmarks.cfg file.
    void rename(const std::wstring& oldName, const std::wstring& newName);
};

// Edit/Add Bookmark dialog. We show the passed-in path. The user can modify the name.
// On OK, we create or rename the bookmark based on isNew flag and close.
class EditBookmarkDialog {
   public:
    EditBookmarkDialog(std::wstring path, std::wstring name, bool isNew);
    void showDialog(HWND owner);

   private:
    // Handler methods for dialog messages
    BOOL onInitDialog(HWND hDlg);
    BOOL onCommand(HWND hDlg, WPARAM wParam);
    BOOL onOk(HWND hDlg);

    static INT_PTR CALLBACK EditBookmarkDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    std::wstring _path;
    std::wstring _name;
    std::wstring _originalName;
    bool _isNew;
};

// Manage Bookmarks dialog. We show all the bookmarks in a ListView, report mode, with two columns: name and path.
// Single selection list.
// Edit/Delete buttons are enabled or disabled based on list selection.
// Edit button opens the EditBookmarkDialog.
// Delete button shows a message box prompt for confirmation, then removes it from the list.
// All changes are in memory until OK is clicked.
class ManageBookmarksDialog {
   public:
    ManageBookmarksDialog();
    void showDialog(HWND owner);

   private:
    // Handler methods for dialog messages
    BOOL onInitDialog(HWND hDlg);
    BOOL onCommand(HWND hDlg, WPARAM wParam);
    BOOL onClose(HWND hDlg);
    BOOL onNotify(HWND hDlg, WPARAM wParam, LPARAM lParam);
    BOOL onSize(HWND hDlg);

    // Helper methods
    void populateListView(HWND hDlg);
    void updateButtonState(HWND hDlg);
    void editSelectedBookmark(HWND hDlg);
    void deleteSelectedBookmark(HWND hDlg);
    int getSelectedIndex(HWND hListView);
    void resizeColumns(HWND hListView);

    static INT_PTR CALLBACK ManageBookmarksDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // In-memory bookmarks until changes are saved
    std::vector<std::unique_ptr<Bookmark>> _bookmarks;
    std::vector<std::wstring> _deletedBookmarks;
    bool _isDirty;
};