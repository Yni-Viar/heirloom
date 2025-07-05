# [NAME](#NAME)

`zipmerge` — merge zip archives

# [SYNOPSIS](#SYNOPSIS)

|            |                                                               |
|------------|---------------------------------------------------------------|
| `zipmerge` | \[`-DhIiSsV`\] `target-zip` `source-zip` \[`source-zip ...`\] |

# [DESCRIPTION](#DESCRIPTION)

`zipmerge` copies files from the source zip archives `source-zip` to the
target zip archive `target-zip`. By default, files in the source zip
archives overwrite existing files of the same name in the target zip
archive. By default, compressed files in the source archive are copied
directly without recompression, uncompressed files are compressed using
the default compression algorithm.

Supported options:

[`-D`](#D)  
Ignore directory components in file name comparisons. This option is
slow for archives with many files.

[`-h`](#h)  
Display a short help message and exit.

[`-I`](#I)  
Ignore case in file name comparisons This option is slow for archives
with many files.

[`-i`](#i)  
Ask before overwriting files. See also `-s`.

[`-k`](#k)  
Do not compress files that were uncompressed in `source-zip`, otherwise
they are compressed with the default compression method.

[`-S`](#S)  
Do not overwrite files that have the same name, size, and CRC32 in both
the source and target archives.

[`-s`](#s)  
When `-i` is given, do not ask before overwriting files that have the
same name, size, and CRC32.

[`-V`](#V)  
Display version information and exit.

# [EXIT STATUS](#EXIT_STATUS)

`zipmerge` exits 0 on success and \>1 if an error occurred.

# [SEE ALSO](#SEE_ALSO)

[zipcmp(1)](zipcmp.md), [ziptool(1)](ziptool.md),
[libzip(3)](libzip.md)

# [HISTORY](#HISTORY)

`zipmerge` was added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

# [CAVEATS](#CAVEATS)

`zipmerge` uses one open file descriptor per zip archive. If you need to
merge a lot of zip archives, check your shell's file descriptor ulimit
and either increase it or run `zipmerge` multiple times with e.g. 1000
source zip archives each time.

|                  |     |
|------------------|-----|
| November 7, 2023 | NiH |
