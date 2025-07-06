#include "libwinfile/pch.h"
#include "ArchiveStatus.h"

namespace libwinfile {

ArchiveStatus::ArchiveStatus() : dirty_(false) {}

bool ArchiveStatus::dirty() {
    std::lock_guard<std::mutex> lock(uiMutex_);
    return dirty_;
}

void ArchiveStatus::update(
    const std::wstring& archiveFilePath,
    const std::wstring& operationText,
    const std::wstring& operationFilePath) {
    std::lock_guard<std::mutex> lock(uiMutex_);
    archiveFilePath_ = archiveFilePath;
    operationText_ = operationText;
    operationFilePath_ = operationFilePath;
    dirty_ = true;
}

void ArchiveStatus::read(std::wstring* archiveFilePath, std::wstring* operationText, std::wstring* operationFilePath) {
    std::lock_guard<std::mutex> lock(uiMutex_);
    if (archiveFilePath) {
        *archiveFilePath = archiveFilePath_;
    }
    if (operationText) {
        *operationText = operationText_;
    }
    if (operationFilePath) {
        *operationFilePath = operationFilePath_;
    }
    dirty_ = false;
}

}  // namespace libwinfile