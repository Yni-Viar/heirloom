#include "libwinfile/pch.h"
#include "ZipArchive.h"
#include "ArchiveStatus.h"

namespace libwinfile {

namespace {

// Convert std::filesystem::path to UTF-8 string for libzip
std::string pathToUtf8(const std::filesystem::path& path) {
    return path.u8string();
}

// Convert UTF-8 string to std::filesystem::path
std::filesystem::path utf8ToPath(const std::string& utf8String) {
    return std::filesystem::u8path(utf8String);
}

// Recursively collect all files and directories from the given paths
void collectFilesRecursive(
    const std::filesystem::path& path,
    const std::filesystem::path& relativeToPath,
    std::vector<std::pair<std::filesystem::path, std::string>>& fileEntries) {
    if (std::filesystem::is_directory(path)) {
        // Add directory entry
        auto relativePath = std::filesystem::relative(path, relativeToPath);
        std::string zipEntryName = pathToUtf8(relativePath);
        // Ensure directory names end with '/'
        if (!zipEntryName.empty() && zipEntryName.back() != '/') {
            zipEntryName += '/';
        }
        fileEntries.emplace_back(path, zipEntryName);

        // Recursively process directory contents
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_directory()) {
                auto relativePath = std::filesystem::relative(entry.path(), relativeToPath);
                std::string zipEntryName = pathToUtf8(relativePath);
                if (!zipEntryName.empty() && zipEntryName.back() != '/') {
                    zipEntryName += '/';
                }
                fileEntries.emplace_back(entry.path(), zipEntryName);
            } else if (entry.is_regular_file()) {
                auto relativePath = std::filesystem::relative(entry.path(), relativeToPath);
                std::string zipEntryName = pathToUtf8(relativePath);
                fileEntries.emplace_back(entry.path(), zipEntryName);
            }
        }
    } else if (std::filesystem::is_regular_file(path)) {
        // Add file entry
        auto relativePath = std::filesystem::relative(path, relativeToPath);
        std::string zipEntryName = pathToUtf8(relativePath);
        fileEntries.emplace_back(path, zipEntryName);
    }
}

// Add files to zip as we encounter them during directory traversal
void addToZipRecursive(
    zip_t* archive,
    const std::filesystem::path& path,
    const std::filesystem::path& relativeToPath,
    ArchiveStatus* status,
    const std::wstring& zipFilePath) {
    if (std::filesystem::is_directory(path)) {
        // Add directory entry
        auto relativePath = std::filesystem::relative(path, relativeToPath);
        std::string zipEntryName = pathToUtf8(relativePath);
        // Ensure directory names end with '/'
        if (!zipEntryName.empty() && zipEntryName.back() != '/') {
            zipEntryName += '/';
        }

        status->update(zipFilePath, L"Compressing folder:", path.wstring());
        zip_int64_t idx = zip_dir_add(archive, zipEntryName.c_str(), ZIP_FL_ENC_UTF_8);
        if (idx < 0) {
            throw std::runtime_error("Failed to add directory to zip: " + zipEntryName);
        }

        // Recursively process directory contents
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            addToZipRecursive(archive, entry.path(), relativeToPath, status, zipFilePath);
        }
    } else if (std::filesystem::is_regular_file(path)) {
        // Add file entry
        auto relativePath = std::filesystem::relative(path, relativeToPath);
        std::string zipEntryName = pathToUtf8(relativePath);

        status->update(zipFilePath, L"Compressing file:", path.wstring());

        zip_source_t* source = zip_source_file(archive, pathToUtf8(path).c_str(), 0, ZIP_LENGTH_TO_END);
        if (!source) {
            throw std::runtime_error("Failed to create source for file: " + pathToUtf8(path));
        }

        zip_int64_t idx = zip_file_add(archive, zipEntryName.c_str(), source, ZIP_FL_ENC_UTF_8);
        if (idx < 0) {
            zip_source_free(source);
            throw std::runtime_error("Failed to add file to zip: " + zipEntryName);
        }
        // Note: source is managed by libzip after successful zip_file_add
    }
}

}  // anonymous namespace

