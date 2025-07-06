#pragma once

#include <mutex>
#include <string>

namespace libwinfile {

class ArchiveStatus {
   private:
    std::mutex uiMutex_;
    std::wstring archiveFilePath_;
    std::wstring operationText_;
    std::wstring operationFilePath_;
    bool dirty_;
    double progressPercentage_;
    bool hasProgressPercentage_;

   public:
    ArchiveStatus();

    bool dirty();

    void update(
        const std::wstring& archiveFilePath,
        const std::wstring& operationText,
        const std::wstring& operationFilePath);

    void updateWithProgress(
        const std::wstring& archiveFilePath,
        const std::wstring& operationText,
        const std::wstring& operationFilePath,
        double progressPercentage);

    void read(std::wstring* archiveFilePath, std::wstring* operationText, std::wstring* operationFilePath);

    void readWithProgress(
        std::wstring* archiveFilePath,
        std::wstring* operationText,
        std::wstring* operationFilePath,
        double* progressPercentage,
        bool* hasProgressPercentage);
};

}  // namespace libwinfile