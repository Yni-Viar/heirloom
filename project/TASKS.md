# Bug fixes (winfile)

- [ ] When deleting a folder, we are recursing into the folder and sending the files individually to the Recycle Bin. This is slow and not what the user expects. When we delete a selection, don't recurse into folders, instead send the folder itself to the Recycle Bin, like Explorer does.
