# [NAME](#NAME)

`ZIP_SOURCE_GET_ARGS` — validate and cast arguments to source callback

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`type *`  
`ZIP_SOURCE_GET_ARGS`(`type`, `void *data`, `zip_uint64_t len`,
`zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The [`ZIP_SOURCE_GET_ARGS`](#ZIP_SOURCE_GET_ARGS)() macro casts `data`
to a pointer to `type`.

# [RETURN VALUES](#RETURN_VALUES)

On success, `ZIP_SOURCE_GET_ARGS`() returns `data`. In case of error, it
returns `NULL` and sets `error`.

# [ERRORS](#ERRORS)

`ZIP_SOURCE_GET_ARGS`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`len` is less than the size of `type`

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_source_function(3)](zip_source_function.md)

# [HISTORY](#HISTORY)

`ZIP_SOURCE_GET_ARGS`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
