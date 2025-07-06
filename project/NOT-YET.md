
- [ ] In libwinfile, implement zip related functions, and write unit tests in `libwinfile_tests`. Update vcxproj accordingly when adding new files.
    - `void createZipArchive(const std::filesystem::path& zipFilePath, const std::vector<std::filesystem::path>& addFileOrFolderPaths, const std::filesystem::path& relativeToPath)` -- creates a zip file where the entries are the given files or folder paths (recursive), and the zip entry full names are relative to `relativeToPath`. For instance, if `addFileOrFolderPaths` contains `C:\Foo\Bar\Baz.txt` and `relativeToPath` is `C:\Foo`, then the zip entry is `Bar\Baz.txt`.
    - `void extractZipArchive(const std::filesystem::path& zipFilePath, const std::filesystem::path& targetFolder)` -- extracts the zip to the target folder.

- [x] New submenu "Zip Archive" right after "Hardlin&k..." in the File menu and context menu.
    - 🤖 *Implemented ZIP Archive submenu with five commands: "Add to Zip", "Add To...", "Extract Here", "Extract to New Folder", and "Extract To...". Added menu structure to both File menu and context menu with proper resource IDs (IDM_ZIPARCHIVE_\*). Implemented menu enable/disable logic based on file selection - Add commands enabled when files are selected, Extract commands enabled only when ZIP files are selected. Added command handlers in AppCommandProc that currently show placeholder messages. Menu logic properly detects .zip file extensions using case-insensitive comparison.*

    - Add commands. Enabled if and only if the user has some selection in the file list. If there is an error, delete the partial zip output file and show a messagebox. 

        - [ ] Submenu command: "Add to Zip".  All selected files and folders are added to a new zip named after the folder. For instance, if the current folder is "Foo" then the filename is "Foo.zip". If "Foo.zip" already exists, then try "Foo (2).zip", then "Foo (3).zip", etc. until you find an unused filename. Use one `ArchiveProgressDialog` for the whole process. 

        - [ ] Submenu command: "Add To...". Show the save file dialog to choose the .zip file path. All selected files and folders are added to a zip written to that file. Use one `ArchiveProgressDialog` for the whole process.

    - Separator

    - Extract commands. Enabled if and only if the user has selected one or more `.zip` files, and no other file types are included in their selection. Whenever the extraction would overwrite a file, use `ConfirmDialog` to prompt. Check for other usages of this. If there is an error, stop and show a message box, and leave the partially extracted files in place

        - [ ] Submenu command: "Extract Here". Each zip is extracted into its containing directory using one `ArchiveProgressDialog` for the whole process.

        - [ ] Submenu command: "Extract to New Folder". A folder named after the zip is created in the containing folder, and the zip is extracted there. For instance, "C:\Temp\Foo.zip" is extracted to a new folder "C:\Temp\Foo\". If the folder already exists, that's ok, treat overwrites as described above.

        - [ ] Submenu command: "Extract To...". Show the open file dialog in folder-selection mode to choose the destination folder. Then, each zip is extracted into that directory using one `ArchiveProgressDialog` for the whole process.
