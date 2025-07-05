# [NAME](#NAME)

`zip_delete` — delete file from zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_delete`(`zip_t *archive`, `zip_uint64_t index`);

# [DESCRIPTION](#DESCRIPTION)

The file at position `index` in the zip archive `archive` is marked as
deleted.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_delete`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_unchange(3)](zip_unchange.md)

# [HISTORY](#HISTORY)

`zip_delete`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
