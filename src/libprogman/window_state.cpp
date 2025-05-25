#include "libprogman/pch.h"
#include "libprogman/window_state.h"

namespace libprogman {

constexpr std::wstring_view kSectionName = L"WindowPlacement";
constexpr std::wstring_view kKeyFlags = L"Flags";
constexpr std::wstring_view kKeyShowCmd = L"ShowCmd";
constexpr std::wstring_view kKeyMinPositionX = L"MinPositionX";
constexpr std::wstring_view kKeyMinPositionY = L"MinPositionY";
constexpr std::wstring_view kKeyMaxPositionX = L"MaxPositionX";
constexpr std::wstring_view kKeyMaxPositionY = L"MaxPositionY";
constexpr std::wstring_view kKeyNormalLeft = L"NormalLeft";
constexpr std::wstring_view kKeyNormalTop = L"NormalTop";
constexpr std::wstring_view kKeyNormalRight = L"NormalRight";
constexpr std::wstring_view kKeyNormalBottom = L"NormalBottom";
constexpr std::wstring_view kKeyIsMinimizedToList = L"IsMinimizedToList";

void saveWindowState(HWND hwnd, std::filesystem::path iniFilePath) {
    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(WINDOWPLACEMENT);

    if (!GetWindowPlacement(hwnd, &wp)) {
        THROW_LAST_ERROR();
    }

    // Write the window placement data to the INI file
    WritePrivateProfileString(
        kSectionName.data(), kKeyFlags.data(), std::to_wstring(wp.flags).c_str(), iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyShowCmd.data(), std::to_wstring(wp.showCmd).c_str(), iniFilePath.c_str());

    // Save minimized position
    WritePrivateProfileString(
        kSectionName.data(), kKeyMinPositionX.data(), std::to_wstring(wp.ptMinPosition.x).c_str(), iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyMinPositionY.data(), std::to_wstring(wp.ptMinPosition.y).c_str(), iniFilePath.c_str());

    // Save maximized position
    WritePrivateProfileString(
        kSectionName.data(), kKeyMaxPositionX.data(), std::to_wstring(wp.ptMaxPosition.x).c_str(), iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyMaxPositionY.data(), std::to_wstring(wp.ptMaxPosition.y).c_str(), iniFilePath.c_str());

    // Save normal position rectangle
    WritePrivateProfileString(
        kSectionName.data(), kKeyNormalLeft.data(), std::to_wstring(wp.rcNormalPosition.left).c_str(),
        iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyNormalTop.data(), std::to_wstring(wp.rcNormalPosition.top).c_str(),
        iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyNormalRight.data(), std::to_wstring(wp.rcNormalPosition.right).c_str(),
        iniFilePath.c_str());
    WritePrivateProfileString(
        kSectionName.data(), kKeyNormalBottom.data(), std::to_wstring(wp.rcNormalPosition.bottom).c_str(),
        iniFilePath.c_str());
}

bool restoreWindowState(HWND hwnd, std::filesystem::path iniFilePath) {
    // Check if the INI file exists
    if (!std::filesystem::exists(iniFilePath)) {
        return false;
    }

    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(WINDOWPLACEMENT);

    // Read the window placement data from the INI file
    wp.flags = GetPrivateProfileInt(kSectionName.data(), kKeyFlags.data(), 0, iniFilePath.c_str());
    wp.showCmd = GetPrivateProfileInt(kSectionName.data(), kKeyShowCmd.data(), SW_SHOWNORMAL, iniFilePath.c_str());

    // Ignore minimized state, always restore the window
    if (wp.showCmd == SW_SHOWMINIMIZED) {
        wp.showCmd = SW_SHOWNORMAL;
    }

    // Read minimized position
    wp.ptMinPosition.x = GetPrivateProfileInt(kSectionName.data(), kKeyMinPositionX.data(), 0, iniFilePath.c_str());
    wp.ptMinPosition.y = GetPrivateProfileInt(kSectionName.data(), kKeyMinPositionY.data(), 0, iniFilePath.c_str());

    // Read maximized position
    wp.ptMaxPosition.x = GetPrivateProfileInt(kSectionName.data(), kKeyMaxPositionX.data(), 0, iniFilePath.c_str());
    wp.ptMaxPosition.y = GetPrivateProfileInt(kSectionName.data(), kKeyMaxPositionY.data(), 0, iniFilePath.c_str());

    // Read normal position rectangle
    wp.rcNormalPosition.left = GetPrivateProfileInt(kSectionName.data(), kKeyNormalLeft.data(), 0, iniFilePath.c_str());
    wp.rcNormalPosition.top = GetPrivateProfileInt(kSectionName.data(), kKeyNormalTop.data(), 0, iniFilePath.c_str());
    wp.rcNormalPosition.right =
        GetPrivateProfileInt(kSectionName.data(), kKeyNormalRight.data(), 0, iniFilePath.c_str());
    wp.rcNormalPosition.bottom =
        GetPrivateProfileInt(kSectionName.data(), kKeyNormalBottom.data(), 0, iniFilePath.c_str());

    // Apply the window placement
    if (!SetWindowPlacement(hwnd, &wp)) {
        return false;
    }

    return true;
}

// Save whether a window is minimized to the MinimizedFolderListControl
void saveWindowMinimizedState(std::filesystem::path iniFilePath, bool isMinimized) {
    // Write minimized state to the INI file
    WritePrivateProfileString(
        kSectionName.data(), kKeyIsMinimizedToList.data(), isMinimized ? L"1" : L"0", iniFilePath.c_str());
}

// Get whether a window was minimized to the MinimizedFolderListControl
bool getWindowMinimizedState(std::filesystem::path iniFilePath) {
    // Check if the INI file exists
    if (!std::filesystem::exists(iniFilePath)) {
        return false;
    }

    // Read the minimized state from the INI file
    return GetPrivateProfileInt(kSectionName.data(), kKeyIsMinimizedToList.data(), 0, iniFilePath.c_str()) == 1;
}

}  // namespace libprogman
