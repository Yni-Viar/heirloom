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

    // Gets the icon.
    wil::shared_hicon getIcon();

    // Shows the shell properties window.
    void showPropertiesWindow();

    // Gets the display name (lnk path with no directory or file extension).
    std::wstring getDisplayName();

    // Launches the shortcut.
    void launch();

   private:
    // Loads the icon for the shortcut
    void loadIcon();

    std::wstring lnkFilePath_;
    wil::shared_hicon icon_;

    // Non-owning pointers - these are managed by the caller
    IShellLink* shellLink_ = nullptr;
    IPersistFile* persistFile_ = nullptr;
};

}  // namespace libprogram
