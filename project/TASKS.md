# Drag and drop bug fixes (winfile)

- [x] When dropping onto a file in the file list, treat it like dropping onto the containing folder instead of the file itself. Do not support running .exes by drag-and-drop or anything like that. Drag-and-drop is always a copy or move operation, and the target is always a folder.
    - *🤖 Modified DropData() function in wfdrop.cpp to check if the selected item is a directory using FILE_ATTRIBUTE_DIRECTORY before using it as a drop target. Files are now ignored and the drop goes to the containing folder.*

- [x] If the user drags a file from the file list and drops it right back in the same file list, we nonsensically prompt the user if they want to move the file with the same source and destination. In this situation, when the source and destination are the same, simply ignore the drag and do not prompt or take any action.
    - *🤖 Added logic to DropData() function to compare source and destination directories before calling DMMoveCopyHelper(). If they match, the drop is silently ignored with no prompt or action.*
    - [x] It didn't work. Consider how this fix interacts with the fix above, and consider the situation where I drag a file and then drop it back onto itself, causing the first fix to kick in and adjust the path to the containing folder. Logic out that situation and implement a fix.
        - *🤖 Fixed the interaction between the two fixes. The issue was that szSrc was being set to the adjusted destination path instead of the actual source directory. Now szSrc correctly stores the current directory where the files originate, and the comparison properly detects when source and destination are the same directory, regardless of whether dropping on a file or folder.*
