# [NAME](#NAME)

`zip_fopen`, `zip_fopen_index` — open file in zip archive for reading

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_file_t *`  
`zip_fopen`(`zip_t *archive`, `const char *fname`, `zip_flags_t flags`);

`zip_file_t *`  
`zip_fopen_index`(`zip_t *archive`, `zip_uint64_t index`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_fopen`](#zip_fopen)() function opens the file name `fname` in
`archive`. The `flags` argument specifies how the name lookup should be
done, according to the values are described in
[zip_name_locate(3)](zip_name_locate.md). Also, the following values
may be [*or*](#or)'ed to it.

[`ZIP_FL_COMPRESSED`](#ZIP_FL_COMPRESSED)  
Read the compressed data. Otherwise the data is uncompressed by
[`zip_fread`](#zip_fread)().

[`ZIP_FL_UNCHANGED`](#ZIP_FL_UNCHANGED)  
Read the original data from the zip archive, ignoring any changes made
to the file; this is not supported by all data sources.

The [`zip_fopen_index`](#zip_fopen_index)() function opens the file at
position `index`.

If encrypted data is encountered, the functions call
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md) or
[zip_fopen_index_encrypted(3)](zip_fopen_index_encrypted.md)
respectively, using the default password set with
[zip_set_default_password(3)](zip_set_default_password.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, a `struct zip_file` pointer is returned.
Otherwise, `NULL` is returned and the error code in `archive` is set to
indicate the error.

# [ERRORS](#ERRORS)

\[[`ZIP_ER_CHANGED`](#ZIP_ER_CHANGED)\]  
The file data has been changed and the data source does not support
rereading data.

\[[`ZIP_ER_COMPNOTSUPP`](#ZIP_ER_COMPNOTSUPP)\]  
The compression method used is not supported.

\[[`ZIP_ER_ENCRNOTSUPP`](#ZIP_ER_ENCRNOTSUPP)\]  
The encryption method used is not supported.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_NOPASSWD`](#ZIP_ER_NOPASSWD)\]  
The file is encrypted, but no password has been provided.

\[[`ZIP_ER_READ`](#ZIP_ER_READ)\]  
A file read error occurred.

\[[`ZIP_ER_SEEK`](#ZIP_ER_SEEK)\]  
A file seek error occurred.

\[[`ZIP_ER_WRONGPASSWD`](#ZIP_ER_WRONGPASSWD)\]  
The provided password does not match the password used for encryption.
Note that some incorrect passwords are not detected by the check done by
`zip_fopen`().

\[[`ZIP_ER_ZLIB`](#ZIP_ER_ZLIB)\]  
Initializing the zlib stream failed.

The function `zip_fopen`() may also fail and set `zip_err` for any of
the errors specified for the routine
[zip_name_locate(3)](zip_name_locate.md).

The function `zip_fopen_index`() may also fail with `ZIP_ER_INVAL` if
`index` is invalid.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fclose(3)](zip_fclose.md),
[zip_fread(3)](zip_fread.md), [zip_fseek(3)](zip_fseek.md),
[zip_get_num_entries(3)](zip_get_num_entries.md),
[zip_name_locate(3)](zip_name_locate.md),
[zip_set_default_password(3)](zip_set_default_password.md)

# [HISTORY](#HISTORY)

`zip_fopen`() and `zip_fopen_index`() were added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
