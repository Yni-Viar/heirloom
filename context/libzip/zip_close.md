# [NAME](#NAME)

`zip_close` — close zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_close`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_close`](#zip_close)() function writes any changes made to
`archive` to disk. If `archive` contains no files, the file is
completely removed (no empty archive is written), unless the archive
flag `ZIP_AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE` is set. If
successful, `archive` is freed. Otherwise `archive` is left unchanged
and must still be freed.

To close and free a zip archive without saving changes, use
[zip_discard(3)](zip_discard.md).

Progress updates for GUIs can be implemented using
[zip_register_progress_callback_with_state(3)](zip_register_progress_callback_with_state.md).
Cancelling the write of an archive during `zip_close` can be implemented
using
[zip_register_cancel_callback_with_state(3)](zip_register_cancel_callback_with_state.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_close`() will fail if:

\[[`ZIP_ER_EOF`](#ZIP_ER_EOF)\]  
Unexpected end-of-file found while reading from a file.

\[[`ZIP_ER_INTERNAL`](#ZIP_ER_INTERNAL)\]  
The callback function of an added or replaced file returned an error but
failed to report which.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
The `path` argument is `NULL`.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_NOZIP`](#ZIP_ER_NOZIP)\]  
File is not a zip archive.

\[[`ZIP_ER_READ`](#ZIP_ER_READ)\]  
A file read failed.

\[[`ZIP_ER_RENAME`](#ZIP_ER_RENAME)\]  
A temporary file could not be renamed to its final name.

\[[`ZIP_ER_SEEK`](#ZIP_ER_SEEK)\]  
A file seek failed.

\[[`ZIP_ER_TMPOPEN`](#ZIP_ER_TMPOPEN)\]  
A temporary file could not be created.

\[[`ZIP_ER_WRITE`](#ZIP_ER_WRITE)\]  
A file write failed.

\[[`ZIP_ER_ZLIB`](#ZIP_ER_ZLIB)\]  
An error occurred while (de)compressing a stream with
[zlib(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/zlib.md).

Additionally, any errors returned by the callback function for added or
replaced files will be passed back.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_discard(3)](zip_discard.md),
[zip_fdopen(3)](zip_fdopen.md),
[zip_get_error(3)](zip_get_error.md), [zip_open(3)](zip_open.md),
[zip_register_cancel_callback_with_state(3)](zip_register_cancel_callback_with_state.md),
[zip_register_progress_callback_with_state(3)](zip_register_progress_callback_with_state.md),
[zip_set_archive_flag(3)](zip_set_archive_flag.md),
[zip_strerror(3)](zip_strerror.md)

# [HISTORY](#HISTORY)

`zip_close`() was added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

# [CAVEATS](#CAVEATS)

Please note that all indices, [zip_stat(3)](zip_stat.md) information
and other data about the archive is invalid after `zip_close`. When you
open the same file again, it will be a completely new `zip_t` structure.

|                  |     |
|------------------|-----|
| January 23, 2023 | NiH |
