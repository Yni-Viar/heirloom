#pragma once

#include "libprogman/pch.h"

namespace libprogman {

enum class ErrorCode {
    kFileNotFound,
    kFolderNotFound,
    kFolderAlreadyExists,
    kUnicodeConversionFailed,
    kFilesystemOperationFailed,
};

class Error : public std::runtime_error {
   public:
    Error(ErrorCode code, std::wstring message);
    std::wstring message() const;
    ErrorCode code() const;

   private:
    ErrorCode code_;
    std::wstring message_;
};

}  // namespace libprogman
