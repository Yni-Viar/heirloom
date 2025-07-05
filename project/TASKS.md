# Drag and drop bug fixes (winfile)

- [x] When dropping onto a file in the file list, treat it like dropping onto the containing folder instead of the file itself. Do not support running .exes by drag-and-drop or anything like that. Drag-and-drop is always a copy or move operation, and the target is always a folder.
    - *🤖 Modified DropData() function in wfdrop.cpp to check if the selected item is a directory using FILE_ATTRIBUTE_DIRECTORY before using it as a drop target. Files are now ignored and the drop goes to the containing folder.*

- [x] If the user drags a file from the file list and drops it right back in the same file list, we nonsensically prompt the user if they want to move the file with the same source and destination. In this situation, when the source and destination are the same, simply ignore the drag and do not prompt or take any action.
    - *🤖 Added logic to DropData() function to compare source and destination directories before calling DMMoveCopyHelper(). If they match, the drop is silently ignored with no prompt or action.*
