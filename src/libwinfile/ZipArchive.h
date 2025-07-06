#pragma once

#include <filesystem>
#include <vector>

namespace libwinfile {

class ArchiveStatus;

// Creates a zip file where the entries are the given files or folder paths (recursive),
// and the zip entry full names are relative to relativeToPath.
// For instance, if addFileOrFolderPaths contains C:\Foo\Bar\Baz.txt and relativeToPath is C:\Foo,
// then the zip entry is Bar\Baz.txt.
// Reports progress via status->update().
void createZipArchive(
    const std::filesystem::path& zipFilePath,
    const std::vector<std::filesystem::path>& addFileOrFolderPaths,
    const std::filesystem::path& relativeToPath,
    ArchiveStatus* status);

// Extracts the zip to the target folder.
// Reports progress via status->update().
void extractZipArchive(
    const std::filesystem::path& zipFilePath,
    const std::filesystem::path& targetFolder,
    ArchiveStatus* status);

}  // namespace libwinfile