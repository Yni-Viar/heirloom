# [NAME](#NAME)

`zip_source_zip`, `zip_source_zip_create` — create data source from zip
file (obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_zip`(`zip_t *archive`, `zip_t *srcarchive`,
`zip_uint64_t srcidx`, `zip_flags_t flags`, `zip_uint64_t start`,
`zip_int64_t len`);

`zip_source_t *`  
`zip_source_zip_create`(`zip_t *srcarchive`, `zip_uint64_t srcidx`,
`zip_flags_t flags`, `zip_uint64_t start`, `zip_int64_t len`,
`zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_zip`](#zip_source_zip)() and
[`zip_source_zip_create`](#zip_source_zip_create)() are the obsolete
versions of [zip_source_zip_file(3)](zip_source_zip_file.md) or
[zip_source_zip_file_create(3)](zip_source_zip_file_create.md)
respectively. If you want to get the compressed data of the complete
file, use

`zip_source_zip_file(za, source_archive, source_index, ZIP_FL_COMPRESSED, 0, -1, NULL)`

The functions [`zip_source_zip`](#zip_source_zip~2)() and
[`zip_source_zip_create`](#zip_source_zip_create~2)() create a zip
source from a file in a zip archive. The `srcarchive` argument is the
(open) zip archive containing the source zip file at index `srcidx`.
`len` bytes from offset `start` will be used in the zip_source. If `len`
is 0 or -1, the rest of the file, starting from `start`, is used. If
`start` is zero and `len` is -1, the whole file will be copied without
decompressing it.

Supported flags are:

[`ZIP_FL_UNCHANGED`](#ZIP_FL_UNCHANGED)  
Try to get the original data without any changes that may have been made
to `srcarchive` after opening it.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_zip`() and `zip_source_zip_create`() fail if:

\[[`ZIP_ER_CHANGED`](#ZIP_ER_CHANGED)\]  
Unchanged data was requested, but it is not available.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`srcarchive`, `srcidx`, `start`, or `len` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

Additionally, it can return all error codes from `zip_stat_index`() and
`zip_fopen_index`().

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_zip`() was added in libzip 1.0. `zip_source_zip_create`()
was added in libzip 1.8.0. Both were deprecated in libzip 1.10.0. Use
`zip_source_zip_file`() or `zip_source_zip_file_create`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| January 23, 2023 | NiH |
