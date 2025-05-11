#include "gitbash.h"

std::optional<std::wstring> GetGitBashPath() {
    const wchar_t* subKey = L"SOFTWARE\\GitForWindows";
    const wchar_t* valueName = L"InstallPath";
    wchar_t installPath[MAX_PATH] = { 0 };
    DWORD bufSize = sizeof(installPath);
    LSTATUS status;

    // Try the 64-bit registry view first
    status = RegGetValueW(
        HKEY_LOCAL_MACHINE, subKey, valueName, RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, nullptr, installPath, &bufSize);

    if (status != ERROR_SUCCESS) {
        // If not found in 64-bit view, try the 32-bit view
        bufSize = sizeof(installPath);
        status = RegGetValueW(
            HKEY_LOCAL_MACHINE, subKey, valueName, RRF_RT_REG_SZ | RRF_SUBKEY_WOW6432KEY, nullptr, installPath,
            &bufSize);
    }

    if (status == ERROR_SUCCESS && installPath[0] != L'\0') {
        // Append "\git-bash.exe" and return
        std::wstring path(installPath);
        if (path.back() != L'\\')
            path += L'\\';
        path += L"git-bash.exe";
        return path;
    }

    // Not found
    return std::nullopt;
}
