#include "archiveprogress.h"
#include "resource.h"
#include <stdexcept>
#include <atomic>
#include <memory>
#include "libheirloom/cancel.h"
#include "libwinfile/ArchiveStatus.h"

ArchiveProgressDialog::ArchiveProgressDialog(
    std::function<void(libheirloom::CancellationToken)> callback,
    libwinfile::ArchiveStatus* status)
    : callback_(std::move(callback)),
      cancellationTokenSource_(std::make_unique<libheirloom::CancellationTokenSource>()),
      status_(status),
      dialogHandle_(nullptr),
      dialogResult_(0),
      exception_(nullptr),
      workerThreadFinished_(false) {
    // Initialize UI strings via the status object
    status_->update(L"", L"Starting...", L"");
}

int ArchiveProgressDialog::showDialog(HWND owner) {
    // Show the dialog modally
    return static_cast<int>(DialogBoxParamW(
        GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_ARCHIVE_PROGRESS), owner, ArchiveProgressDialogProc,
        reinterpret_cast<LPARAM>(this)));
}

std::exception_ptr ArchiveProgressDialog::getException() const {
    return exception_;
}

BOOL ArchiveProgressDialog::onInitDialog(HWND hDlg) {
    dialogHandle_ = hDlg;

    // Set initial text from ArchiveStatus
    std::wstring archiveFilePath, operationText, operationFilePath;
    status_->read(&archiveFilePath, &operationText, &operationFilePath);

    SetDlgItemTextW(hDlg, IDC_ARCHIVE_PATH, archiveFilePath.c_str());
    SetDlgItemTextW(hDlg, IDC_OPERATION_LABEL, operationText.c_str());
    SetDlgItemTextW(hDlg, IDC_OPERATION_FILE, operationFilePath.c_str());

    // Start the UI update timer
    SetTimer(hDlg, TIMER_ID, TIMER_INTERVAL, nullptr);

    // Start the worker thread
    startWorkerThread(hDlg);

    return TRUE;
}

BOOL ArchiveProgressDialog::onCommand(HWND hDlg, WPARAM wParam) {
    switch (LOWORD(wParam)) {
        case IDC_ARCHIVE_CANCEL:
            // Request cancellation
            cancellationTokenSource_->cancel();

            // Disable the cancel button to prevent multiple clicks
            EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_CANCEL), FALSE);

            return TRUE;
    }
    return FALSE;
}

BOOL ArchiveProgressDialog::onClose(HWND hDlg) {
    // Don't allow closing while operation is in progress
    if (!workerThreadFinished_) {
        // Request cancellation instead
        cancellationTokenSource_->cancel();
        EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_CANCEL), FALSE);
        return TRUE;  // Prevent closing
    }
    return FALSE;  // Allow closing
}

BOOL ArchiveProgressDialog::onTimer(HWND hDlg) {
    // Update UI from worker thread data
    updateUIFromWorkerThread();

    // Check if worker thread has finished
    if (workerThreadFinished_) {
        // Determine the result
        int result = IDOK;
        if (exception_) {
            result = IDABORT;
        } else if (cancellationTokenSource_->isCancellationRequested()) {
            result = IDCANCEL;
        }

        // End the dialog
        endDialog(hDlg, result);
        return TRUE;
    }

    return TRUE;
}

BOOL ArchiveProgressDialog::onDestroy(HWND hDlg) {
    // Kill the timer
    KillTimer(hDlg, TIMER_ID);

    // Wait for worker thread to finish if it's still running
    if (workerThread_ && workerThread_->joinable()) {
        // Request cancellation if not already requested
        if (!cancellationTokenSource_->isCancellationRequested()) {
            cancellationTokenSource_->cancel();
        }

        // Wait for thread to finish
        workerThread_->join();
    }

    dialogHandle_ = nullptr;
    return TRUE;
}

void ArchiveProgressDialog::startWorkerThread(HWND hDlg) {
    workerThread_ = std::make_unique<std::thread>([this]() { workerThreadFunction(); });
}

void ArchiveProgressDialog::updateUIFromWorkerThread() {
    // Check if the status has been updated
    if (status_->dirty()) {
        std::wstring archiveFilePath, operationText, operationFilePath;
        status_->read(&archiveFilePath, &operationText, &operationFilePath);

        if (dialogHandle_) {
            // Update the labels with current values
            SetDlgItemTextW(dialogHandle_, IDC_ARCHIVE_PATH, archiveFilePath.c_str());
            SetDlgItemTextW(dialogHandle_, IDC_OPERATION_LABEL, operationText.c_str());
            SetDlgItemTextW(dialogHandle_, IDC_OPERATION_FILE, operationFilePath.c_str());
        }
    }
}

void ArchiveProgressDialog::endDialog(HWND hDlg, int result) {
    dialogResult_ = result;
    EndDialog(hDlg, result);
}

void ArchiveProgressDialog::workerThreadFunction() {
    try {
        // Create the cancellation token
        auto token = cancellationTokenSource_->createToken();

        // Call the user's callback
        callback_(token);

        // Mark as finished
        workerThreadFinished_ = true;

    } catch (...) {
        // Capture any exception that occurred
        exception_ = std::current_exception();
        workerThreadFinished_ = true;
    }
}

INT_PTR CALLBACK
ArchiveProgressDialog::ArchiveProgressDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    ArchiveProgressDialog* dlg = nullptr;

    if (message == WM_INITDIALOG) {
        // Store the pointer to our object
        dlg = reinterpret_cast<ArchiveProgressDialog*>(lParam);
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(dlg));
        return dlg->onInitDialog(hDlg);
    }

    // Retrieve the pointer to our dialog object
    dlg = reinterpret_cast<ArchiveProgressDialog*>(GetWindowLongPtrW(hDlg, GWLP_USERDATA));
    if (!dlg) {
        return FALSE;
    }

    switch (message) {
        case WM_COMMAND:
            return dlg->onCommand(hDlg, wParam);

        case WM_CLOSE:
            return dlg->onClose(hDlg);

        case WM_TIMER:
            if (wParam == TIMER_ID) {
                return dlg->onTimer(hDlg);
            }
            break;

        case WM_DESTROY:
            return dlg->onDestroy(hDlg);
    }

    return FALSE;
}