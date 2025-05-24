#include "libprogman/pch.h"
#include "libprogman/window_data.h"

namespace libprogman {

static LPCWSTR getKeyAtom() {
    static const LPCWSTR atom = MAKEINTATOM(GlobalAddAtomW(L"HEIRLOOM_PROGMAN_WINDOW_DATA"));
    return atom;
}

void setWindowDataImpl(HWND hwnd, void* data) {
    SetPropW(hwnd, getKeyAtom(), data);
}

void* getWindowDataImpl(HWND hwnd) {
    return GetPropW(hwnd, getKeyAtom());
}

}  // namespace libprogman
