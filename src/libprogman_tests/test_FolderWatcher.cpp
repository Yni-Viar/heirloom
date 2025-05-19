#include "pch.h"
#include "CppUnitTest.h"
#include "libprogman/FolderWatcher.h"
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libprogman_tests {

// Helper class to manage a temporary test folder
class TestFolder {
   public:
    TestFolder() {
        // Create a unique temporary directory
        std::filesystem::path tempDir = std::filesystem::temp_directory_path();
        std::wstring testFolderName = L"folderWatcherTest_" +
            std::to_wstring(std::chrono::system_clock::now().time_since_epoch().count() % 10000);
        path_ = tempDir / testFolderName;

        std::filesystem::create_directory(path_);
    }

    ~TestFolder() {
        try {
            if (std::filesystem::exists(path_)) {
                std::filesystem::remove_all(path_);
            }
        } catch (...) {
            // Ignore cleanup errors
        }
    }

    const std::filesystem::path& path() const { return path_; }

   private:
    std::filesystem::path path_;
};

// Helper class to set up and wait for file change notifications
class ChangeDetector {
   public:
    ChangeDetector(const std::filesystem::path& folderPath)
        : folderPath_(folderPath),
          watcher_(
              folderPath,
              [this]() { this->onChangeDetected(); },
              [](std::wstring errorMsg) { Logger::WriteMessage((L"Error in FolderWatcher: " + errorMsg).c_str()); }),
          changeDetected_(false) {
        // Allow watcher to initialize
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void onChangeDetected() {
        std::lock_guard<std::mutex> lock(mutex_);
        changeDetected_ = true;
        cv_.notify_all();
    }

    bool waitForChange(std::chrono::seconds timeout = std::chrono::seconds(5)) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [this]() { return changeDetected_; });
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        changeDetected_ = false;
    }

   private:
    std::filesystem::path folderPath_;
    libprogman::FolderWatcher watcher_;
    bool changeDetected_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

TEST_CLASS (test_FolderWatcher) {
   public:
    TEST_METHOD (TestFileCreation) {
        TestFolder testFolder;
        ChangeDetector detector(testFolder.path());

        // Create a new file
        std::filesystem::path testFile = testFolder.path() / L"newFile.txt";
        {
            std::ofstream file(testFile.string());
            file << "Test content";
            file.close();
        }

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"File creation should be detected");
    }

    TEST_METHOD (TestFileModification) {
        TestFolder testFolder;

        // Create a file before setting up the watcher
        std::filesystem::path testFile = testFolder.path() / L"existingFile.txt";
        {
            std::ofstream file(testFile.string());
            file << "Initial content";
            file.close();
        }

        // Wait to ensure file timestamp will differ
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Set up the change detector
        ChangeDetector detector(testFolder.path());

        // Modify the file
        {
            std::ofstream file(testFile.string());
            file << "Modified content";
            file.close();
        }

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"File modification should be detected");
    }

    TEST_METHOD (TestFileRename) {
        TestFolder testFolder;

        // Create a file before setting up the watcher
        std::filesystem::path originalFile = testFolder.path() / L"originalName.txt";
        std::filesystem::path renamedFile = testFolder.path() / L"renamedFile.txt";
        {
            std::ofstream file(originalFile.string());
            file << "File content";
            file.close();
        }

        // Set up the change detector
        ChangeDetector detector(testFolder.path());

        // Rename the file
        std::filesystem::rename(originalFile, renamedFile);

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"File rename should be detected");
    }

    TEST_METHOD (TestFileDeletion) {
        TestFolder testFolder;

        // Create a file before setting up the watcher
        std::filesystem::path testFile = testFolder.path() / L"fileToDelete.txt";
        {
            std::ofstream file(testFile.string());
            file << "File content";
            file.close();
        }

        // Set up the change detector
        ChangeDetector detector(testFolder.path());

        // Delete the file
        std::filesystem::remove(testFile);

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"File deletion should be detected");
    }

    TEST_METHOD (TestFolderCreation) {
        TestFolder testFolder;
        ChangeDetector detector(testFolder.path());

        // Create a new subfolder
        std::filesystem::path newFolder = testFolder.path() / L"newSubfolder";
        std::filesystem::create_directory(newFolder);

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"Folder creation should be detected");
    }

    TEST_METHOD (TestFolderRename) {
        TestFolder testFolder;

        // Create a subfolder before setting up the watcher
        std::filesystem::path originalFolder = testFolder.path() / L"originalFolder";
        std::filesystem::path renamedFolder = testFolder.path() / L"renamedFolder";
        std::filesystem::create_directory(originalFolder);

        // Set up the change detector
        ChangeDetector detector(testFolder.path());

        // Rename the folder
        std::filesystem::rename(originalFolder, renamedFolder);

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"Folder rename should be detected");
    }

    TEST_METHOD (TestFolderDeletion) {
        TestFolder testFolder;

        // Create a subfolder before setting up the watcher
        std::filesystem::path folderToDelete = testFolder.path() / L"folderToDelete";
        std::filesystem::create_directory(folderToDelete);

        // Set up the change detector
        ChangeDetector detector(testFolder.path());

        // Delete the folder
        std::filesystem::remove(folderToDelete);

        // Verify change detection
        bool changeDetected = detector.waitForChange();
        Assert::IsTrue(changeDetected, L"Folder deletion should be detected");
    }
};

}  // namespace libprogman_tests
