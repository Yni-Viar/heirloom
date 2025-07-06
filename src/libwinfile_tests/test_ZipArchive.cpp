#include "pch.h"
#include "CppUnitTest.h"
#include "libwinfile/ZipArchive.h"
#include "libwinfile/ArchiveStatus.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libwinfile_tests {

TEST_CLASS (ZipArchiveTests) {
    std::filesystem::path tempDir_;
    std::filesystem::path testDataDir_;

    TEST_METHOD_INITIALIZE(SetUp) {
        // Create a temporary directory for testing
        tempDir_ = std::filesystem::temp_directory_path() / "libwinfile_zip_test";
        std::filesystem::create_directories(tempDir_);

        // Create a test data directory
        testDataDir_ = tempDir_ / "testdata";
        std::filesystem::create_directories(testDataDir_);
    }

    TEST_METHOD_CLEANUP(TearDown) {
        // Clean up test files
        std::error_code ec;
        std::filesystem::remove_all(tempDir_, ec);
        // Don't throw if cleanup fails
    }

    void CreateTestFile(const std::filesystem::path& filePath, const std::string& content) {
        std::filesystem::create_directories(filePath.parent_path());
        std::ofstream file(filePath);
        Assert::IsTrue(file.is_open(), L"Failed to create test file");
        file << content;
        file.close();
    }

    std::string ReadFileContent(const std::filesystem::path& filePath) {
        std::ifstream file(filePath);
        Assert::IsTrue(file.is_open(), L"Failed to open file for reading");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

    TEST_METHOD (CreateZipArchive_SingleFile) {
        // Arrange
        auto testFile = testDataDir_ / "test.txt";
        CreateTestFile(testFile, "Hello, World!");

        auto zipFile = tempDir_ / "test.zip";
        std::vector<std::filesystem::path> filesToAdd = { testFile };
        libwinfile::ArchiveStatus status;

        // Act
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Assert
        Assert::IsTrue(std::filesystem::exists(zipFile), L"Zip file was not created");
        Assert::IsTrue(std::filesystem::file_size(zipFile) > 0, L"Zip file is empty");
    }

    TEST_METHOD (CreateZipArchive_MultipleFiles) {
        // Arrange
        auto file1 = testDataDir_ / "file1.txt";
        auto file2 = testDataDir_ / "file2.txt";
        CreateTestFile(file1, "Content of file 1");
        CreateTestFile(file2, "Content of file 2");

        auto zipFile = tempDir_ / "multiple.zip";
        std::vector<std::filesystem::path> filesToAdd = { file1, file2 };
        libwinfile::ArchiveStatus status;

        // Act
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Assert
        Assert::IsTrue(std::filesystem::exists(zipFile), L"Zip file was not created");
        Assert::IsTrue(std::filesystem::file_size(zipFile) > 0, L"Zip file is empty");
    }

    TEST_METHOD (CreateZipArchive_WithDirectories) {
        // Arrange
        auto subDir = testDataDir_ / "subdir";
        auto fileInSubDir = subDir / "nested.txt";
        CreateTestFile(fileInSubDir, "Nested file content");

        auto zipFile = tempDir_ / "with_dirs.zip";
        std::vector<std::filesystem::path> filesToAdd = { subDir };
        libwinfile::ArchiveStatus status;

        // Act
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Assert
        Assert::IsTrue(std::filesystem::exists(zipFile), L"Zip file was not created");
        Assert::IsTrue(std::filesystem::file_size(zipFile) > 0, L"Zip file is empty");
    }

    TEST_METHOD (CreateZipArchive_NullStatus_ThrowsException) {
        // Arrange
        auto testFile = testDataDir_ / "test.txt";
        CreateTestFile(testFile, "Hello, World!");

        auto zipFile = tempDir_ / "test.zip";
        std::vector<std::filesystem::path> filesToAdd = { testFile };

        // Act & Assert
        Assert::ExpectException<std::invalid_argument>(
            [&]() { libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, nullptr); });
    }

    TEST_METHOD (ExtractZipArchive_SingleFile) {
        // Arrange
        auto testFile = testDataDir_ / "test.txt";
        std::string testContent = "Hello, World!";
        CreateTestFile(testFile, testContent);

        auto zipFile = tempDir_ / "test.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { testFile };
        libwinfile::ArchiveStatus status;

        // Create zip
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Act
        libwinfile::extractZipArchive(zipFile, extractDir, &status);

        // Assert
        auto extractedFile = extractDir / "test.txt";
        Assert::IsTrue(std::filesystem::exists(extractedFile), L"Extracted file does not exist");
        Assert::AreEqual(testContent, ReadFileContent(extractedFile), L"Extracted content does not match");
    }

    TEST_METHOD (ExtractZipArchive_MultipleFiles) {
        // Arrange
        auto file1 = testDataDir_ / "file1.txt";
        auto file2 = testDataDir_ / "file2.txt";
        std::string content1 = "Content of file 1";
        std::string content2 = "Content of file 2";
        CreateTestFile(file1, content1);
        CreateTestFile(file2, content2);

        auto zipFile = tempDir_ / "multiple.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { file1, file2 };
        libwinfile::ArchiveStatus status;

        // Create zip
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Act
        libwinfile::extractZipArchive(zipFile, extractDir, &status);

        // Assert
        auto extractedFile1 = extractDir / "file1.txt";
        auto extractedFile2 = extractDir / "file2.txt";
        Assert::IsTrue(std::filesystem::exists(extractedFile1), L"Extracted file1 does not exist");
        Assert::IsTrue(std::filesystem::exists(extractedFile2), L"Extracted file2 does not exist");
        Assert::AreEqual(content1, ReadFileContent(extractedFile1), L"Extracted content1 does not match");
        Assert::AreEqual(content2, ReadFileContent(extractedFile2), L"Extracted content2 does not match");
    }

    TEST_METHOD (ExtractZipArchive_WithDirectories) {
        // Arrange
        auto subDir = testDataDir_ / "subdir";
        auto fileInSubDir = subDir / "nested.txt";
        std::string nestedContent = "Nested file content";
        CreateTestFile(fileInSubDir, nestedContent);

        auto zipFile = tempDir_ / "with_dirs.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { subDir };
        libwinfile::ArchiveStatus status;

        // Create zip
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Act
        libwinfile::extractZipArchive(zipFile, extractDir, &status);

        // Assert
        auto extractedFile = extractDir / "subdir" / "nested.txt";
        Assert::IsTrue(std::filesystem::exists(extractedFile), L"Extracted nested file does not exist");
        Assert::AreEqual(nestedContent, ReadFileContent(extractedFile), L"Extracted nested content does not match");
    }

    TEST_METHOD (ExtractZipArchive_NullStatus_ThrowsException) {
        // Arrange
        auto testFile = testDataDir_ / "test.txt";
        CreateTestFile(testFile, "Hello, World!");

        auto zipFile = tempDir_ / "test.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { testFile };
        libwinfile::ArchiveStatus status;

        // Create zip
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Act & Assert
        Assert::ExpectException<std::invalid_argument>(
            [&]() { libwinfile::extractZipArchive(zipFile, extractDir, nullptr); });
    }

    TEST_METHOD (ExtractZipArchive_NonExistentFile_ThrowsException) {
        // Arrange
        auto nonExistentZip = tempDir_ / "does_not_exist.zip";
        auto extractDir = tempDir_ / "extract";
        libwinfile::ArchiveStatus status;

        // Act & Assert
        Assert::ExpectException<std::runtime_error>(
            [&]() { libwinfile::extractZipArchive(nonExistentZip, extractDir, &status); });
    }

    TEST_METHOD (CreateAndExtractZipArchive_ComplexStructure) {
        // Arrange - Create a complex directory structure
        auto dir1 = testDataDir_ / "dir1";
        auto dir2 = testDataDir_ / "dir2";
        auto subDir = dir1 / "subdir";

        CreateTestFile(dir1 / "file1.txt", "File 1 content");
        CreateTestFile(dir2 / "file2.txt", "File 2 content");
        CreateTestFile(subDir / "nested.txt", "Nested file content");
        CreateTestFile(subDir / "another.txt", "Another nested file");

        auto zipFile = tempDir_ / "complex.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { dir1, dir2 };
        libwinfile::ArchiveStatus status;

        // Act
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);
        libwinfile::extractZipArchive(zipFile, extractDir, &status);

        // Assert
        Assert::IsTrue(std::filesystem::exists(extractDir / "dir1" / "file1.txt"), L"dir1/file1.txt not extracted");
        Assert::IsTrue(std::filesystem::exists(extractDir / "dir2" / "file2.txt"), L"dir2/file2.txt not extracted");
        Assert::IsTrue(
            std::filesystem::exists(extractDir / "dir1" / "subdir" / "nested.txt"),
            L"dir1/subdir/nested.txt not extracted");
        Assert::IsTrue(
            std::filesystem::exists(extractDir / "dir1" / "subdir" / "another.txt"),
            L"dir1/subdir/another.txt not extracted");

        // Verify content
        Assert::AreEqual(
            std::string("File 1 content"), ReadFileContent(extractDir / "dir1" / "file1.txt"),
            L"File 1 content mismatch");
        Assert::AreEqual(
            std::string("File 2 content"), ReadFileContent(extractDir / "dir2" / "file2.txt"),
            L"File 2 content mismatch");
        Assert::AreEqual(
            std::string("Nested file content"), ReadFileContent(extractDir / "dir1" / "subdir" / "nested.txt"),
            L"Nested file content mismatch");
        Assert::AreEqual(
            std::string("Another nested file"), ReadFileContent(extractDir / "dir1" / "subdir" / "another.txt"),
            L"Another nested file content mismatch");
    }

    TEST_METHOD (ArchiveStatus_Updates_During_Operations) {
        // Arrange
        auto testFile = testDataDir_ / "test.txt";
        CreateTestFile(testFile, "Hello, World!");

        auto zipFile = tempDir_ / "test.zip";
        auto extractDir = tempDir_ / "extract";
        std::vector<std::filesystem::path> filesToAdd = { testFile };
        libwinfile::ArchiveStatus status;

        // Act & Assert - Create
        libwinfile::createZipArchive(zipFile, filesToAdd, testDataDir_, &status);

        // Status should have been updated
        Assert::IsTrue(status.dirty(), L"Status should be dirty after creation");

        std::wstring archivePath, operationText, operationFilePath;
        status.read(&archivePath, &operationText, &operationFilePath);
        Assert::IsFalse(status.dirty(), L"Status should not be dirty after reading");
        Assert::AreEqual(zipFile.wstring(), archivePath, L"Archive path should match");
        Assert::AreEqual(
            std::wstring(L"Compression complete."), operationText, L"Operation text should indicate completion");

        // Act & Assert - Extract
        libwinfile::extractZipArchive(zipFile, extractDir, &status);

        // Status should have been updated again
        Assert::IsTrue(status.dirty(), L"Status should be dirty after extraction");

        status.read(&archivePath, &operationText, &operationFilePath);
        Assert::AreEqual(
            std::wstring(L"Extraction complete."), operationText,
            L"Operation text should indicate extraction completion");
    }
};
}  // namespace libwinfile_tests