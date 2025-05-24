#include "pch.h"
#include "CppUnitTest.h"
#include "libprogman/window_data.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libprogman_tests {

// Helper struct for tests
struct TestData {
    int value;
    std::wstring text;

    TestData(int v, std::wstring t) : value(v), text(std::move(t)) {}

    bool operator==(const TestData& other) const { return value == other.value && text == other.text; }
};

TEST_CLASS (test_window_data) {
   public:
    // Helper function to create a real window for testing
    HWND createTestWindow() {
        // Register a simple window class
        static bool registered = false;
        static const wchar_t* CLASS_NAME = L"WindowDataTest";

        if (!registered) {
            WNDCLASSEXW wcex = {};
            wcex.cbSize = sizeof(WNDCLASSEXW);
            wcex.lpfnWndProc = DefWindowProcW;
            wcex.hInstance = GetModuleHandleW(nullptr);
            wcex.lpszClassName = CLASS_NAME;

            RegisterClassExW(&wcex);
            registered = true;
        }

        // Create the window
        HWND hwnd = CreateWindowExW(
            0,                    // Optional window styles
            CLASS_NAME,           // Window class
            L"Test Window",       // Window text
            WS_OVERLAPPEDWINDOW,  // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,

            nullptr,                    // Parent window
            nullptr,                    // Menu
            GetModuleHandleW(nullptr),  // Instance handle
            nullptr                     // Additional application data
        );

        return hwnd;
    }

    TEST_METHOD (TestSetAndGetWindowData) {
        // Create a window
        HWND hwnd = createTestWindow();
        Assert::IsNotNull(hwnd);

        // Create test data
        auto data = std::make_unique<TestData>(42, L"Hello Window");

        // Set the data
        libprogman::setWindowData<TestData>(hwnd, data.get());

        // Get the data
        TestData* retrievedData = libprogman::getWindowData<TestData>(hwnd);

        // Check that retrieved data is not null and matches
        Assert::IsNotNull(retrievedData);
        Assert::AreEqual(42, retrievedData->value);
        Assert::AreEqual(L"Hello Window", retrievedData->text.c_str());
        Assert::IsTrue(*data == *retrievedData);

        // Check that the retrieved pointer is the same as the one we set
        Assert::IsTrue(data.get() == retrievedData);

        // Destroy the window
        DestroyWindow(hwnd);
    }

    TEST_METHOD (TestMultipleWindows) {
        // Create two windows
        HWND hwnd1 = createTestWindow();
        HWND hwnd2 = createTestWindow();

        Assert::IsNotNull(hwnd1);
        Assert::IsNotNull(hwnd2);

        // Create test data
        auto data1 = std::make_unique<TestData>(42, L"Window One");
        auto data2 = std::make_unique<TestData>(84, L"Window Two");

        // Set the data
        libprogman::setWindowData<TestData>(hwnd1, data1.get());
        libprogman::setWindowData<TestData>(hwnd2, data2.get());

        // Check the data in both windows
        TestData* retrievedData1 = libprogman::getWindowData<TestData>(hwnd1);
        TestData* retrievedData2 = libprogman::getWindowData<TestData>(hwnd2);

        // Verify first window's data
        Assert::IsNotNull(retrievedData1);
        Assert::AreEqual(42, retrievedData1->value);
        Assert::AreEqual(L"Window One", retrievedData1->text.c_str());
        Assert::IsTrue(data1.get() == retrievedData1);

        // Verify second window's data
        Assert::IsNotNull(retrievedData2);
        Assert::AreEqual(84, retrievedData2->value);
        Assert::AreEqual(L"Window Two", retrievedData2->text.c_str());
        Assert::IsTrue(data2.get() == retrievedData2);

        // Destroy the windows
        DestroyWindow(hwnd1);
        DestroyWindow(hwnd2);
    }

    TEST_METHOD (TestOverwriteData) {
        // Create a window
        HWND hwnd = createTestWindow();
        Assert::IsNotNull(hwnd);

        // Create test data
        auto data1 = std::make_unique<TestData>(42, L"Original Data");
        auto data2 = std::make_unique<TestData>(99, L"Overwritten Data");

        // Set original data
        libprogman::setWindowData<TestData>(hwnd, data1.get());

        // Verify original data
        TestData* retrievedData = libprogman::getWindowData<TestData>(hwnd);
        Assert::IsNotNull(retrievedData);
        Assert::AreEqual(42, retrievedData->value);
        Assert::AreEqual(L"Original Data", retrievedData->text.c_str());

        // Overwrite with new data
        libprogman::setWindowData<TestData>(hwnd, data2.get());

        // Verify new data
        retrievedData = libprogman::getWindowData<TestData>(hwnd);
        Assert::IsNotNull(retrievedData);
        Assert::AreEqual(99, retrievedData->value);
        Assert::AreEqual(L"Overwritten Data", retrievedData->text.c_str());
        Assert::IsTrue(data2.get() == retrievedData);

        // Destroy the window
        DestroyWindow(hwnd);
    }

    TEST_METHOD (TestNullData) {
        // Create a window
        HWND hwnd = createTestWindow();
        Assert::IsNotNull(hwnd);

        // Set null data
        libprogman::setWindowData<TestData>(hwnd, nullptr);

        // Verify null data is retrieved
        TestData* retrievedData = libprogman::getWindowData<TestData>(hwnd);
        Assert::IsNull(retrievedData);

        // Destroy the window
        DestroyWindow(hwnd);
    }
};

}  // namespace libprogman_tests
