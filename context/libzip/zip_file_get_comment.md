# [NAME](#NAME)

`zip_file_get_comment` — get comment for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_file_get_comment`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint32_t *lenp`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_get_comment`](#zip_file_get_comment)() function returns
the comment for the file at position `index` in the zip archive. The
name is in UTF-8 encoding unless `ZIP_FL_ENC_RAW` was specified (see
below). This pointer should not be modified or
[free(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.md)'d,
and becomes invalid when `archive` is closed. If `lenp` is not `NULL`,
the integer to which it points will be set to the length of the comment.
If `flags` is set to `ZIP_FL_UNCHANGED`, the original unchanged comment
is returned.

Additionally, the following `flags` are supported:

[`ZIP_FL_ENC_RAW`](#ZIP_FL_ENC_RAW)  
Return the unmodified comment as it is in the ZIP archive.

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
(Default.) Guess the encoding of the comment in the ZIP archive and
convert it to UTF-8, if necessary. (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_STRICT`](#ZIP_FL_ENC_STRICT)  
Follow the ZIP specification for file names and extend it to file
comments, expecting them to be encoded in CP-437 in the ZIP archive
(except if it is a UTF-8 comment from the special extra field). Convert
it to UTF-8.

[*Note*](#Note): ASCII is a subset of both CP-437 and UTF-8.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, a pointer to the comment is returned, or
`NULL` if there is no comment. In case of an error, `NULL` is returned
and the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_get_comment`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_set_comment(3)](zip_file_set_comment.md),
[zip_get_archive_comment(3)](zip_get_archive_comment.md)

# [HISTORY](#HISTORY)

`zip_file_get_comment`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 22, 2020 | NiH |
