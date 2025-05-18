#include "pch.h"
#include "CppUnitTest.h"
#include "libprogman/Shortcut.h"
#include <ShlObj.h>
#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libprogram;

namespace libprogman_tests {

class ComInitializer {
   public:
    ComInitializer() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        initialized_ = SUCCEEDED(hr);
    }

    ~ComInitializer() {
        if (initialized_) {
            CoUninitialize();
        }
    }

    bool isInitialized() const { return initialized_; }

   private:
    bool initialized_;
};

TEST_CLASS (test_Shortcut) {
   private:
    ComInitializer comInit_;
    std::wstring testDir_;
    std::wstring targetPath_;
    std::wstring lnkPath_;

   public:
    test_Shortcut() {
        // Get temp directory for test files
        wchar_t tempPath[MAX_PATH];
        GetTempPathW(MAX_PATH, tempPath);
        testDir_ = tempPath;
        testDir_ += L"ShortcutTest\\";
        std::filesystem::create_directories(testDir_);

        // Use notepad as our target application
        wchar_t systemDir[MAX_PATH];
        GetSystemDirectoryW(systemDir, MAX_PATH);
        targetPath_ = systemDir;
        targetPath_ += L"\\notepad.exe";

        // Our test shortcut path
        lnkPath_ = testDir_ + L"TestShortcut.lnk";
    }

    ~test_Shortcut() {
        // Clean up test files
        try {
            std::filesystem::remove_all(testDir_);
        } catch (...) {
            // Ignore cleanup errors
        }
    }

    TEST_METHOD_INITIALIZE(Initialize) {
        Assert::IsTrue(comInit_.isInitialized(), L"COM initialization failed");

        // Remove any existing test file
        try {
            std::filesystem::remove(lnkPath_);
        } catch (...) {
            // Ignore if file doesn't exist
        }
    }

    TEST_METHOD (TestInitNew) {
        Shortcut shortcut;
        shortcut.initNew(lnkPath_, targetPath_);

        // Verify the shortcut file was created
        Assert::IsTrue(std::filesystem::exists(lnkPath_), L"Shortcut file was not created");
    }

    TEST_METHOD (TestInitOpen) {
        // First create a shortcut
        {
            Shortcut shortcut;
            shortcut.initNew(lnkPath_, targetPath_);
        }

        // Then try to open it
        Shortcut shortcut;
        shortcut.initOpen(lnkPath_);

        // If we get here without exceptions, the test passed
    }

    TEST_METHOD (TestGetDisplayName) {
        // Create a shortcut first
        Shortcut shortcut;
        shortcut.initNew(lnkPath_, targetPath_);

        std::wstring displayName = shortcut.getDisplayName();

        // The display name should be "TestShortcut" (without .lnk extension)
        Assert::AreEqual(L"TestShortcut", displayName.c_str());
    }

    TEST_METHOD (TestLoadIcon) {
        // Create a shortcut first
        Shortcut shortcut;
        shortcut.initNew(lnkPath_, targetPath_);

        wil::unique_hicon icon = shortcut.loadIcon();

        // Verify the icon handle is not null
        Assert::IsNotNull(icon.get(), L"Icon should not be null");
    }
};

}  // namespace libprogman_tests
