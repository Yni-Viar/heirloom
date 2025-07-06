#include "pch.h"
#include "CppUnitTest.h"
#include <libwinfile/ArchiveStatus.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using libwinfile::ArchiveStatus;

namespace libwinfile_tests {

TEST_CLASS (ArchiveStatusTests) {
   public:
    TEST_METHOD (TestInitialState) {
        // Test that a new ArchiveStatus object is not dirty initially
        ArchiveStatus status;
        Assert::IsFalse(status.dirty());
    }

    TEST_METHOD (TestUpdateMakesDirty) {
        // Test that calling update() makes the object dirty
        ArchiveStatus status;

        status.update(L"test.zip", L"Compressing file:", L"test.txt");

        Assert::IsTrue(status.dirty());
    }

    TEST_METHOD (TestReadCleansDirty) {
        // Test that calling read() cleans the dirty flag
        ArchiveStatus status;

        status.update(L"test.zip", L"Compressing file:", L"test.txt");
        Assert::IsTrue(status.dirty());

        std::wstring archivePath, operationText, operationFile;
        status.read(&archivePath, &operationText, &operationFile);

        Assert::IsFalse(status.dirty());
    }

    TEST_METHOD (TestReadReturnsCorrectValues) {
        // Test that read() returns the values set by update()
        ArchiveStatus status;

        const std::wstring expectedArchive = L"test.zip";
        const std::wstring expectedOperation = L"Compressing file:";
        const std::wstring expectedFile = L"test.txt";

        status.update(expectedArchive, expectedOperation, expectedFile);

        std::wstring archivePath, operationText, operationFile;
        status.read(&archivePath, &operationText, &operationFile);

        Assert::AreEqual(expectedArchive, archivePath);
        Assert::AreEqual(expectedOperation, operationText);
        Assert::AreEqual(expectedFile, operationFile);
    }

    TEST_METHOD (TestReadWithNullPointers) {
        // Test that read() handles null pointers gracefully
        ArchiveStatus status;

        status.update(L"test.zip", L"Compressing file:", L"test.txt");

        // Should not crash when called with null pointers
        status.read(nullptr, nullptr, nullptr);

        // Should still clear the dirty flag
        Assert::IsFalse(status.dirty());
    }

    TEST_METHOD (TestReadWithPartialNullPointers) {
        // Test that read() handles partial null pointers
        ArchiveStatus status;

        const std::wstring expectedArchive = L"test.zip";
        const std::wstring expectedOperation = L"Compressing file:";
        const std::wstring expectedFile = L"test.txt";

        status.update(expectedArchive, expectedOperation, expectedFile);

        std::wstring archivePath, operationText;
        status.read(&archivePath, &operationText, nullptr);

        Assert::AreEqual(expectedArchive, archivePath);
        Assert::AreEqual(expectedOperation, operationText);
        Assert::IsFalse(status.dirty());
    }

    TEST_METHOD (TestMultipleUpdates) {
        // Test that multiple updates work correctly
        ArchiveStatus status;

        // First update
        status.update(L"test1.zip", L"Compressing file:", L"file1.txt");
        Assert::IsTrue(status.dirty());

        // Second update
        status.update(L"test2.zip", L"Extracting file:", L"file2.txt");
        Assert::IsTrue(status.dirty());

        // Read should return the latest values
        std::wstring archivePath, operationText, operationFile;
        status.read(&archivePath, &operationText, &operationFile);

        Assert::AreEqual(std::wstring(L"test2.zip"), archivePath);
        Assert::AreEqual(std::wstring(L"Extracting file:"), operationText);
        Assert::AreEqual(std::wstring(L"file2.txt"), operationFile);
        Assert::IsFalse(status.dirty());
    }

    TEST_METHOD (TestEmptyStrings) {
        // Test that empty strings are handled correctly
        ArchiveStatus status;

        status.update(L"", L"", L"");

        std::wstring archivePath, operationText, operationFile;
        status.read(&archivePath, &operationText, &operationFile);

        Assert::AreEqual(std::wstring(L""), archivePath);
        Assert::AreEqual(std::wstring(L""), operationText);
        Assert::AreEqual(std::wstring(L""), operationFile);
    }
};
}  // namespace libwinfile_tests