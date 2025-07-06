# Bug fixes (winfile)

- [x] When deleting a folder, we are recursing into the folder and sending the files individually to the Recycle Bin. This is slow and not what the user expects. When we delete a selection, don't recurse into folders, instead send the folder itself to the Recycle Bin, like Explorer does.
  - *🤖 Modified the `OPER_RMDIR | FUNC_DELETE` case in `WFMoveCopyDriverThread` to call `MoveFileToRecycleBin` directly on the directory instead of using `RMDir`. This sends the entire directory to the recycle bin at once, which is much faster and matches Explorer behavior. The fix preserves the existing behavior for move operations.*
