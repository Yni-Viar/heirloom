#include "libprogman/pch.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/com_util.h"
#include "libprogman/Shortcut.h"

namespace libprogman {

static wil::com_ptr<IShellLink> newShellLink() {
    wil::com_ptr<IShellLink> shellLink;
    libprogman::throwIfComFailed(
        CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink)),
        "Failed to create the IShellLink COM object.");
    return shellLink;
}

static wil::com_ptr<IPersistFile> newPersistFile(const wil::com_ptr<IShellLink>& shellLink) {
    wil::com_ptr<IPersistFile> persistFile;
    try {
        shellLink.query_to(&persistFile);
    } catch (const std::exception& e) {
        std::string message = "Failed to create the IPersistFile COM object. (";
        message += e.what();
        message += ")";
        throw std::runtime_error(message);
    }
    return persistFile;
}

void ShortcutFactory::create(std::filesystem::path lnkFilePath, std::filesystem::path targetPath) {
    auto shellLink = newShellLink();
    auto persistFile = newPersistFile(shellLink);

    // Set the target path
    THROW_IF_FAILED(shellLink->SetPath(targetPath.wstring().c_str()));

    // Set working directory to the target path's directory
    if (targetPath.has_parent_path()) {
        THROW_IF_FAILED(shellLink->SetWorkingDirectory(targetPath.parent_path().wstring().c_str()));
    }

    // Save the shortcut file
    THROW_IF_FAILED(persistFile->Save(lnkFilePath.wstring().c_str(), TRUE));
}

std::shared_ptr<Shortcut> ShortcutFactory::open(
    std::filesystem::path lnkFilePath,
    std::filesystem::file_time_type lastWriteTime) {
    auto shellLink = newShellLink();
    auto persistFile = newPersistFile(shellLink);

    auto wstrPath = lnkFilePath.wstring();

    THROW_IF_FAILED(persistFile->Load(wstrPath.c_str(), STGM_READ));
    auto icon = loadIcon(shellLink.get());

    return std::make_shared<Shortcut>(wstrPath, icon, lastWriteTime);
}

wil::shared_hicon ShortcutFactory::loadIcon(IShellLink* shellLink) {
    HICON hIcon = nullptr;
    int iconIndex = 0;

    // Get the icon location
    std::wstring iconPath(MAX_PATH, L'\0');
    HRESULT hr = shellLink->GetIconLocation(iconPath.data(), static_cast<int>(iconPath.size()), &iconIndex);
    if (FAILED(hr)) {
        // If no icon is specified in the shortcut, try to extract from the target
        WCHAR targetPath[MAX_PATH] = { 0 };
        WIN32_FIND_DATA findData = { 0 };
        hr = shellLink->GetPath(targetPath, MAX_PATH, &findData, SLGP_RAWPATH);
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

    return wil::shared_hicon{ hIcon };
}

}  // namespace libprogman
