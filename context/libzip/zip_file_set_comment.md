# [NAME](#NAME)

`zip_file_set_comment` — set comment for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_set_comment`(`zip_t *archive`, `zip_uint64_t index`,
`const char *comment`, `zip_uint16_t len`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_set_comment`](#zip_file_set_comment)() function sets the
comment for the file at position `index` in the zip archive to `comment`
of length `len`. If `comment` is `NULL` and `len` is 0, the file comment
will be removed. The `flags` argument can be any of:

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
Guess encoding of `comment` (default). (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_UTF_8`](#ZIP_FL_ENC_UTF_8)  
Interpret `comment` as UTF-8.

[`ZIP_FL_ENC_CP437`](#ZIP_FL_ENC_CP437)  
Interpret `comment` as code page 437 (CP-437).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_set_comment`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`, or `len` is less than 0
or longer than the maximum comment length in a zip file (65535), or
`comment` is not a valid UTF-8 encoded string.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
The `archive` was opened in read-only mode.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_get_comment(3)](zip_file_get_comment.md),
[zip_get_archive_comment(3)](zip_get_archive_comment.md),
[zip_set_archive_comment(3)](zip_set_archive_comment.md)

# [HISTORY](#HISTORY)

`zip_file_set_comment`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 22, 2020 | NiH |
