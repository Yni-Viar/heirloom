# [NAME](#NAME)

`zip_set_default_password` — set default password for encrypted files in
zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_set_default_password`(`zip_t *archive`, `const char *password`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_set_default_password`](#zip_set_default_password)() function
sets the default password used when accessing encrypted files. If
`password` is `NULL` or the empty string, the default password is unset.

If you prefer a different password for single files, use
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md) instead of
[zip_fopen(3)](zip_fopen.md). Usually, however, the same password is
used for every file in an zip archive.

The password is not verified when calling this function. See the
[CAVEATS](#CAVEATS) section in
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md) for more details
about password handling.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_set_default_password`() fails if:

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fopen(3)](zip_fopen.md),
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md)

# [HISTORY](#HISTORY)

`zip_set_default_password`() was added in libzip 0.10.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 15, 2020 | NiH |
