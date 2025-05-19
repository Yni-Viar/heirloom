#include "libprogman/pch.h"
#include "libprogman/ShortcutFolder.h"
#include "libprogman/Error.h"

namespace libprogman {

ShortcutFolder::ShortcutFolder(
    std::filesystem::path path,
    immer::map<std::wstring, std::shared_ptr<Shortcut>> shortcuts)
    : path_(std::move(path)), shortcuts_(std::move(shortcuts)) {
    // Extract the folder name from the path
    name_ = path_.filename().wstring();
}

const std::wstring& ShortcutFolder::name() const noexcept {
    return name_;
}

const std::filesystem::path& ShortcutFolder::path() const noexcept {
    return path_;
}

const immer::map<std::wstring, std::shared_ptr<Shortcut>>& ShortcutFolder::shortcuts() const noexcept {
    return shortcuts_;
}

std::shared_ptr<Shortcut> ShortcutFolder::shortcut(std::wstring name) const {
    auto ptr = shortcuts_.find(name);
    if (ptr == nullptr) {
        throw Error(ErrorCode::kFileNotFound, L"Shortcut not found: " + name);
    }
    return *ptr;
}

std::shared_ptr<Shortcut> ShortcutFolder::shortcutOrNull(std::wstring name) const noexcept {
    auto ptr = shortcuts_.find(name);
    if (ptr == nullptr) {
        return nullptr;
    }
    return *ptr;
}

}  // namespace libprogman
