# Improve interactions with minimized folder icons (progman)
- [ ] Rename and delete minimized folders
    - When the bar is focused, F2 should start editing the name of the selected minimized folder.
        - *🤖 Added F2 key handling in ListViewSubclassProc to call ListView_EditLabel, enabled LVS_EDITLABELS style on ListView, and handled LVN_ENDLABELEDIT notification to call rename callback*
        - [ ] That failed. Nothing happens when I press F2 when the minimized folder icon is focused and selected.
    - When the bar is focused, Ctrl+D (existing delete hotkey) should delete the selected minimized folder instead of the selected shortcut in the most recently focused (but not currently focused) shown folder window.
        - *🤖 Added Ctrl+D key handling in ListViewSubclassProc to detect key combination and call delete callback with confirmation dialog*
        - That failed. Ctrl+D attempts to delete the last focused restored folder window instead of the currently selected minimized folder icon.
- [x] When right-clicking on a minimized folder, a context menu should appear:
    - [x] "Open" - Same as double-clicking the icon
    - [x] Separator
    - [x] "Rename" - Same as pressing F2 to edit the name
    - [x] "Delete" - Same as pressing Ctrl+D to delete the folder
    - *🤖 Added IDR_FOLDER_MENU context menu resource, implemented WM_RBUTTONDOWN handler in ListViewSubclassProc to show menu and handle commands. Added callback methods to MinimizedFolderListControl and ProgramManagerWindow to handle rename/delete operations via filesystem.*

# Add drag and drop support (progman)
- [ ] Accept drags of arbitrary files and folders to shortcut folder windows. If a .lnk shortcut is dragged in, then simply copy that shortcut into the shortcut folder. If anything else is dragged in, then create a new shortcut to that target file or folder.
