# [NAME](#NAME)

`zip_set_archive_comment` — set zip archive comment

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_set_archive_comment`(`zip_t *archive`, `const char *comment`,
`zip_uint16_t len`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_set_archive_comment`](#zip_set_archive_comment)() function
sets the comment for the entire zip archive. If `comment` is `NULL` and
`len` is 0, the archive comment will be removed. `comment` must be
encoded in ASCII or UTF-8.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_set_archive_comment`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`len` is less than 0 or longer than the maximum comment length in a zip
file (65535), or `comment` is not a valid UTF-8 encoded string.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_get_comment(3)](zip_file_get_comment.md),
[zip_file_set_comment(3)](zip_file_set_comment.md),
[zip_get_archive_comment(3)](zip_get_archive_comment.md)

# [HISTORY](#HISTORY)

`zip_set_archive_comment`() was added in libzip 0.7. In libzip 0.11 the
type of `len` was changed from `int` to `zip_uint16_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
