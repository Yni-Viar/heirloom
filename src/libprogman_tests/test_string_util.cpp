#include "pch.h"
#include "CppUnitTest.h"
#include "libprogman/string_util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libprogman_tests {

TEST_CLASS (test_string_util) {
   public:
    TEST_METHOD (TestUtf8ToWideAndBack) {
        std::string s1 = "hello!";
        std::wstring s2 = libprogman::utf8ToWide(s1);
        Assert::AreEqual(L"hello!", s2.c_str());
        std::string s3 = libprogman::wideToUtf8(s2);
        Assert::AreEqual("hello!", s3.c_str());
    }
};

}  // namespace libprogman_tests
