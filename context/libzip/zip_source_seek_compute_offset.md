# [NAME](#NAME)

`zip_source_seek_compute_offset` — validate arguments and compute offset

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_source_seek_compute_offset`(`zip_uint64_t offset`,
`zip_uint64_t length`, `void *data`, `zip_uint64_t data_length`,
`zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

Use this function to compute the offset for a `ZIP_SOURCE_SEEK` or
`ZIP_SOURCE_SEEK_WRITE` command. `data` and `data_length` are the
arguments to the source callback, `offset` is the current offset and
`length` is the length of the source data or, for
`ZIP_SOURCE_SEEK_WRITE`, the amount of data written.

# [RETURN VALUES](#RETURN_VALUES)

On success, it returns the new offset, on error it returns -1 and sets
`error`.

# [ERRORS](#ERRORS)

`zip_source_seek_compute_offset`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
One of the arguments is invalid or the seek would place the offset
outside the data.

# [SEE ALSO](#SEE_ALSO)

[zip_source_function(3)](zip_source_function.md)

# [HISTORY](#HISTORY)

`zip_source_seek_compute_offset`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
