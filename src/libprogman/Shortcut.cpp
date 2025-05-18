#include "libprogman/pch.h"
#include "libprogman/Shortcut.h"

namespace libprogram {

Shortcut::Shortcut(IShellLink* shellLink, IPersistFile* persistFile)
    : shellLink_(shellLink), persistFile_(persistFile) {
    // No AddRef needed since we're not taking ownership
}

Shortcut::~Shortcut() = default;

void Shortcut::initNew(std::wstring lnkFilePath, std::wstring targetPath) {
    lnkFilePath_ = std::move(lnkFilePath);

    // Set the target path
    THROW_IF_FAILED(shellLink_->SetPath(targetPath.c_str()));

    // Set working directory to the target path's directory
    std::filesystem::path path(targetPath);
    if (path.has_parent_path()) {
        THROW_IF_FAILED(shellLink_->SetWorkingDirectory(path.parent_path().c_str()));
    }

    // Save the shortcut file
    THROW_IF_FAILED(persistFile_->Save(lnkFilePath_.c_str(), TRUE));
}

void Shortcut::initOpen(std::wstring lnkFilePath) {
    lnkFilePath_ = std::move(lnkFilePath);

    // Load the shortcut file
    THROW_IF_FAILED(persistFile_->Load(lnkFilePath_.c_str(), STGM_READ));
}

wil::unique_hicon Shortcut::loadIcon() {
    wil::unique_hicon hIcon;
    int iconIndex = 0;

    // Get the icon location
    std::wstring iconPath(MAX_PATH, L'\0');
    HRESULT hr = shellLink_->GetIconLocation(iconPath.data(), static_cast<int>(iconPath.size()), &iconIndex);
    if (FAILED(hr)) {
        // If no icon is specified in the shortcut, try to extract from the target
        WCHAR targetPath[MAX_PATH] = { 0 };
        WIN32_FIND_DATA findData = { 0 };
        hr = shellLink_->GetPath(targetPath, MAX_PATH, &findData, SLGP_RAWPATH);
        if (SUCCEEDED(hr)) {
            // Extract icon from target executable
            ExtractIconExW(targetPath, 0, nullptr, &hIcon, 1);
            if (!hIcon) {
                // Use default icon if extraction failed
                ExtractIconExW(L"shell32.dll", 0, nullptr, &hIcon, 1);
            }
        }
    } else {
        // Use the icon specified in the shortcut
        if (!iconPath.empty()) {
            ExtractIconExW(iconPath.c_str(), iconIndex, nullptr, &hIcon, 1);
        }

        // Fallback to default icon
        if (!hIcon) {
            ExtractIconExW(L"shell32.dll", 0, nullptr, &hIcon, 1);
        }
    }

    return hIcon;
}

void Shortcut::showPropertiesWindow() {
    // Use the shell to show the properties dialog
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = lnkFilePath_.c_str();
    sei.lpVerb = L"properties";
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.nShow = SW_SHOW;

    THROW_IF_FAILED(ShellExecuteExW(&sei) ? S_OK : HRESULT_FROM_WIN32(GetLastError()));
}

std::wstring Shortcut::getDisplayName() {
    std::filesystem::path path(lnkFilePath_);
    return path.stem().wstring();
}

void Shortcut::launch() {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = lnkFilePath_.c_str();
    sei.nShow = SW_SHOWNORMAL;

    THROW_IF_FAILED(ShellExecuteExW(&sei) ? S_OK : HRESULT_FROM_WIN32(GetLastError()));
}

}  // namespace libprogram
