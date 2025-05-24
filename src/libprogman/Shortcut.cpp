#include "libprogman/pch.h"
#include "libprogman/Shortcut.h"

namespace libprogman {

Shortcut::Shortcut(
    std::filesystem::path path,
    wil::shared_hicon icon,
    std::filesystem::file_time_type lastWriteTime) noexcept
    : path_(std::move(path)), icon_(std::move(icon)), lastWriteTime_(lastWriteTime) {
    name_ = path_.stem().wstring();
}

const std::filesystem::path& Shortcut::path() const noexcept {
    return path_;
}

const std::wstring& Shortcut::name() const noexcept {
    return name_;
}

wil::shared_hicon Shortcut::icon() const noexcept {
    return icon_;
}

std::filesystem::file_time_type Shortcut::lastWriteTime() const noexcept {
    return lastWriteTime_;
}

void Shortcut::showPropertiesWindow() const {
    // Use the shell to show the properties dialog
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = path_.c_str();
    sei.lpVerb = L"properties";
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.nShow = SW_SHOW;

    THROW_IF_FAILED(ShellExecuteExW(&sei) ? S_OK : HRESULT_FROM_WIN32(GetLastError()));
}

void Shortcut::launch() const {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = path_.c_str();
    sei.nShow = SW_SHOWNORMAL;

    THROW_IF_FAILED(ShellExecuteExW(&sei) ? S_OK : HRESULT_FROM_WIN32(GetLastError()));
}

void Shortcut::deleteFile() const {
    try {
        std::filesystem::remove(path_);
    } catch (const std::filesystem::filesystem_error& e) {
        // Convert filesystem error to an exception with a user-friendly message
        std::string errorMsg = "Failed to delete shortcut: " + std::string(e.what());
        throw std::runtime_error(errorMsg);
    }
}

}  // namespace libprogman
