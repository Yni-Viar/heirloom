#pragma once

#include "libprogman/pch.h"
#include "libprogman/core/string_util.h"

namespace libprogman {

class Error : public std::runtime_error {
   public:
    Error(std::wstring message);
    std::wstring message() const;

   private:
    std::wstring message_;
};

}  // namespace libprogman
