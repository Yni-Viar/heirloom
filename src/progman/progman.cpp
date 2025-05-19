#include "progman/pch.h"
#include "libprogman/com_util.h"
#include "libprogman/FolderWatcher.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/ShortcutManager.h"
#include "libprogman/string_util.h"
#include "progman/ProgramManagerWindow.h"
#include "progman/resource.h"

static std::filesystem::path createShortcutsFolder() {
    // It's "%APPDATA%\Heirloom Program Manager\Shortcuts\".

    // Get the user's app data directory
    PWSTR appDataPath = nullptr;
    THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, nullptr, &appDataPath));

    // Build the full path to the shortcuts directory
    std::filesystem::path shortcutsPath =
        std::filesystem::path(appDataPath) / L"Heirloom Program Manager" / L"Shortcuts";
    CoTaskMemFree(appDataPath);

    // Create the directory if it doesn't exist
    std::filesystem::create_directories(shortcutsPath);

    return shortcutsPath;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    try {
        // Initialize COM
        auto com = wil::CoInitializeEx(COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // Create DI graph
        auto shortcutsPath = createShortcutsFolder();

        auto shortcutFactory = std::make_unique<libprogman::ShortcutFactory>();

        auto shortcutManager = std::make_unique<libprogman::ShortcutManager>(shortcutsPath, shortcutFactory.get());

        auto programManagerWindow = std::make_unique<progman::ProgramManagerWindow>();
        programManagerWindow->create();
        programManagerWindow->show(nCmdShow);

        auto folderWatcher = std::make_unique<libprogman::FolderWatcher>(
            shortcutsPath, [&shortcutManager]() { shortcutManager->refresh(); },
            [](std::wstring errorMsg) {
                MessageBox(nullptr, errorMsg.c_str(), L"Program Manager", MB_OK | MB_ICONERROR);
            });

        // Message pump
        auto accelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROGMAN));
        MSG msg{};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (!TranslateAccelerator(msg.hwnd, accelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return 0;
    } catch (const std::exception& e) {
        std::wstring wstr = libprogman::utf8ToWide(e.what());
        MessageBox(nullptr, wstr.c_str(), L"Program Manager", MB_OK | MB_ICONERROR);
        return 1;
    }
}
