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
    int autoSize(HWND mdiClient) const;

   private:
    HWND window_ = nullptr;
    HWND listView_ = nullptr;
    std::function<void(std::wstring)> onRestore_;
    friend LRESULT CALLBACK MinimizedFolderListControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

}  // namespace progman
