#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutFolder.h"

namespace progman {

class MinimizedFolderListControl {
   public:
    MinimizedFolderListControl(HINSTANCE instance, HWND parent, std::function<void(std::wstring)> onRestore);
    ~MinimizedFolderListControl();
    MinimizedFolderListControl(const MinimizedFolderListControl&) = delete;
    MinimizedFolderListControl& operator=(const MinimizedFolderListControl&) = delete;
    MinimizedFolderListControl(MinimizedFolderListControl&&) = delete;
    MinimizedFolderListControl& operator=(MinimizedFolderListControl&&) = delete;

    HWND hwnd() const;
    void addMinimizedFolder(std::wstring name);
    void removeMinimizedFolder(const std::wstring& name);
    int autoSize(HWND mdiClient);

    // Restore a minimized folder window
    bool restoreMinimizedFolder(const std::wstring& folderName, bool maximize = false);

    // Getter/setter for splitter height
    int getSplitterPosition() const { return controlHeight_; }
    void setSplitterPosition(int height);

    // Set callback for folder rename
    void setOnRenameCallback(std::function<void(const std::wstring&, const std::wstring&)> callback) {
        onRename_ = std::move(callback);
    }

    // Set callback for folder delete
    void setOnDeleteCallback(std::function<void(const std::wstring&)> callback) { onDelete_ = std::move(callback); }

    // Check if a minimized folder is selected and the control has focus
    bool hasSelectedItemAndFocus() const;

    // Get the name of the selected minimized folder
    std::wstring getSelectedFolderName() const;

    // Start editing the selected folder name
    void startEditingSelectedFolder();

   private:
    HWND window_ = nullptr;
    HWND listView_ = nullptr;
    std::function<void(std::wstring)> onRestore_;
    std::function<void(const std::wstring&, const std::wstring&)> onRename_;
    std::function<void(const std::wstring&)> onDelete_;

    // Splitter-related members
    int controlHeight_ = 0;
    int splitterHeight_ = 0;
    bool isSplitterHover_ = false;
    bool isDragging_ = false;
    int dragStartY_ = 0;
    POINT dragStartScreen_ = { 0, 0 };  // Store initial drag position in screen coordinates
    int initialHeight_ = 0;

    // Track if mouse tracking is enabled
    bool isTrackingMouse_ = false;

    // Initialize mouse tracking for hover effects
    void initMouseTracking();

    // Check if a point is within the splitter area
    bool isPointInSplitter(int y) const;

    // Handle splitter painting
    void paintSplitter(HDC hdc);

    friend LRESULT CALLBACK MinimizedFolderListControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    friend LRESULT CALLBACK
    ListViewSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

}  // namespace progman
