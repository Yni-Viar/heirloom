#pragma once

#include "libprogman/pch.h"

namespace libprogman {

void throwIfComFailed(HRESULT hr, const std::string& information);

}  // namespace libprogman
