# [NAME](#NAME)

`zip_source_win32handle`, `zip_source_win32handle_create` — create data
source from a Windows file handle

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_win32handle`(`zip_t *archive`, `HANDLE h`,
`zip_uint64_t start`, `zip_int64_t len`);

`zip_source_t *`  
`zip_source_win32handle_create`(`HANDLE h`, `zip_uint64_t start`,
`zip_int64_t len`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_win32handle`](#zip_source_win32handle)() and
[`zip_source_win32handle_create`](#zip_source_win32handle_create)()
create a zip source from a Windows file handle. They read `len` bytes
from offset `start` from it. If `len` is 0 or -1, the whole file
(starting from `start`) is used.

If the file supports seek, the source can be used to open a zip archive
from.

The file is opened and read when the data from the source is used,
usually by [`zip_close`](#zip_close)() or
[`zip_open_from_source`](#zip_open_from_source)().

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_w32handle`() and `zip_source_w32handle_create`() fail if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`h`, `start`, or `len` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md),
[zip_source_win32a(3)](zip_source_win32a.md),
[zip_source_win32w(3)](zip_source_win32w.md)

# [HISTORY](#HISTORY)

`zip_source_win32handle`() and `zip_source_win32handle_create`() were
added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|              |     |
|--------------|-----|
| May 14, 2024 | NiH |
