# [NAME](#NAME)

`zip_dir_add` — add directory to zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_dir_add`(`zip_t *archive`, `const char *name`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_dir_add`](#zip_dir_add)() adds a directory to a zip
archive. The argument `archive` specifies the zip archive to which the
directory should be added. `name` is the directory's name in the zip
archive.

This function adds an entry to the archive. It does not check whether a
directory with that name exists in the file system, nor does it add its
contents if it does. The `flags` argument can be any of:

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
Guess encoding of `name` (default). (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_UTF_8`](#ZIP_FL_ENC_UTF_8)  
Interpret `name` as UTF-8.

[`ZIP_FL_ENC_CP437`](#ZIP_FL_ENC_CP437)  
Interpret `name` as code page 437 (CP-437).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the index of the new entry in the archive is
returned. Otherwise, -1 is returned and the error code in `archive` is
set to indicate the error.

# [ERRORS](#ERRORS)

`zip_dir_add`() fails if:

\[[`ZIP_ER_EXISTS`](#ZIP_ER_EXISTS)\]  
There is already an entry called `name` in the archive.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`archive` or `name` are `NULL`, or invalid UTF-8 encoded file names.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md)

# [HISTORY](#HISTORY)

`zip_dir_add`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 20, 2020 | NiH |
