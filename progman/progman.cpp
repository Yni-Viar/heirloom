#include "progman.h"

#include "common.h"
#include "resource.h"

static HINSTANCE g_appInstance;

int APIENTRY
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    g_appInstance = hInstance;

    // TODO: Initialization

    auto accelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROGMAN));

    MSG msg{};

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, accelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}
