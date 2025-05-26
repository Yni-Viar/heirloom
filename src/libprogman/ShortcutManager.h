#pragma once

#include "libprogman/pch.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/ShortcutFolder.h"
#include "libprogman/Shortcut.h"

namespace libprogman {

// Manages a folder structure inside the specified root path.
// The root folder contains a single level of subfolders, and those subfolders contain shortcuts (.lnk files).
// Folders cannot be nested inside other folders.
class ShortcutManager {
   public:
    // Populates rootPath_ and folders_.
    ShortcutManager(std::wstring rootPath, ShortcutFactory* shortcutFactory);

    // Returns the in-memory folders_.
    immer::map<std::wstring, std::shared_ptr<ShortcutFolder>> folders() const noexcept;

    // Looks up a particular folder by name. Throws Error(kFolderNotFound) if not found.
    std::shared_ptr<ShortcutFolder> folder(std::wstring name) const;

    // Looks up a particular folder by name. Returns nullptr if not found.
    std::shared_ptr<ShortcutFolder> folderOrNull(std::wstring name) const noexcept;

    // Adds a new folder. Throws Error(kFolderAlreadyExists) if a folder with the same name already exists.
    void addFolder(std::wstring name);

    // Renames a folder. Throws Error(kFolderNotFound) if the folder is not found.
    void renameFolder(ShortcutFolder* folder, std::wstring newName);

    // Deletes a folder. Throws Error(kFolderNotFound) if the folder is not found.
    void deleteFolder(ShortcutFolder* folder);

    // Loads changes from disk.
    void refresh();

   private:
    void refreshCore();
    void setupInitialShortcuts();
    std::shared_ptr<ShortcutFolder> refreshFolder(std::filesystem::path folderPath) const;
    std::shared_ptr<Shortcut> refreshShortcut(
        std::filesystem::path shortcutPath,
        std::filesystem::file_time_type lastWriteTime,
        const ShortcutFolder* existingFolder) const;

    std::wstring rootPath_;
    immer::map<std::wstring, std::shared_ptr<ShortcutFolder>> folders_;  // Only updated in refresh()
    ShortcutFactory* shortcutFactory_;
    std::mutex refreshMutex_;
};

}  // namespace libprogman
