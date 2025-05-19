#include "libprogman/pch.h"
#include "libprogman/Error.h"
#include "libprogman/string_util.h"

namespace libprogman {

Error::Error(ErrorCode code, std::wstring message)
    : std::runtime_error(wideToUtf8(message)), code_(code), message_(std::move(message)) {}

std::wstring Error::message() const {
    return message_;
}

ErrorCode Error::code() const {
    return code_;
}

}  // namespace libprogman
