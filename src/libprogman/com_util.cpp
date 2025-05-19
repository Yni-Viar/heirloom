#include "libprogman/pch.h"
#include "libprogman/com_util.h"

namespace libprogman {

void throwIfComFailed(HRESULT hr, const std::string& information) {
    if (FAILED(hr)) {
        try {
            // Get WIL to provide a good error message for the HRESULT itself.
            THROW_IF_FAILED(hr);
        } catch (const std::exception& e) {
            // Then prepend our friendly message.
            throw std::runtime_error(information + "\n\n" + e.what() + " (error code " + std::to_string(hr) + ")");
        }
    }
}

}  // namespace libprogman
