# Zip integration using libzip (winfile)

Search the libzip docs in `context/libzip/` for guidance on library usage.

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

- [ ] New submenu "Zip Archive" right after "Hardlin&k..." in the File menu and context menu. This submenu is enabled if and only if the "Properties" command is enabled. Piggyback on the logic for Properties.

    - Add commands. If there is an error, delete the partial zip output file and show a messagebox.

        - [ ] Submenu command: "Add to Zip". Enabled if and only if the user has some selection in the file list. All selected files and folders are added to a new zip named after the folder. For instance, if the current folder is "Foo" then the filename is "Foo.zip". If "Foo.zip" already exists, then try "Foo (2).zip", then "Foo (3).zip", etc. until you find an unused filename. Use one `ArchiveProgressDialog` for the whole process. 

        - [ ] Submenu command: "Add To...". Enabled if and only if the user has some selection in the file list. Show the save file dialog to choose the .zip file path. All selected files and folders are added to a zip written to that file. Use one `ArchiveProgressDialog` for the whole process.

    - Extract commands. Whenever the extraction would overwrite a file, use `ConfirmDialog` to prompt. Check for other usages of this. If there is an error, stop and show a message box, and leave the partially extracted files in place

        - [ ] Submenu command: "Extract Here". Enabled if and only if the user has selected one or more `.zip` files, and no other file types are included in their selection. Each zip is extracted into its containing directory using one `ArchiveProgressDialog` for the whole process.

        - [ ] Submenu command: "Extract To...". Enabled if and only if the user has selected one or more `.zip` files, and no other file types are included in their selection. Show the open file dialog in folder-selection mode to choose the destination folder. Then, each zip is extracted into that directory using one `ArchiveProgressDialog` for the whole process.
