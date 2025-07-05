# [NAME](#NAME)

`zip_get_archive_flag` — get status flags for zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_get_archive_flag`(`zip_t *archive`, `zip_flags_t flag`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_archive_flag`](#zip_get_archive_flag)() function returns
if the flag `flag` is set for the archive `archive`. The archive flags
might have been changed with
[`zip_set_archive_flag`](#zip_set_archive_flag)(); if `flags` is set to
`ZIP_FL_UNCHANGED`, the original unchanged flags are tested.

Supported flags are:

[`ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE`](#ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE)  
If this flag is cleared, the archive file will be removed if the archive
is empty. If it is set, an empty archive will be created, which is not
recommended by the zip specification. This flag is always cleared unless
explicitly set by the user with
[zip_set_archive_flag(3)](zip_set_archive_flag.md).

[`ZIP_AFL_IS_TORRENTZIP`](#ZIP_AFL_IS_TORRENTZIP)  
The archive is in torrentzip format.

[`ZIP_AFL_RDONLY`](#ZIP_AFL_RDONLY)  
The archive is read-only.

[`ZIP_AFL_WANT_TORRENTZIP`](#ZIP_AFL_WANT_TORRENTZIP)  
If the flag is set, the archive will be written in torrentzip format.
This flag is always cleared unless explicitly set by the user with
[zip_set_archive_flag(3)](zip_set_archive_flag.md).

# [RETURN VALUES](#RETURN_VALUES)

`zip_get_archive_flag`() returns 1 if `flag` is set for `archive`, 0 if
not, and -1 if an error occurred.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_set_archive_flag(3)](zip_set_archive_flag.md)

# [HISTORY](#HISTORY)

`zip_get_archive_flag`() was added in libzip 0.9. In libzip 0.11 the
type of `flag` was changed from `int` to `zip_flags_t m` and the type of
`flags` was changed from `int` to `zip_flags_t`.
`ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE`,
`ZIP_AFL_IS_TORRENTZIP`, and `ZIP_AFL_WANT_TORRENTZIP` were added in
libzip 1.10.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| January 23, 2023 | NiH |
