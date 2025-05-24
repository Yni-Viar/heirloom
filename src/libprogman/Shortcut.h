#pragma once

#include "libprogman/pch.h"

namespace libprogman {

// Represents a .lnk file.
class Shortcut {
   public:
    Shortcut(
        std::filesystem::path path,
        wil::shared_hicon icon,
        std::filesystem::file_time_type lastWriteTime) noexcept;
    const std::filesystem::path& path() const noexcept;
    const std::wstring& name() const noexcept;
    wil::shared_hicon icon() const noexcept;
    std::filesystem::file_time_type lastWriteTime() const noexcept;
    void showPropertiesWindow() const;
    void launch() const;
    void deleteFile() const;

   private:
    std::wstring name_;
    std::filesystem::path path_;
    wil::shared_hicon icon_;
    std::filesystem::file_time_type lastWriteTime_;
};

}  // namespace libprogman
