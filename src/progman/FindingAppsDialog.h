#pragma once

#include "progman/pch.h"
#include "libprogman/InstalledAppList.h"
#include "libprogman/Shortcut.h"
#include "libprogman/cancel.h"

namespace progman {

class FindingAppsDialog {
   public:
    FindingAppsDialog(HINSTANCE instance, libprogman::InstalledAppList* installedAppList);

    // Creates the worker thread which calls installedAppList_->apps(), which blocks while it works.
    // Creates and shows the modal dialog.
    // When the worker thread gets the apps() result, it sets apps_ and closes the dialog with IDOK result.
    // The result ID is returned from this function, and if it's IDOK, apps() can be called.
    // If the user clicks Cancel or closes the dialog, the apps() call is canceled.
    int showDialog(HWND parent);

    // Returns the apps found by the worker thread.
    immer::vector<std::shared_ptr<libprogman::Shortcut>> const& apps() const;

   private:
    friend INT_PTR CALLBACK FindingAppsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    friend DWORD WINAPI LoadAppsThreadProc(LPVOID lpParameter);

    // Internal method used by the worker thread to execute the app loading
    DWORD executeLoadApps();

    // Request cancellation of the app loading process
    void requestCancellation();

    // Sets the dialog handle for cross-thread communication
    void setDialogHandle(HWND dialogHandle);

    // Injected dependencies
    HINSTANCE instance_;
    libprogman::InstalledAppList* installedAppList_;

    // Returned list
    immer::vector<std::shared_ptr<libprogman::Shortcut>> apps_;

    // Cancellation support
    libprogman::CancellationTokenSource cancellationSource_;

    // Thread handle for the worker thread
    HANDLE threadHandle_;

    // Dialog handle for communicating back to the UI
    HWND dialogHandle_;

    // Result of the dialog operation
    INT_PTR dialogResult_;

    // Mutex to protect access to apps_
    std::mutex mutex_;
};

}  // namespace progman
