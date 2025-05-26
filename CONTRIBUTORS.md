# Getting started
- Run `src/install-deps.sh` to download the required third party dependency files.
- Build with `src/build-*.sh` or one of the Visual Studio solutions.

# Editing dialogs
- Set the font to Segoe UI / Regular / 9pt.
- (Optional) Set the margins to 7 DLU, it comes up in tiny text when you resize the guidelines.
- Set grid spacing to 7 DLU and resize the dialog in grid mode to be an exact multiple of the grid spacing. You will have to go back and forth between "None" and "Grid" guide modes. Size controls in "None" mode, then lay out the dialog in "Grid" mode. Don't resize controls in grid mode.
- Control sizes
    - Button: 50x14 DLU
    - Textbox: 12 DLU tall
    - Checkbox: 10 DLU tall (Size to Content does it correctly)
    - Label: 8 DLU tall (Size to Content does it correctly)
    - Dialog margin: 7
- Put labels above their associated textbox (not to the left). Using the grid editor, the label will be either too close or too far from the control. Put it in the "too close" position and hit the up arrow 3 times to move it up by 3 DLU.
- To stack multiple lines of labels, use the grid editor to place them on top of each other. This will be too tight. Use the arrow key to space them out by an additional 2 arrow key steps each.
- In the .rc file, set the left/top coordinates of the dialog (the first two numbers after DIALOGEX) to 20, 20.

## File Manager
We have `res_dummy.rc` which can be opened in Visual Studio, while `res.rc` doesn't open properly.
Copy your dialog from `res.rc` into `res_dummy.rc`, edit it there, and copy it back.
After saving in Visual Studio, it will trash `resource.h`. Restore it using git.

# Making a release
- Set version in `res.rc` and `progman.rc`, look for `VERSION`. Commit as "Version X.X.X".
- Install Windows SDK in order to get `signtool`. The only necessary features are "Windows SDK Signing Tools for Desktop Apps" and "MSI Tools". https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
- Search for signtool in `C:\Program Files (x86)`. Set `$signtool` to its path.
- Find the HSM entry in 1Password. Set `$sha1` to the SHA1 hash. Keep the entry up so you can copy the password out.
- Download the artifact zip from GitHub Actions.
- Set $sha1 to the hash of the code signing certificate, then: `& $signtool sign /v /tr http://timestamp.sectigo.com /fd SHA256 /td SHA256 /sha1 $sha1 winfile.exe` (for each build). Paste the password when prompted.
- Verify digital signatures in the file properties.
- Rename Arm64 and x64 folders to `heirloom-arch-X.X.X` and zip them (from the inside, just the files).
- Create release on GitHub and drag the two zips in.
