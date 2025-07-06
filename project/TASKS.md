# Zip integration using libzip (winfile)

Search the libzip docs in `context/libzip/` for guidance on library usage.
If you need to #include new system headers, add them to `winfile.h`.

- [x] Prepare a dialog resource `IDM_ARCHIVE_PROGRESS` and a C++ class `ArchiveProgressDialog` for a zip/unzip operation progress dialog, following the example of `IDD_BOOKMARK_EDIT`.
    - Dialog resource
        - Title "Archive Progress"
        - Vertical stack
            - Static "Archive file:" 
            - Static, path to the zip file. Initially "" in the resource.
            - A gap
            - Static "Compressing file:" or "Extracting file:" depending on the operation. Initially "Starting..." in the resource.
            - Static, path to the current operation file. Initially "" in the resource.
            - A gap
            - Right-aligned "Cancel" button
    - C++ class `ArchiveProgressDialog`
        - Ctor takes a std::function callback. Arguments: `ArchiveProgressDialog*, CancellationToken`.
        - When the window is shown, a thread is started to run the callback. If the user cancels or tries to closes the window, deny closing the window, disable the cancel button, and request cancellation from the `CancellationTokenSource`. The dialog will be closed when the callback returns in all situations.
        - When the window is shown, a 100ms interval timer on the UI thread starts will update the UI. It will update the archive file path, operation text, and operation file path labels from private string member variables protected with a mutex. Timer ends when the window closes. This timer provides the way that the worker thread's updates will make it to the UI.
        - When the callback ends, if cancellation was requested, dialog result `IDCANCEL`. If the callback succeeded, dialog result `IDOK`. If the callback threw an exception, we catch it and expose it from a getter in `ArchiveProgressDialog` then set dialog result to `IDABORT`. Regardless, close the dialog.
        - Member function for setting the private member variables: archive file path string, operation text ("Compressing file:" or "Extracting file:" e.g.), operation file path string. Take the mutex when setting these. This function will be called from a worker thread.
    - 🤖 *Implemented dialog resource `IDD_ARCHIVE_PROGRESS` with IDs 2500-2505, created `ArchiveProgressDialog` class with full thread management, mutex-protected UI updates, cancellation support via libheirloom, and proper exception handling. Added project references to libheirloom and updated winfile documentation.*

- [ ] In libwinfile, create class `ArchiveStatus`.
    ```
    std::mutex uiMutex_;
    std::wstring archiveFilePath_;
    std::wstring operationText_;
    std::wstring operationFilePath_;
    bool dirty_;
    ```
    - [ ] Member function `bool dirty()` accessing `dirty_`
    - [ ] Member function `void update(const std::wstring& archiveFilePath, const std::wstring& operationText, const std::wstring& operationFilePath)` -- takes mutex and sets fields, sets dirty bit
    - [ ] Member function `void read(std::wstring* archiveFilePath, std::wstring* operationText, std::wstring* operationFilePath)` -- takes mutex, copies out strings from private fields, clears dirty bit
    Instead, `ArchiveProgressDialog`'s ctor will receive a pointer to an `ArchiveStatus` and it will access these values (including the mutex) through it.
    - [ ] Write unit tests in `libwinfile_tests\test_ArchiveStatus.cpp`.
    - [ ] Update `ArchiveProgressDialog`. Its ctor will receive `ArchiveStatus*`. It will not have the mutex or the three strings as fields, instead it will keep the status pointer. The timer will check `dirty()` to see if anything has changed, and if so, it calls `read()` to get the updated text.
