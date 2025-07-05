# [NAME](#NAME)

`zip_source_zip_file`, `zip_source_zip_file_create` — create data source
from zip file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_zip_file`(`zip_t *archive`, `zip_t *srcarchive`,
`zip_uint64_t srcidx`, `zip_flags_t flags`, `zip_uint64_t start`,
`zip_int64_t length`, `const char *password`);

`zip_source_t *`  
`zip_source_zip_file_create`(`zip_t *srcarchive`, `zip_uint64_t srcidx`,
`zip_flags_t flags`, `zip_uint64_t start`, `zip_int64_t length`,
`const char *password`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_zip_file`](#zip_source_zip_file)() and
[`zip_source_zip_file_create`](#zip_source_zip_file_create)() create a
zip source from a file in a zip archive. The `srcarchive` argument is
the (open) zip archive containing the source zip file at index `srcidx`.
`length` bytes from offset `start` will be used in the zip_source. If
`length` is -1, the rest of the file, starting from `start`, is used.

If you intend to copy a file from one archive to another, using the flag
`ZIP_FL_COMPRESSED` is more efficient, as it avoids recompressing the
file data.

Supported flags are:

[`ZIP_FL_COMPRESSED`](#ZIP_FL_COMPRESSED)  
Get the compressed data. This is only supported if the complete file
data is requested (`start` == 0 and `length` == -1). This is not
supported for changed data. Default is uncompressed.

[`ZIP_FL_ENCRYPTED`](#ZIP_FL_ENCRYPTED)  
Get the encrypted data. (This flag implies `ZIP_FL_COMPRESSED`.) This is
only supported if the complete file data is requested (`start` == 0 and
`length` == -1). Default is decrypted.

[`ZIP_FL_UNCHANGED`](#ZIP_FL_UNCHANGED)  
Try to get the original data without any changes that may have been made
to `srcarchive` after opening it.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_zip_file`() and `zip_source_zip_file_create`() fail if:

\[[`ZIP_ER_CHANGED`](#ZIP_ER_CHANGED)\]  
Unchanged data was requested, but it is not available.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`srcarchive`, `srcidx`, `start`, or `length` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

Additionally, it can return all error codes from `zip_stat_index`() and
`zip_fopen_index`().

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_zip_file`() and `zip_source_zip_file_create`() were added in
libzip 1.10.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 10, 2023 | NiH |
