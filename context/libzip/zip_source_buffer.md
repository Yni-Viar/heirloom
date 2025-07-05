# [NAME](#NAME)

`zip_source_buffer`, `zip_source_buffer_create` — create zip data source
from buffer

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_buffer`(`zip_t *archive`, `const void *data`,
`zip_uint64_t len`, `int freep`);

`zip_source_t *`  
`zip_source_buffer_create`(`const void *data`, `zip_uint64_t len`,
`int freep`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_buffer`](#zip_source_buffer)() and
[`zip_source_buffer_create`](#zip_source_buffer_create)() create a zip
source from the buffer `data` of size `len`. If `freep` is non-zero, the
buffer will be freed when it is no longer needed. `data` must remain
valid for the lifetime of the created source.

The source can be used to open a zip archive from.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_buffer`() and `zip_source_buffer_create`() fail if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`len` is greater than zero and `data` is `NULL`.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_open_from_source(3)](zip_open_from_source.md),
[zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_buffer`() and `zip_source_buffer_create`() were added in
libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
