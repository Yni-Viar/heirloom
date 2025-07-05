# Zip integration using libzip (winfile)

Search the libzip docs in `context/libzip/` for guidance on library usage.

- [ ] Prepare a dialog resource `IDM_ARCHIVE_PROGRESS` and a C++ class `ArchiveProgressDialog` for a zip/unzip operation progress dialog, following the example of `IDD_BOOKMARK_EDIT`.
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
    - C++ class
        - Ctor takes a std::function callback. Arg is
        - When the window is shown, a thread is started to run the callback. When the callback ends, we close the window. If the callback succeeded, dialog result `IDOK`. If the callback threw an exception, we catch it and expose it from a getter in `ArchiveProgressDialog` then set dialog result to `IDABORT`.

- [ ] New submenu "Zip Archive" right after "Hardlin&k..." in the File menu and context menu. This submenu is enabled if and only if the "Properties" command is enabled. Piggyback on the logic for Properties.

    - [ ] Submenu command: "Extract Here". Enabled if and only if the user has selected one or more `.zip` files, and no other file types are included in their selection. 