#pragma once

#include "libprogman/pch.h"

namespace libprogram {

// Represents a .lnk file.
class Shortcut {
   public:
    // Constructor with dependency injection
    Shortcut(IShellLink* shellLink, IPersistFile* persistFile);
    ~Shortcut();

    // Creates a new .lnk file.
    void initNew(std::wstring lnkFilePath, std::wstring targetPath);

    // Opens an existing .lnk file.
    void initOpen(std::wstring lnkFilePath);

    // Loads the icon.
    wil::unique_hicon loadIcon();

    // Shows the shell properties window.
    void showPropertiesWindow();

    // Gets the display name (lnk path with no directory or file extension).
    std::wstring getDisplayName();

    // Launches the shortcut.
    void launch();

   private:
    std::wstring lnkFilePath_;
    // Non-owning pointers - these are managed by the caller
    IShellLink* shellLink_ = nullptr;
    IPersistFile* persistFile_ = nullptr;
};

}  // namespace libprogram
