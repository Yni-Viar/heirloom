#include "libprogman/pch.h"
#include "libprogman/FolderWatcher.h"

namespace libprogman {

FolderWatcher::FolderWatcher(
    std::filesystem::path folderPath,
    std::function<void()> onChange,
    std::function<void(std::wstring)> onError)
    : folderPath_(std::move(folderPath)),
      onChange_(std::move(onChange)),
      onError_(std::move(onError)),
      running_(true),
      watchThread_([this]() { this->WatchThreadProc(); }) {}

FolderWatcher::~FolderWatcher() noexcept {
    // Signal thread to exit
    running_ = false;

    // Join the thread if it's joinable
    if (watchThread_.joinable()) {
        watchThread_.join();
    }
}

void FolderWatcher::WatchThreadProc() {
    // Open the directory for monitoring with overlapped I/O
    wil::unique_handle dirHandle(CreateFileW(
        folderPath_.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr));

    if (!dirHandle || dirHandle.get() == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::wstring errorMsg = L"Failed to open directory for monitoring. Error code: " + std::to_wstring(error);
        onError_(errorMsg);
        return;
    }

    // Create an event for overlapped I/O
    wil::unique_event ioEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!ioEvent) {
        DWORD error = GetLastError();
        std::wstring errorMsg =
            L"Failed to create event for directory monitoring. Error code: " + std::to_wstring(error);
        onError_(errorMsg);
        return;
    }

    // Buffer for file change information
    constexpr DWORD bufferSize = 4096;
    std::vector<BYTE> buffer(bufferSize);

    // Overlapped structure for I/O
    OVERLAPPED overlapped = {};
    overlapped.hEvent = ioEvent.get();

    // Start the initial async operation
    BOOL readResult = ReadDirectoryChangesW(
        dirHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()),
        TRUE,  // Include subdirectories
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
        nullptr,  // bytesReturned not used for async
        &overlapped, nullptr);

    if (!readResult && GetLastError() != ERROR_IO_PENDING) {
        // Failed to start monitoring
        DWORD error = GetLastError();
        std::wstring errorMsg = L"Failed to start directory monitoring. Error code: " + std::to_wstring(error);
        onError_(errorMsg);
        return;
    }

    while (running_) {
        // Wait for either the I/O to complete or a short timeout to check running_ flag
        DWORD waitResult = WaitForSingleObject(ioEvent.get(), 100);

        if (!running_) {
            // Cleanup: cancel any pending operations
            CancelIoEx(dirHandle.get(), &overlapped);
            break;
        }

        if (waitResult == WAIT_OBJECT_0) {
            // Read completed, check results
            DWORD bytesTransferred = 0;
            BOOL result = GetOverlappedResult(dirHandle.get(), &overlapped, &bytesTransferred, FALSE);

            if (result && bytesTransferred > 0) {
                // Changes detected, invoke callback
                if (onChange_) {
                    onChange_();
                }
            }

            // Reset event for the next operation
            ResetEvent(ioEvent.get());

            // Start the next async operation
            readResult = ReadDirectoryChangesW(
                dirHandle.get(), buffer.data(), static_cast<DWORD>(buffer.size()),
                TRUE,  // Include subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                    FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
                nullptr,  // bytesReturned not used for async
                &overlapped, nullptr);

            if (!readResult && GetLastError() != ERROR_IO_PENDING) {
                // Failed to restart monitoring
                DWORD error = GetLastError();
                std::wstring errorMsg =
                    L"Failed to restart directory monitoring. Error code: " + std::to_wstring(error);
                onError_(errorMsg);
                break;
            }
        } else if (waitResult == WAIT_FAILED) {
            // Wait operation failed
            DWORD error = GetLastError();
            std::wstring errorMsg =
                L"Wait operation failed during directory monitoring. Error code: " + std::to_wstring(error);
            onError_(errorMsg);
            break;
        }
    }
}

}  // namespace libprogman
