# [NAME](#NAME)

`zip_file_set_encryption` — set encryption method for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_set_encryption`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t method`, `const char *password`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_set_encryption`](#zip_file_set_encryption)() function
sets the encryption method for the file at position `index` in the zip
archive to `method` using the password `password`. The `method` is the
same as returned by [zip_stat(3)](zip_stat.md). For the `method`
argument, currently only the following values are supported:

[`ZIP_EM_NONE`](#ZIP_EM_NONE)  
No encryption.

[`ZIP_EM_AES_128`](#ZIP_EM_AES_128)  
Winzip AES-128 encryption.

[`ZIP_EM_AES_192`](#ZIP_EM_AES_192)  
Winzip AES-192 encryption.

[`ZIP_EM_AES_256`](#ZIP_EM_AES_256)  
Winzip AES-256 encryption.

[`ZIP_EM_TRAD_PKWARE`](#ZIP_EM_TRAD_PKWARE)  
Traditional PKWare encryption. Do not use this method, it is not secure.
It is only provided for backwards compatibility.

If `password` is `NULL`, the default password provided by
[zip_set_default_password(3)](zip_set_default_password.md) is used.

The current encryption method for a file in a zip archive can be
determined using [zip_stat(3)](zip_stat.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_set_encryption`() fails if:

\[[`ZIP_ER_ENCRNOTSUPP`](#ZIP_ER_ENCRNOTSUPP)\]  
Unsupported compression method requested.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`, or the argument
combination is invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
Read-only zip file, no changes allowed.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_encryption_method_supported(3)](zip_encryption_method_supported.md),
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md),
[zip_fopen_index_encrypted(3)](zip_fopen_index_encrypted.md),
[zip_set_default_password(3)](zip_set_default_password.md),
[zip_stat(3)](zip_stat.md)

# [HISTORY](#HISTORY)

`zip_file_set_encryption`() was added in libzip 1.2.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| April 2, 2020 | NiH |
