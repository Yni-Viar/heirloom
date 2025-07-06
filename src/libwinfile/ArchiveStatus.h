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

   public:
    ArchiveStatus();

    bool dirty();

    void update(
        const std::wstring& archiveFilePath,
        const std::wstring& operationText,
        const std::wstring& operationFilePath);

    void read(std::wstring* archiveFilePath, std::wstring* operationText, std::wstring* operationFilePath);
};

}  // namespace libwinfile