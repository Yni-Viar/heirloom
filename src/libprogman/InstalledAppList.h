#pragma once

#include "libprogman/pch.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/cancel.h"

namespace libprogman {

// Automatically detects a list of installed applications.
// The following two filesystem paths are scanned recursively for .lnk files:
// Then the shortcuts are loaded with ShortcutFactory.
class InstalledAppList {
   public:
    InstalledAppList(ShortcutFactory* shortcutFactory, immer::vector<std::filesystem::path> foldersToScan);

    // Updates apps_ to account for newly installed/updated applications and then returns the complete list.
    // Last write time is used to decide whether a .lnk file we've loaded previously needs to be reloaded.
    // Holds the mutex the whole time.
    immer::vector<std::shared_ptr<Shortcut>> apps(CancellationToken cancel);

   private:
    ShortcutFactory* shortcutFactory_;
    immer::vector<std::filesystem::path> foldersToScan_;
    std::mutex mutex_;  // Protects apps_ and the scanning process.
    immer::vector<std::shared_ptr<Shortcut>> apps_;
};

}  // namespace libprogman
