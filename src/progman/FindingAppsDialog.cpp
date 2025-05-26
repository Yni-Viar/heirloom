#include "progman/pch.h"
#include "progman/FindingAppsDialog.h"
#include "progman/resource.h"
#include "libprogman/window_data.h"
#include "libprogman/cancel.h"
#include "libprogman/constants.h"

namespace progman {

// Message struct for error display
struct ErrorMessageData {
    std::wstring message;
};

// Dialog window procedure
INT_PTR CALLBACK FindingAppsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    auto dialog = libprogman::getWindowData<progman::FindingAppsDialog>(hDlg);

    switch (message) {
        case WM_INITDIALOG: {
            FindingAppsDialog* dialog = reinterpret_cast<FindingAppsDialog*>(lParam);
            libprogman::setWindowData<progman::FindingAppsDialog>(hDlg, dialog);
            // Store the dialog handle for thread communication
            dialog->setDialogHandle(hDlg);
            return TRUE;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL) {
                // Disable the cancel button to indicate cancellation is in progress
                EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
                dialog->requestCancellation();
                // Don't end the dialog here - let the worker thread end it
                // after the cancellation is processed
                return TRUE;
            } else if (LOWORD(wParam) == IDOK) {
                // This is sent from the worker thread when it completes successfully
                EndDialog(hDlg, IDOK);
                return TRUE;
            } else if (LOWORD(wParam) == IDABORT) {
                // This is sent from the worker thread when it's canceled
                EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }
            break;
        case WM_CLOSE:
            // Treat window close as cancellation
            EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
            dialog->requestCancellation();
            // Don't end the dialog here - let the worker thread end it
            // after the cancellation is processed
            return TRUE;
        case libprogman::WM_DISPLAY_ERROR: {
            // Display error message sent from worker thread
            auto* errorData = reinterpret_cast<ErrorMessageData*>(wParam);
            MessageBoxW(hDlg, errorData->message.c_str(), L"Error", MB_ICONERROR | MB_OK);
            delete errorData;
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

// Thread function for loading apps
DWORD WINAPI LoadAppsThreadProc(LPVOID lpParameter) {
    auto* dialog = static_cast<FindingAppsDialog*>(lpParameter);
    return dialog->executeLoadApps();
}

FindingAppsDialog::FindingAppsDialog(HINSTANCE instance, libprogman::InstalledAppList* installedAppList)
    : instance_(instance),
      installedAppList_(installedAppList),
      apps_{},
      cancellationSource_{},
      threadHandle_{ nullptr },
      dialogHandle_{ nullptr },
      dialogResult_{ IDCANCEL },
      mutex_{} {}

int FindingAppsDialog::showDialog(HWND parent) {
    // Create the worker thread
    threadHandle_ = CreateThread(nullptr, 0, LoadAppsThreadProc, this, 0, nullptr);

    if (!threadHandle_) {
        MessageBoxW(parent, L"Failed to create worker thread", L"Error", MB_ICONERROR | MB_OK);
        return IDCANCEL;
    }

    // Show the dialog modal
    INT_PTR result = DialogBoxParamW(
        instance_, MAKEINTRESOURCEW(IDD_FINDING_APPS), parent, FindingAppsDialogProc, reinterpret_cast<LPARAM>(this));

    // Wait for thread to complete
    WaitForSingleObject(threadHandle_, INFINITE);
    CloseHandle(threadHandle_);
    threadHandle_ = nullptr;

    return static_cast<int>(result);
}

void FindingAppsDialog::setDialogHandle(HWND dialogHandle) {
    dialogHandle_ = dialogHandle;
}

DWORD FindingAppsDialog::executeLoadApps() {
    try {
        // Create cancellation token for the operation
        auto token = cancellationSource_.createToken();

        // Get the apps list
        auto loadedApps = installedAppList_->apps(token);

        // Store the result
        {
            std::lock_guard<std::mutex> lock(mutex_);
            apps_ = std::move(loadedApps);
        }

        // Signal success by ending the dialog
        PostMessage(dialogHandle_, WM_COMMAND, IDOK, 0);
        return 0;
    } catch (const libprogman::OperationCanceledException&) {
        // Operation was canceled by user, close the dialog with IDCANCEL
        // Use IDABORT as a special value to tell the dialog proc to end with IDCANCEL
        PostMessage(dialogHandle_, WM_COMMAND, IDABORT, 0);
        return 1;
    } catch (const std::exception& e) {
        // Convert narrow string to wide string for MessageBox
        std::string errorMsg = "Error loading applications: ";
        errorMsg += e.what();

        // Create error data and pass it to the UI thread
        auto* errorData = new ErrorMessageData();

        // Convert the error message to wide string
        int size = MultiByteToWideChar(CP_UTF8, 0, errorMsg.c_str(), -1, nullptr, 0);
        errorData->message.resize(size - 1);  // -1 because size includes null terminator, but wstring handles that
        MultiByteToWideChar(CP_UTF8, 0, errorMsg.c_str(), -1, &errorData->message[0], size);

        // Send the error message to the UI thread
        SendMessage(dialogHandle_, libprogman::WM_DISPLAY_ERROR, reinterpret_cast<WPARAM>(errorData), 0);

        return 1;
    }
}

void FindingAppsDialog::requestCancellation() {
    cancellationSource_.cancel();
}

immer::vector<std::shared_ptr<libprogman::Shortcut>> const& FindingAppsDialog::apps() const {
    return apps_;
}

}  // namespace progman
