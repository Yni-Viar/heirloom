# [NAME](#NAME)

`zip_get_archive_comment` — get zip archive comment

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_get_archive_comment`(`zip_t *archive`, `int *lenp`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_archive_comment`](#zip_get_archive_comment)() function
returns the comment for the entire zip archive. The return value is in
UTF-8 encoding unless `ZIP_FL_ENC_RAW` was specified (see below). This
pointer should not be modified or
[free(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.md)'d,
and becomes invalid when `archive` is closed. If `lenp` is not `NULL`,
the integer to which it points will be set to the length of the comment.
If `flags` is set to `ZIP_FL_UNCHANGED`, the original unchanged comment
is returned.

Additionally, the following `flags` are supported:

[`ZIP_FL_ENC_RAW`](#ZIP_FL_ENC_RAW)  
Return the unmodified archive comment as it is in the ZIP archive.

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
(Default.) Guess the encoding of the archive comment in the ZIP archive
and convert it to UTF-8, if necessary. (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_STRICT`](#ZIP_FL_ENC_STRICT)  
Follow the ZIP specification for file names and extend it to the archive
comment, thus also expecting it in CP-437 encoding. Convert it to UTF-8.

[*Note*](#Note): ASCII is a subset of both CP-437 and UTF-8.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, a pointer to the comment is returned, or
`NULL` if there is no comment.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_get_comment(3)](zip_file_get_comment.md)

# [HISTORY](#HISTORY)

`zip_get_archive_comment`() was added in libzip 0.7. In libzip 0.11 the
type of `flags` was changed from `int` to `zip_flags_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 22, 2020 | NiH |
