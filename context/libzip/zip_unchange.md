# [NAME](#NAME)

`zip_unchange` — undo changes to file in zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_unchange`(`zip_t *archive`, `zip_uint64_t index`);

# [DESCRIPTION](#DESCRIPTION)

Changes to the file at position `index` are reverted.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_unchange`() fails if:

\[[`ZIP_ER_EXISTS`](#ZIP_ER_EXISTS)\]  
Unchanging the name would result in a duplicate name in the archive.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `zip`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_unchange_all(3)](zip_unchange_all.md),
[zip_unchange_archive(3)](zip_unchange_archive.md)

# [HISTORY](#HISTORY)

`zip_unchange`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
