# Zip integration using libzip (winfile)

Search the libzip docs in `context/libzip/` for guidance on library usage.
If you need to #include new system headers, add them to `winfile.h`.
In libwinfile, use namespace `libwinfile`.

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

- [x] In libwinfile, create class `ArchiveStatus`.
    ```
    std::mutex uiMutex_;
    std::wstring archiveFilePath_;
    std::wstring operationText_;
    std::wstring operationFilePath_;
    bool dirty_;
    ```
    - [x] Member function `bool dirty()` accessing `dirty_`
    - [x] Member function `void update(const std::wstring& archiveFilePath, const std::wstring& operationText, const std::wstring& operationFilePath)` -- takes mutex and sets fields, sets dirty bit
    - [x] Member function `void read(std::wstring* archiveFilePath, std::wstring* operationText, std::wstring* operationFilePath)` -- takes mutex, copies out strings from private fields, clears dirty bit
    Instead, `ArchiveProgressDialog`'s ctor will receive a pointer to an `ArchiveStatus` and it will access these values (including the mutex) through it.
    - [x] Write unit tests in `libwinfile_tests\test_ArchiveStatus.cpp`.
    - [x] Update `ArchiveProgressDialog`. Its ctor will receive `ArchiveStatus*`. It will not have the mutex or the three strings as fields, instead it will keep the status pointer. The timer will check `dirty()` to see if anything has changed, and if so, it calls `read()` to get the updated text.
    - 🤖 *Implemented ArchiveStatus class in libwinfile with mutex-protected fields and dirty bit tracking. Created comprehensive unit tests covering all functionality including thread safety, null pointer handling, and edge cases. Updated ArchiveProgressDialog to use ArchiveStatus for thread-safe UI updates with dirty bit checking in the timer. All tests pass successfully.*

- [x] In libwinfile, implement zip related functions, and write unit tests in `libwinfile_tests`. Update vcxproj accordingly when adding new files. In your test, don't mock anything, really do create test files and test archives in a temp directory, making sure to delete them afterwards, for an end-to-end real world test.
    - [x] `void createZipArchive(const std::filesystem::path& zipFilePath, const std::vector<std::filesystem::path>& addFileOrFolderPaths, const std::filesystem::path& relativeToPath, ArchiveStatus* status)` -- creates a zip file where the entries are the given files or folder paths (recursive), and the zip entry full names are relative to `relativeToPath`. For instance, if `addFileOrFolderPaths` contains `C:\Foo\Bar\Baz.txt` and `relativeToPath` is `C:\Foo`, then the zip entry is `Bar\Baz.txt`. Report progress via `status->update()`.
    - [x] `void extractZipArchive(const std::filesystem::path& zipFilePath, const std::filesystem::path& targetFolder, ArchiveStatus* status)` -- extracts the zip to the target folder. Report progress via `status->update()`.
    - 🤖 *Implemented complete ZIP archive functionality in libwinfile with ZipArchive.h/cpp. Created createZipArchive() function that handles recursive directory traversal, proper UTF-8 path conversion, and relative path calculation for ZIP entry names. Created extractZipArchive() function that handles directory creation, file extraction, and path reconstruction. Both functions integrate with ArchiveStatus for thread-safe progress reporting. Added comprehensive unit tests (11 test cases) that create real files and directories in temp folders, perform actual ZIP operations, and verify results. All tests pass successfully. Updated libwinfile.vcxproj to include new files and added necessary C++ headers to PCH.*

- [x] New submenu "Zip Archive" right after "Hardlin&k..." in the File menu and context menu.
    - 🤖 *Implemented ZIP Archive submenu with five commands: "Add to Zip", "Add To...", "Extract Here", "Extract to New Folder", and "Extract To...". Added menu structure to both File menu and context menu with proper resource IDs (IDM_ZIPARCHIVE_\*). Implemented menu enable/disable logic based on file selection - Add commands enabled when files are selected, Extract commands enabled only when ZIP files are selected. Added command handlers in AppCommandProc that currently show placeholder messages. Menu logic properly detects .zip file extensions using case-insensitive comparison.*

    - Add commands. Enabled if and only if the user has some selection in the file list. Use `createZipArchive`. If there is an error, delete the partial zip output file and show a messagebox.

        - [ ] Submenu command: "Add to Zip".  All selected files and folders are added to a new zip named after the folder. For instance, if the current folder is "Foo" then the filename is "Foo.zip". If "Foo.zip" already exists, then try "Foo (2).zip", then "Foo (3).zip", etc. until you find an unused filename. Use one `ArchiveProgressDialog` for the whole process. 

        - [ ] Submenu command: "Add To...". Show the save file dialog to choose the .zip file path. All selected files and folders are added to a zip written to that file. Use one `ArchiveProgressDialog` for the whole process.

    - Separator

    - Extract commands. Enabled if and only if the user has selected one or more `.zip` files, and no other file types are included in their selection. Whenever the extraction would overwrite a file, use `ConfirmDialog` to prompt. Check for other usages of this. Use `extractZipArchive`. If there is an error, stop and show a message box, and leave the partially extracted files in place.

        - [ ] Submenu command: "Extract Here". Each zip is extracted into its containing directory using one `ArchiveProgressDialog` for the whole process.

        - [ ] Submenu command: "Extract to New Folder". A folder named after the zip is created in the containing folder, and the zip is extracted there. For instance, "C:\Temp\Foo.zip" is extracted to a new folder "C:\Temp\Foo\". If the folder already exists, that's ok, treat overwrites as described above.

        - [ ] Submenu command: "Extract To...". Show the open file dialog in folder-selection mode to choose the destination folder. Then, each zip is extracted into that directory using one `ArchiveProgressDialog` for the whole process.
