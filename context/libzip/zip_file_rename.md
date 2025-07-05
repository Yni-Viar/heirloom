# [NAME](#NAME)

`zip_file_rename` — rename file in zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_rename`(`zip_t *archive`, `zip_uint64_t index`,
`const char *name`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The file at position `index` in the zip archive `archive` is renamed to
`name`. The `flags` argument can be any of:

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
Guess encoding of `name` (default). (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_UTF_8`](#ZIP_FL_ENC_UTF_8)  
Interpret `name` as UTF-8.

[`ZIP_FL_ENC_CP437`](#ZIP_FL_ENC_CP437)  
Interpret `name` as code page 437 (CP-437).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_rename`() fails if:

\[[`ZIP_ER_DELETED`](#ZIP_ER_DELETED)\]  
The file to be renamed has been deleted from the archive.

\[[`ZIP_ER_EXISTS`](#ZIP_ER_EXISTS)\]  
There is already a file called `name` in the archive.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`, `name is` `NULL`, the
empty string, or not a valid UTF-8 encoded string. Also a file cannot be
renamed to a directory or vice versa. Directories are denoted by a
trailing slash.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_unchange(3)](zip_unchange.md)

# [HISTORY](#HISTORY)

`zip_file_rename`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 22, 2020 | NiH |
