#pragma once

#include "libprogman/pch.h"

namespace libprogman {

class Shortcut;

class ShortcutFactory {
   public:
    void create(std::filesystem::path lnkFilePath, std::filesystem::path targetPath);
    std::shared_ptr<Shortcut> open(std::filesystem::path lnkFilePath, std::filesystem::file_time_type lastWriteTime);

   private:
    wil::shared_hicon loadIcon(IShellLink* shellLink);
};

}  // namespace libprogman
