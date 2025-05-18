#include "progman/pch.h"
#include "progman/progman.h"
#include "progman/resource.h"
#include "progman/ProgramManagerWindow.h"

namespace progman {

static HINSTANCE g_appInstance;

}  // namespace progman

int APIENTRY
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    progman::g_appInstance = hInstance;

    progman::ProgramManagerWindow window{};
    window.create();
    window.show(nCmdShow);

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
