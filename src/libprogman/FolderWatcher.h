#pragma once

#include "libprogman/pch.h"

namespace libprogman {

// Monitors a folder for file and subfolder changes (new, modified, deleted).
// Calls a simple callback when any change is detected.
// Stops monitoring when the object is destroyed.
class FolderWatcher {
   public:
    FolderWatcher(
        std::filesystem::path folderPath,
        std::function<void()> onChange,
        std::function<void(std::wstring)> onError = [](std::wstring) {});
    ~FolderWatcher() noexcept;

   private:
    void WatchThreadProc();

    std::filesystem::path folderPath_;
    std::function<void()> onChange_;
    std::function<void(std::wstring)> onError_;
    std::atomic<bool> running_;
    std::thread watchThread_;
};

}  // namespace libprogman
