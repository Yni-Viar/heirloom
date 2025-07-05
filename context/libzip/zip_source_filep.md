# [NAME](#NAME)

`zip_source_filep`, `zip_source_filep_create` — create data source from
FILE \*

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_filep`(`zip_t *archive`, `FILE *file`, `zip_uint64_t start`,
`zip_int64_t len`);

`zip_source_t *`  
`zip_source_filep_create`(`FILE *file`, `zip_uint64_t start`,
`zip_int64_t len`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_filep`](#zip_source_filep)() and
[`zip_source_filep_create`](#zip_source_filep_create)() create a zip
source from a file stream. They read `len` bytes from offset `start`
from the open file stream `file`. For a description of the `len`
argument, see [zip_source_file(3)](zip_source_file.md).

If the file stream supports seeking, the source can be used to open a
read-only zip archive from.

The file stream is closed when the source is being freed, usually by
[zip_close(3)](zip_close.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_filep`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`file`, `start`, or `len` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md),
[zip_source_file(3)](zip_source_file.md)

# [HISTORY](#HISTORY)

`zip_source_filep`() and `zip_source_filep_create`() were added in
libzip 1.0.

`ZIP_LENGTH_TO_END` and `ZIP_LENGTH_UNCHECKED` were added in libzip
1.10.1.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| June 30, 2023 | NiH |
