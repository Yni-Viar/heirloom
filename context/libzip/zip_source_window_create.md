# [NAME](#NAME)

`zip_source_window_create` — create zip data source overlay

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_window_create`(`zip_source_t *source`, `zip_uint64_t start`,
`zip_int64_t len`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_source_window_create`](#zip_source_window_create)() function
create a zip source from an underlying zip source, restricting access to
a particular window starting at byte `start` and having size `len`. If
`len` is -1, the window spans to the end of the underlying source.

[`zip_source_window`](#zip_source_window)() and
`zip_source_window_create`() don't take ownership of `source`. The
caller is responsible for freeing it. (This is different to other
layered sources.)

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `error` is set to indicate the
error.

# [ERRORS](#ERRORS)

`zip_source_window_create`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`src` is `NULL`; there is an integer overflow adding `start` and `len`;
or `len` is less than -1.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_window_create`() was added in libzip 1.8.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| April 29, 2021 | NiH |
