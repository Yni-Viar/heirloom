#include "libprogman/pch.h"
#include "libprogman/ShortcutManager.h"
#include "libprogman/Error.h"
#include "libprogman/ShortcutFolder.h"
#include "libprogman/Shortcut.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/string_util.h"

namespace libprogman {

ShortcutManager::ShortcutManager(std::wstring rootPath, ShortcutFactory* shortcutFactory)
    : rootPath_(std::move(rootPath)), folders_(), shortcutFactory_(shortcutFactory) {
    refresh();
}

immer::map<std::wstring, std::shared_ptr<ShortcutFolder>> ShortcutManager::folders() const noexcept {
    return folders_;
}

std::shared_ptr<ShortcutFolder> ShortcutManager::folder(std::wstring name) const {
    auto ptr = folders_.find(name);
    if (ptr == nullptr) {
        throw Error(ErrorCode::kFolderNotFound, L"Folder not found: " + name);
    }
    return *ptr;
}

std::shared_ptr<ShortcutFolder> ShortcutManager::folderOrNull(std::wstring name) const noexcept {
    auto ptr = folders_.find(name);
    if (ptr == nullptr) {
        return nullptr;
    }
    return *ptr;
}

void ShortcutManager::addFolder(std::wstring name) {
    // Check if folder already exists
    if (folders_.find(name) != nullptr) {
        throw Error(ErrorCode::kFolderAlreadyExists, L"Folder already exists: " + name);
    }

    // Create the folder on disk
    std::filesystem::path folderPath = std::filesystem::path(rootPath_) / name;
    std::filesystem::create_directory(folderPath);

    // Refresh to update the in-memory folders_
    refresh();
}

void ShortcutManager::renameFolder(ShortcutFolder* folder, std::wstring newName) {
    // Check if the folder exists in our collection
    auto srcPtr = folders_.find(folder->name());
    if (srcPtr == nullptr) {
        throw Error(ErrorCode::kFolderNotFound, L"Folder not found.");
    }

    // Check if a folder with the new name already exists
    auto dstPtr = folders_.find(newName);
    if (dstPtr != nullptr) {
        throw Error(ErrorCode::kFolderAlreadyExists, L"Folder already exists with name: " + newName);
    }

    // Rename the folder on disk
    std::filesystem::path oldPath{ folder->path() };
    std::filesystem::path newPath = std::filesystem::path(rootPath_) / newName;
    std::error_code ec;
    std::filesystem::rename(oldPath, newPath, ec);
    if (ec) {
        throw Error(
            ErrorCode::kFilesystemOperationFailed, L"Failed to rename folder: " + libprogman::utf8ToWide(ec.message()));
    }

    // Refresh to update the in-memory folders_
    refresh();
}

void ShortcutManager::deleteFolder(ShortcutFolder* folder) {
    // Check if the folder exists in our collection
    auto ptr = folders_.find(folder->name());
    if (ptr == nullptr) {
        throw Error(ErrorCode::kFolderNotFound, L"Folder not found.");
    }

    // Delete the folder from disk
    std::filesystem::path folderPath = std::filesystem::path(rootPath_) / folder->name();
    std::error_code ec;
    std::filesystem::remove_all(folderPath, ec);
    if (ec) {
        throw Error(
            ErrorCode::kFilesystemOperationFailed, L"Failed to delete folder: " + libprogman::utf8ToWide(ec.message()));
    }

    // Refresh to update the in-memory folders_
    refresh();
}

void ShortcutManager::refresh() {
    immer::map_transient<std::wstring, std::shared_ptr<ShortcutFolder>> folders;

    // Walk rootPath_'s subfolders. Ignore files.
    for (const auto& entry : std::filesystem::directory_iterator(rootPath_)) {
        if (entry.is_directory()) {
            auto folder = refreshFolder(entry.path());
            folders.insert({ folder->name(), folder });
        }
    }

    // Cut over to the new map.
    folders_ = folders.persistent();
}

std::shared_ptr<ShortcutFolder> ShortcutManager::refreshFolder(std::filesystem::path folderPath) const {
    immer::map_transient<std::wstring, std::shared_ptr<Shortcut>> shortcuts;

    // We will reuse existing Shortcut objects if possible. Grab them from the existing folder, if we have it.
    const auto* existingFolder = folderOrNull(folderPath.filename().wstring()).get();

    // Walk folderPath's files. Ignore directories.
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            const auto& path = entry.path();

            // Verify .lnk file extension; ignore otherwise.
            if (path.extension() != ".lnk") {
                continue;
            }

            auto shortcut = refreshShortcut(path, entry.last_write_time(), existingFolder);
            shortcuts.insert({ shortcut->name(), shortcut });
        }
    }

    // Create a new ShortcutFolder object.
    return std::make_shared<ShortcutFolder>(std::move(folderPath), shortcuts.persistent());
}

std::shared_ptr<Shortcut> ShortcutManager::refreshShortcut(
    std::filesystem::path shortcutPath,
    std::filesystem::file_time_type lastWriteTime,
    const ShortcutFolder* existingFolder) const {
    // If we have an existing folder, try to reuse one of its shortcuts.
    if (existingFolder != nullptr) {
        auto shortcut = existingFolder->shortcutOrNull(shortcutPath.filename().wstring());
        if (shortcut != nullptr && shortcut->lastWriteTime() >= lastWriteTime) {
            // The file has not changed, so we can reuse the existing shortcut.
            return shortcut;
        }
    }

    // Otherwise, load the shortcut from disk.
    return shortcutFactory_->open(shortcutPath, lastWriteTime);
}

}  // namespace libprogman
