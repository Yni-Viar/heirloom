#include "pch.h"
#include "libprogman/InstalledAppList.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/Shortcut.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libprogman;

namespace libprogman_tests {

TEST_CLASS (InstalledAppListTests) {
   public:
    TEST_METHOD (ConstructorInitializesFields) {
        ShortcutFactory factory;
        immer::vector<std::filesystem::path> folders = { L"C:\\TestFolder1", L"C:\\TestFolder2" };
        InstalledAppList appList(&factory, folders);

        // Initial call to apps() should return an empty vector
        auto apps = appList.apps();
        Assert::IsTrue(apps.size() == 0);
    }

    TEST_METHOD (FindsShortcutsInFolders) {
        // Create a temporary test directory
        auto testDir = std::filesystem::temp_directory_path() / L"InstalledAppListTest";
        std::filesystem::create_directories(testDir);

        // Create a real shortcut
        ShortcutFactory factory;
        auto lnkPath = testDir / L"TestApp.lnk";
        auto targetPath = L"C:\\Windows\\notepad.exe";
        factory.create(lnkPath, targetPath);

        // Create the app list with our test directory
        immer::vector<std::filesystem::path> folders = { testDir };
        InstalledAppList appList(&factory, folders);

        // Get the apps
        auto apps = appList.apps();

        // Validate that our shortcut was found
        Assert::IsTrue(apps.size() == 1);
        Assert::IsTrue(apps[0]->path() == lnkPath);

        // Clean up
        std::filesystem::remove_all(testDir);
    }

    TEST_METHOD (DetectsUpdatedShortcuts) {
        // Create a temporary test directory
        auto testDir = std::filesystem::temp_directory_path() / L"InstalledAppListTest";
        std::filesystem::create_directories(testDir);

        // Create a real shortcut
        ShortcutFactory factory;
        auto lnkPath = testDir / L"TestApp.lnk";
        auto targetPath = L"C:\\Windows\\notepad.exe";
        factory.create(lnkPath, targetPath);

        // Create the app list with our test directory
        immer::vector<std::filesystem::path> folders = { testDir };
        InstalledAppList appList(&factory, folders);

        // Get the apps
        auto firstApps = appList.apps();
        Assert::IsTrue(firstApps.size() == 1);

        // Sleep to ensure the file time will be different
        Sleep(1000);

        // Update the shortcut
        auto newTargetPath = L"C:\\Windows\\system32\\calc.exe";
        factory.create(lnkPath, newTargetPath);

        // Get the apps again
        auto updatedApps = appList.apps();

        // Validate that our shortcut was updated
        Assert::IsTrue(updatedApps.size() == 1);
        Assert::IsTrue(updatedApps[0]->path() == lnkPath);
        Assert::IsTrue(updatedApps[0]->lastWriteTime() > firstApps[0]->lastWriteTime());

        // Clean up
        std::filesystem::remove_all(testDir);
    }

    TEST_METHOD (HandlesMultipleFolders) {
        // Create temporary test directories
        auto testDir1 = std::filesystem::temp_directory_path() / L"InstalledAppListTest1";
        auto testDir2 = std::filesystem::temp_directory_path() / L"InstalledAppListTest2";
        std::filesystem::create_directories(testDir1);
        std::filesystem::create_directories(testDir2);

        // Create shortcuts in both directories
        ShortcutFactory factory;
        auto lnkPath1 = testDir1 / L"TestApp1.lnk";
        auto lnkPath2 = testDir2 / L"TestApp2.lnk";
        factory.create(lnkPath1, L"C:\\Windows\\notepad.exe");
        factory.create(lnkPath2, L"C:\\Windows\\system32\\calc.exe");

        // Create the app list with both test directories
        immer::vector<std::filesystem::path> folders = { testDir1, testDir2 };
        InstalledAppList appList(&factory, folders);

        // Get the apps
        auto apps = appList.apps();

        // Validate that both shortcuts were found
        Assert::IsTrue(apps.size() == 2);
        bool found1 = false, found2 = false;
        for (const auto& app : apps) {
            if (app->path() == lnkPath1)
                found1 = true;
            if (app->path() == lnkPath2)
                found2 = true;
        }
        Assert::IsTrue(found1 && found2);

        // Clean up
        std::filesystem::remove_all(testDir1);
        std::filesystem::remove_all(testDir2);
    }

    TEST_METHOD (HandlesNonexistentFolders) {
        // Create a valid folder and a nonexistent folder path
        auto testDir = std::filesystem::temp_directory_path() / L"InstalledAppListTest";
        auto nonexistentDir = std::filesystem::temp_directory_path() / L"NonexistentFolder";
        std::filesystem::create_directories(testDir);

        // Create a shortcut in the valid folder
        ShortcutFactory factory;
        auto lnkPath = testDir / L"TestApp.lnk";
        factory.create(lnkPath, L"C:\\Windows\\notepad.exe");

        // Create the app list with both folders
        immer::vector<std::filesystem::path> folders = { testDir, nonexistentDir };
        InstalledAppList appList(&factory, folders);

        // Get the apps - should not throw and should find the shortcut in the valid folder
        auto apps = appList.apps();
        Assert::IsTrue(apps.size() == 1);
        Assert::IsTrue(apps[0]->path() == lnkPath);

        // Clean up
        std::filesystem::remove_all(testDir);
    }
};

}  // namespace libprogman_tests
