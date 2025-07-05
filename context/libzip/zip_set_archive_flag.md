# [NAME](#NAME)

`zip_set_archive_flag` — set zip archive flag

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_set_archive_flag`(`zip_t *archive`, `zip_flags_t flag`,
`int value`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_set_archive_flag`](#zip_set_archive_flag)() function sets the
flag `flag` for the archive `archive` to the value `value`.

Supported flags are:

[`ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE`](#ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE)  
If this flag is cleared, the archive file will be removed if the archive
is empty. If it is set, an empty archive will be created, which is not
recommended by the zip specification.

[`ZIP_AFL_RDONLY`](#ZIP_AFL_RDONLY)  
If this flag is set, no modification to the archive are allowed. This
flag can only be cleared if it was manually set with
`zip_set_archive_flag`, not if the archive was opened read-only.

[`ZIP_AFL_WANT_TORRENTZIP`](#ZIP_AFL_WANT_TORRENTZIP)  
If this flag is set, the archive will be written in torrentzip format.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned, and -1 if an error occurred.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_get_archive_flag(3)](zip_get_archive_flag.md)

# [HISTORY](#HISTORY)

`zip_set_archive_flag`() was added in libzip 0.9. In libzip 0.11 the
type of `flag` was changed from `int` to `zip_flags_t`.
`ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE` and
`ZIP_AFL_WANT_TORRENTZIP` were added in libzip 1.10.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| July 19, 2023 | NiH |
