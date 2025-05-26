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

// Gets the appropriate icon size based on current DPI scaling
static int getScaledIconSize(HWND hwnd = nullptr) {
    // Get the DPI for the window or primary monitor
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : GetDpiForSystem();

    // Calculate scaling factor (96 is the baseline DPI)
    double scalingFactor = static_cast<double>(dpi) / 96.0;

    // Base icon size is 32x32, scale it based on DPI
    int scaledSize = static_cast<int>(32 * scalingFactor);

    // Round to common icon sizes (16, 32, 48, 64, 128, 256)
    if (scaledSize <= 16)
        return 16;
    if (scaledSize <= 32)
        return 32;
    if (scaledSize <= 48)
        return 48;
    if (scaledSize <= 64)
        return 64;
    if (scaledSize <= 128)
        return 128;
    return 256;
}

wil::shared_hicon ShortcutFactory::loadIcon(IShellLink* shellLink) {
    HICON hIcon = nullptr;
    int iconSize = getScaledIconSize();

    // First try to get the icon from the shortcut
    WCHAR iconPath[MAX_PATH] = { 0 };
    int iconIndex = 0;
    HRESULT hr = shellLink->GetIconLocation(iconPath, MAX_PATH, &iconIndex);

    if (SUCCEEDED(hr) && iconPath[0] != L'\0') {
        // Try to load the icon at the appropriate size
        hIcon = static_cast<HICON>(
            LoadImage(nullptr, iconPath, IMAGE_ICON, iconSize, iconSize, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS));

        if (!hIcon) {
            // If failed, try SHDefExtractIcon which can scale icons
            hr = SHDefExtractIconW(iconPath, iconIndex, 0, &hIcon, nullptr, iconSize);

            if (FAILED(hr) || !hIcon) {
                // If that fails, fall back to ExtractIconEx
                ExtractIconExW(iconPath, iconIndex, nullptr, &hIcon, 1);
            }
        }
    }

    // If we still don't have an icon, try to get it from the target file
    if (!hIcon) {
        WCHAR targetPath[MAX_PATH] = { 0 };
        WIN32_FIND_DATAW findData = { 0 };
        hr = shellLink->GetPath(targetPath, MAX_PATH, &findData, SLGP_RAWPATH);

        if (SUCCEEDED(hr) && targetPath[0] != L'\0') {
            // Use SHGetFileInfo to get the icon with proper scaling
            SHFILEINFOW sfi = { 0 };
            DWORD_PTR result = SHGetFileInfoW(
                targetPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

            if (result && sfi.hIcon) {
                hIcon = sfi.hIcon;

                // For better high-DPI handling, try to get a better size using IExtractIcon
                wil::com_ptr<IShellItemImageFactory> psiif;
                hr = SHCreateItemFromParsingName(targetPath, nullptr, IID_PPV_ARGS(&psiif));

                if (SUCCEEDED(hr)) {
                    HBITMAP hBitmap = nullptr;
                    SIZE size = { iconSize, iconSize };

                    // Try to get higher quality icon at the right size
                    hr = psiif->GetImage(size, SIIGBF_ICONONLY, &hBitmap);
                    if (SUCCEEDED(hr) && hBitmap) {
                        // Convert HBITMAP to HICON
                        ICONINFO iconInfo = { 0 };
                        iconInfo.fIcon = TRUE;
                        iconInfo.hbmMask = hBitmap;
                        iconInfo.hbmColor = hBitmap;

                        HICON hBetterIcon = CreateIconIndirect(&iconInfo);
                        if (hBetterIcon) {
                            DestroyIcon(hIcon);  // Destroy the previous icon
                            hIcon = hBetterIcon;
                        }

                        // Clean up the bitmap
                        DeleteObject(hBitmap);
                    }
                }
            }
        }
    }

    // If we still don't have an icon, use default icon from shell32.dll
    if (!hIcon) {
        // Load shell32.dll default icon with appropriate scaling
        hIcon = static_cast<HICON>(LoadImage(nullptr, IDI_APPLICATION, IMAGE_ICON, iconSize, iconSize, LR_SHARED));

        if (!hIcon) {
            // Last resort, try shell32.dll
            SHDefExtractIconW(L"shell32.dll", 0, 0, &hIcon, nullptr, iconSize);
        }
    }

    return wil::shared_hicon{ hIcon };
}

}  // namespace libprogman
