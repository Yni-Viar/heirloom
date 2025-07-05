# [NAME](#NAME)

`zip_get_name` — get name of file by index

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_get_name`(`zip_t *archive`, `zip_uint64_t index`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_name`](#zip_get_name)() function returns the name of the
file at position `index` in `archive`. The name is in UTF-8 encoding
unless `ZIP_FL_ENC_RAW` was specified (see below).

If `flags` is set to `ZIP_FL_UNCHANGED`, the original unchanged filename
is returned. The returned string must not be modified or freed, and
becomes invalid when `archive` is closed.

Additionally, the following `flags` are supported:

[`ZIP_FL_ENC_RAW`](#ZIP_FL_ENC_RAW)  
Return the unmodified names as it is in the ZIP archive.

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
(Default.) Guess the encoding of the name in the ZIP archive and convert
it to UTF-8, if necessary. (Only CP-437 and UTF-8 are recognized.)

[`ZIP_FL_ENC_STRICT`](#ZIP_FL_ENC_STRICT)  
Follow the ZIP specification and expect CP-437 encoded names in the ZIP
archive (except if they are explicitly marked as UTF-8). Convert it to
UTF-8.

[*Note*](#Note): ASCII is a subset of both CP-437 and UTF-8.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, a pointer to the name is returned.
Otherwise, `NULL` and the error code in `archive` is set to indicate the
error.

# [ERRORS](#ERRORS)

`zip_get_name`() fails if:

\[[`ZIP_ER_DELETED`](#ZIP_ER_DELETED)\]  
`index` refers to a file that has been deleted (see
[zip_delete(3)](zip_delete.md)).

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`, or `index` points to an
added file and `ZIP_FL_UNCHANGED` is set.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_name_locate(3)](zip_name_locate.md)

# [HISTORY](#HISTORY)

`zip_get_name`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`. In libzip 0.11 the
type of `flags` was changed from `int` to `zip_flags_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 22, 2020 | NiH |
