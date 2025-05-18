#include "libprogman/pch.h"
#include "libprogman/string_util.h"

namespace libprogman {

std::wstring utf8ToWide(const std::string& str) {
    if (str.empty()) {
        return L"";
    }

    // Calculate the required buffer size
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    if (size_needed <= 0) {
        return L"";
    }

    // Allocate the buffer and perform the conversion
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &result[0], size_needed);

    return result;
}

std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }

    // Calculate the required buffer size
    int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) {
        return "";
    }

    // Allocate the buffer and perform the conversion
    std::string result(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);

    return result;
}

}  // namespace libprogman
