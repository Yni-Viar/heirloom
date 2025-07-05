# [NAME](#NAME)

`zip_source_win32w`, `zip_source_win32w_create` — create data source
from a Windows Unicode file name

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_win32w`(`zip_t *archive`, `const wchar_t *fname`,
`zip_uint64_t start`, `zip_int64_t len`);

`zip_source_t *`  
`zip_source_win32w_create`(`const wchar_t *fname`, `zip_uint64_t start`,
`zip_int64_t len`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_win32w`](#zip_source_win32w)() and
[`zip_source_win32w_create`](#zip_source_win32w_create)() create a zip
source on Windows using a Windows Unicode name. They open `fname` and
read `len` bytes from offset `start` from it. For a description of the
`len` argument, see [zip_source_file(3)](zip_source_file.md).

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

`zip_source_win32w`() and `zip_source_win32w_create`() fail if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`fname`, `start`, or `len` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_OPEN`](#ZIP_ER_OPEN)\]  
Opening `fname` failed.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md),
[zip_source_file(3)](zip_source_file.md),
[zip_source_win32a(3)](zip_source_win32a.md),
[zip_source_win32handle(3)](zip_source_win32handle.md)

# [HISTORY](#HISTORY)

`zip_source_win32w`() was added in libzip 1.0.

`ZIP_LENGTH_TO_END` and `ZIP_LENGTH_UNCHECKED` were added in libzip
1.10.1.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| June 30, 2023 | NiH |
