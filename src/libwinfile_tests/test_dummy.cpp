#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libwinfile_tests {

TEST_CLASS (test_dummy) {
    public:
    TEST_METHOD (test_dummy_method) {
        Assert::IsTrue(true);
    }
};

}  // namespace libwinfile_tests
