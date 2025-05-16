#include "libprogman/pch.h"
#include "libprogman/progman.h"
#include "libprogman/resource.h"

namespace progman {

static HINSTANCE g_appInstance;

int ProgmanWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow) {
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    g_appInstance = instance;

    // TODO: Initialization

    auto accelTable = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_PROGMAN));

    MSG msg{};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, accelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

}  // namespace progman
