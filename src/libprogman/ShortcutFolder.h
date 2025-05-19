#pragma once

#include "libprogman/pch.h"
#include "libprogman/Shortcut.h"

namespace libprogman {

class ShortcutFolder {
   public:
    ShortcutFolder(std::filesystem::path path, immer::map<std::wstring, std::shared_ptr<Shortcut>> shortcuts);
    const std::wstring& name() const noexcept;
    const std::filesystem::path& path() const noexcept;
    const immer::map<std::wstring, std::shared_ptr<Shortcut>>& shortcuts() const noexcept;
    std::shared_ptr<Shortcut> shortcut(std::wstring name) const;
    std::shared_ptr<Shortcut> shortcutOrNull(std::wstring name) const noexcept;

   private:
    std::wstring name_;
    std::filesystem::path path_;
    immer::map<std::wstring, std::shared_ptr<Shortcut>> shortcuts_;
};

}  // namespace libprogman
