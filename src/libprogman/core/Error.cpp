#include "libprogman/pch.h"
#include "libprogman/core/Error.h"

namespace progman {

Error::Error(std::wstring message) : std::runtime_error(wideToUtf8(message)), message_(std::move(message)) {}

std::wstring Error::message() const {
    return message_;
}

}  // namespace progman
