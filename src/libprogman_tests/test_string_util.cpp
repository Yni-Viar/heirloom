#include "pch.h"
#include "CppUnitTest.h"
#include "libprogman/core/string_util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libprogmantests {

TEST_CLASS (test_string_util) {
   public:
    TEST_METHOD (TestUtf8ToWideAndBack) {
        std::string s1 = "hello!";
        std::wstring s2 = progman::utf8ToWide(s1);
        Assert::AreEqual(L"hello!", s2.c_str());
        std::string s3 = progman::wideToUtf8(s2);
        Assert::AreEqual("hello!", s3.c_str());
    }
};

}  // namespace libprogmantests
