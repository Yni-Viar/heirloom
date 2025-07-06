#pragma once

#include <windows.h>
#include <functional>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <stdexcept>

// Forward declaration of cancellation types to avoid including libheirloom headers
// We'll include the actual header in the implementation file where it's needed
namespace libheirloom {
class CancellationToken;
class CancellationTokenSource;
}  // namespace libheirloom

// Forward declaration of ArchiveStatus to avoid including libwinfile headers
namespace libwinfile {
class ArchiveStatus;
}  // namespace libwinfile

// Archive Progress dialog for zip/unzip operations.
// Shows the current archive file path, operation type, and current file being processed.
// Supports cancellation via CancellationTokenSource.
class ArchiveProgressDialog {
   public:
    // Constructor takes a callback function that will be called on a worker thread.
    // The callback receives the dialog instance and a cancellation token.
    ArchiveProgressDialog(
        std::function<void(libheirloom::CancellationToken)> callback,
        libwinfile::ArchiveStatus* status);

    // Shows the dialog modal to the owner window.
    // Returns IDOK if operation succeeded, IDCANCEL if cancelled, IDABORT if exception occurred.
    int showDialog(HWND owner);

    // Gets the exception that occurred during the callback, if any.
    std::exception_ptr getException() const;

   private:
    // Handler methods for dialog messages
    BOOL onInitDialog(HWND hDlg);
    BOOL onCommand(HWND hDlg, WPARAM wParam);
    BOOL onClose(HWND hDlg);
    BOOL onTimer(HWND hDlg);
    BOOL onDestroy(HWND hDlg);

    // Helper methods
    void startWorkerThread(HWND hDlg);
    void updateUIFromWorkerThread();
    void endDialog(HWND hDlg, int result);

    static INT_PTR CALLBACK ArchiveProgressDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // Worker thread function
    void workerThreadFunction();

    // Callback function to execute
    std::function<void(libheirloom::CancellationToken)> callback_;

    // Cancellation support (using opaque pointer to avoid header dependency)
    std::unique_ptr<libheirloom::CancellationTokenSource> cancellationTokenSource_;

    // Worker thread
    std::unique_ptr<std::thread> workerThread_;

    // Archive status (not owned by this class)
    libwinfile::ArchiveStatus* status_;

    // Dialog state
    HWND dialogHandle_;
    std::atomic<int> dialogResult_;
    std::exception_ptr exception_;
    std::atomic<bool> workerThreadFinished_;

    // Timer for UI updates
    static constexpr UINT_PTR TIMER_ID = 1;
    static constexpr UINT TIMER_INTERVAL = 100;  // 100ms
};