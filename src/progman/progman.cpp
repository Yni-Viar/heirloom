#include "../libprogman/windows10.h"
#include "../libprogman/progman.h"

int APIENTRY
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    return progman::ProgmanWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
