# Making a release
- Set version in `res.rc`, look for `VER_FILEVERSION`. Commit as "Version X.X.X".
- Install Windows SDK in order to get `signtool`. The only necessary features are "Windows SDK Signing Tools for Desktop Apps" and "MSI Tools". https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
- Search for signtool in `C:\Program Files (x86)`. Set `$signtool` to its path.
- Find the HSM entry in 1Password. Set `$sha1` to the SHA1 hash. Keep the entry up so you can copy the password out.
- Download the artifact zip from GitHub Actions.
- Set $sha1 to the hash of the code signing certificate, then: `& $signtool sign /v /tr http://timestamp.sectigo.com /fd SHA256 /td SHA256 /sha1 $sha1 winfile.exe` (for each build). Paste the password when prompted.
- Verify digital signatures in the file properties.
- Rename Arm64 and x64 folders to `winfile-arch-X.X.X` and zip them (from the inside, just the files).
- Create release on GitHub and drag the two zips in.