#include "libprogman/pch.h"
#include "libprogman/Error.h"

namespace libprogman {

Error::Error(std::wstring message) : std::runtime_error(wideToUtf8(message)), message_(std::move(message)) {}

std::wstring Error::message() const {
    return message_;
}

}  // namespace libprogman
