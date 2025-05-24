#include "pch.h"
#include "libprogman/window_state.h"
#include <CppUnitTest.h>
#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libprogman_tests {

// Helper class to create a test window
class TestWindow {
   public:
    TestWindow(const std::wstring& className = L"TestWindowClass", const std::wstring& windowName = L"Test Window") {
        // Register window class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DefWindowProcW;
        wcex.hInstance = GetModuleHandleW(nullptr);
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = className.c_str();

        RegisterClassExW(&wcex);

        // Create window
        hwnd_ = CreateWindowExW(
            0, className.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

        Assert::IsNotNull(hwnd_, L"Failed to create test window");
    }

    ~TestWindow() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    HWND hwnd() const { return hwnd_; }

    void moveAndResize(int x, int y, int width, int height) { MoveWindow(hwnd_, x, y, width, height, TRUE); }

    void show(int nCmdShow = SW_SHOWNORMAL) {
        ShowWindow(hwnd_, nCmdShow);
        UpdateWindow(hwnd_);
    }

    WINDOWPLACEMENT getPlacement() const {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(hwnd_, &wp);
        return wp;
    }

   private:
    HWND hwnd_ = nullptr;
};

TEST_CLASS (WindowStateTest) {
   public:
    TEST_METHOD_INITIALIZE(SetUp) {
        // Create a temporary directory for test files
        tempDir_ = std::filesystem::temp_directory_path() / "WindowStateTest";
        std::filesystem::create_directories(tempDir_);

        iniFilePath_ = tempDir_ / "window_state_test.ini";
    }

    TEST_METHOD_CLEANUP(TearDown) {
        // Clean up temporary directory
        std::filesystem::remove_all(tempDir_);
    }

    TEST_METHOD (SaveAndRestoreNormalState) {
        // Create a test window
        TestWindow window;

        // Position the window
        window.moveAndResize(100, 200, 800, 600);
        window.show();

        // Save window state
        libprogman::saveWindowState(window.hwnd(), iniFilePath_);

        // Create a second window with different position
        TestWindow window2;
        window2.moveAndResize(400, 300, 500, 400);
        window2.show();

        // Record the placement before restoring
        auto placementBefore = window2.getPlacement();

        // Restore window state
        libprogman::restoreWindowState(window2.hwnd(), iniFilePath_);

        // Check that the state was restored
        auto placementAfter = window2.getPlacement();

        // Verify that the placement changed
        Assert::IsTrue(placementBefore.rcNormalPosition.left != placementAfter.rcNormalPosition.left);
        Assert::IsTrue(placementBefore.rcNormalPosition.top != placementAfter.rcNormalPosition.top);

        // Verify that the placement matches the first window
        auto originalPlacement = window.getPlacement();
        Assert::AreEqual(originalPlacement.rcNormalPosition.left, placementAfter.rcNormalPosition.left);
        Assert::AreEqual(originalPlacement.rcNormalPosition.top, placementAfter.rcNormalPosition.top);
        Assert::AreEqual(originalPlacement.rcNormalPosition.right, placementAfter.rcNormalPosition.right);
        Assert::AreEqual(originalPlacement.rcNormalPosition.bottom, placementAfter.rcNormalPosition.bottom);
    }

    TEST_METHOD (SaveAndRestoreMaximizedState) {
        // Create a test window
        TestWindow window;

        // Maximize the window
        window.show(SW_MAXIMIZE);

        // Save window state
        libprogman::saveWindowState(window.hwnd(), iniFilePath_);

        // Create a second window with normal state
        TestWindow window2;
        window2.show(SW_SHOWNORMAL);

        // Record the state before restoring
        auto placementBefore = window2.getPlacement();
        Assert::IsTrue(placementBefore.showCmd != SW_MAXIMIZE);

        // Restore window state
        libprogman::restoreWindowState(window2.hwnd(), iniFilePath_);

        // Check that the state was restored
        auto placementAfter = window2.getPlacement();
        Assert::IsTrue(placementAfter.showCmd == SW_MAXIMIZE);
    }

    TEST_METHOD (HandleNonExistentFile) {
        // Create a test window
        TestWindow window;
        window.show();

        // Try to restore from a non-existent file
        auto nonExistentFile = tempDir_ / "does_not_exist.ini";

        // This should not throw and should leave the window unchanged
        auto placementBefore = window.getPlacement();
        libprogman::restoreWindowState(window.hwnd(), nonExistentFile);
        auto placementAfter = window.getPlacement();

        // Verify the placement is unchanged
        Assert::AreEqual(placementBefore.rcNormalPosition.left, placementAfter.rcNormalPosition.left);
        Assert::AreEqual(placementBefore.rcNormalPosition.top, placementAfter.rcNormalPosition.top);
        Assert::AreEqual(placementBefore.rcNormalPosition.right, placementAfter.rcNormalPosition.right);
        Assert::AreEqual(placementBefore.rcNormalPosition.bottom, placementAfter.rcNormalPosition.bottom);
    }

   private:
    std::filesystem::path tempDir_;
    std::filesystem::path iniFilePath_;
};

}  // namespace libprogman_tests