void createZipArchive(
    const std::filesystem::path& zipFilePath,
    const std::vector<std::filesystem::path>& addFileOrFolderPaths,
    const std::filesystem::path& relativeToPath,
    ArchiveStatus* status) {
    if (!status) {
        throw std::invalid_argument("status parameter cannot be null");
    }

    // Create zip archive
    int error = 0;
    zip_t* archive = zip_open(pathToUtf8(zipFilePath).c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    if (!archive) {
        zip_error_t zipError;
        zip_error_init_with_code(&zipError, error);
        std::string errorMsg = "Failed to create zip archive: " + std::string(zip_error_strerror(&zipError));
        zip_error_fini(&zipError);
        throw std::runtime_error(errorMsg);
    }

    try {
        status->update(zipFilePath.wstring(), L"Starting compression...", L"");

        // Process files as we encounter them instead of collecting them first
        for (const auto& path : addFileOrFolderPaths) {
            addToZipRecursive(archive, path, relativeToPath, status, zipFilePath.wstring());
        }

        // Close archive (this writes the zip file)
        status->update(zipFilePath.wstring(), L"Finalizing archive...", L"");
        if (zip_close(archive) < 0) {
            throw std::runtime_error("Failed to close zip archive: " + std::string(zip_strerror(archive)));
        }

        status->update(zipFilePath.wstring(), L"Compression complete.", L"");
    } catch (...) {
        // Clean up on error
        zip_discard(archive);
        throw;
    }
}

void extractZipArchive(
    const std::filesystem::path& zipFilePath,
    const std::filesystem::path& targetFolder,
    ArchiveStatus* status) {
    if (!status) {
        throw std::invalid_argument("status parameter cannot be null");
    }

    // Open zip archive
    int error = 0;
    zip_t* archive = zip_open(pathToUtf8(zipFilePath).c_str(), ZIP_RDONLY, &error);
    if (!archive) {
        zip_error_t zipError;
        zip_error_init_with_code(&zipError, error);
        std::string errorMsg = "Failed to open zip archive: " + std::string(zip_error_strerror(&zipError));
        zip_error_fini(&zipError);
        throw std::runtime_error(errorMsg);
    }

    try {
        // Get number of entries
        zip_int64_t numEntries = zip_get_num_entries(archive, 0);
        if (numEntries < 0) {
            throw std::runtime_error("Failed to get number of entries in zip archive");
        }

        status->update(zipFilePath.wstring(), L"Starting extraction...", L"");

        // Create target directory if it doesn't exist
        std::filesystem::create_directories(targetFolder);

        // Extract each entry
        for (zip_int64_t i = 0; i < numEntries; ++i) {
            zip_stat_t stat;
            zip_stat_init(&stat);
            if (zip_stat_index(archive, i, 0, &stat) < 0) {
                throw std::runtime_error("Failed to get file info for entry " + std::to_string(i));
            }

            std::string entryName = stat.name;
            std::filesystem::path entryPath = targetFolder / utf8ToPath(entryName);

            // Update progress
            std::wstring progressText =
                L"Extracting file: (" + std::to_wstring(i + 1) + L"/" + std::to_wstring(numEntries) + L")";
            status->update(zipFilePath.wstring(), progressText, entryPath.wstring());

            // Check if it's a directory (ends with '/')
            if (!entryName.empty() && entryName.back() == '/') {
                // Create directory
                std::filesystem::create_directories(entryPath);
            } else {
                // Create parent directories if needed
                std::filesystem::create_directories(entryPath.parent_path());

                // Extract file
                zip_file_t* file = zip_fopen_index(archive, i, 0);
                if (!file) {
                    throw std::runtime_error("Failed to open file in zip: " + entryName);
                }

                try {
                    // Ensure we can overwrite the file if it exists
                    if (std::filesystem::exists(entryPath)) {
                        std::filesystem::permissions(
                            entryPath, std::filesystem::perms::owner_write, std::filesystem::perm_options::add);
                    }

                    // Create output file
                    std::ofstream outFile(entryPath, std::ios::binary | std::ios::trunc);
                    if (!outFile.is_open()) {
                        throw std::runtime_error("Failed to create output file: " + pathToUtf8(entryPath));
                    }

                    // Copy data
                    constexpr size_t bufferSize = 8192;
                    char buffer[bufferSize];
                    zip_int64_t bytesRead;
                    while ((bytesRead = zip_fread(file, buffer, bufferSize)) > 0) {
                        outFile.write(buffer, bytesRead);
                        if (outFile.fail()) {
                            throw std::runtime_error("Failed to write to output file: " + pathToUtf8(entryPath));
                        }
                    }

                    if (bytesRead < 0) {
                        throw std::runtime_error("Failed to read from zip file: " + entryName);
                    }

                    outFile.close();
                } catch (...) {
                    zip_fclose(file);
                    throw;
                }

                zip_fclose(file);
            }
        }

        status->update(zipFilePath.wstring(), L"Extraction complete.", L"");
    } catch (...) {
        zip_close(archive);
        throw;
    }

    zip_close(archive);
}

}  // namespace libwinfile