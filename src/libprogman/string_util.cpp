#include "libprogman/pch.h"
#include "libprogman/string_util.h"
#include "libprogman/Error.h"

namespace libprogman {

static int ThrowingMultiByteToWideChar(
    UINT CodePage,
    DWORD dwFlags,
    LPCCH lpMultiByteStr,
    int cbMultiByte,
    LPWSTR lpWideCharStr,
    int cchWideChar) {
    int result = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
    if (result <= 0) {
        DWORD error = GetLastError();
        std::wostringstream errorMsg;
        errorMsg << L"Unicode conversion failed. MultiByteToWideChar failed with error code " << std::to_wstring(error);
        throw Error(ErrorCode::kUnicodeConversionFailed, errorMsg.str());
    }
    return result;
}

static int ThrowingWideCharToMultiByte(
    UINT CodePage,
    DWORD dwFlags,
    LPCWCH lpWideCharStr,
    int cchWideChar,
    LPSTR lpMultiByteStr,
    int cbMultiByte,
    LPCCH lpDefaultChar,
    LPBOOL lpUsedDefaultChar) {
    int result = WideCharToMultiByte(
        CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
    if (result <= 0) {
        DWORD error = GetLastError();
        std::wostringstream errorMsg;
        errorMsg << L"Unicode conversion failed. WideCharToMultiByte failed with error code " << error;
        throw Error(ErrorCode::kUnicodeConversionFailed, errorMsg.str());
    }
    return result;
}

std::wstring utf8ToWide(const std::string& str) {
    if (str.empty()) {
        return L"";
    }

    // Calculate the required buffer size
    int size_needed = ThrowingMultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);

    // Allocate the buffer and perform the conversion
    std::wstring result(size_needed, 0);
    ThrowingMultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &result[0], size_needed);

    return result;
}

std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }

    // Calculate the required buffer size
    int size_needed = ThrowingWideCharToMultiByte(
        CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);

    // Allocate the buffer and perform the conversion
    std::string result(size_needed, 0);
    ThrowingWideCharToMultiByte(
        CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);

    return result;
}

}  // namespace libprogman
