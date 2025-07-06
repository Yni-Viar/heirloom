#include "libwinfile/pch.h"
#include "ArchiveStatus.h"

namespace libwinfile {

ArchiveStatus::ArchiveStatus() : dirty_(false), progressPercentage_(0.0), hasProgressPercentage_(false) {}

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
    hasProgressPercentage_ = false;
    dirty_ = true;
}

void ArchiveStatus::updateWithProgress(
    const std::wstring& archiveFilePath,
    const std::wstring& operationText,
    const std::wstring& operationFilePath,
    double progressPercentage) {
    std::lock_guard<std::mutex> lock(uiMutex_);
    archiveFilePath_ = archiveFilePath;
    operationText_ = operationText;
    operationFilePath_ = operationFilePath;
    progressPercentage_ = progressPercentage;
    hasProgressPercentage_ = true;
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

void ArchiveStatus::readWithProgress(
    std::wstring* archiveFilePath,
    std::wstring* operationText,
    std::wstring* operationFilePath,
    double* progressPercentage,
    bool* hasProgressPercentage) {
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
    if (progressPercentage) {
        *progressPercentage = progressPercentage_;
    }
    if (hasProgressPercentage) {
        *hasProgressPercentage = hasProgressPercentage_;
    }
    dirty_ = false;
}

}  // namespace libwinfile