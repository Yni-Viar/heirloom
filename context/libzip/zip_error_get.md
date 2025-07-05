# [NAME](#NAME)

`zip_error_get`, `zip_file_error_get` — get error codes for archive or
file (obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_get`(`zip_t *archive`, `int *zep`, `int *sep`);

`void`  
`zip_file_error_get`(`zip_file_t *file`, `int *zep`, `int *sep`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_error_get`](#zip_error_get)() and
[`zip_file_error_get`](#zip_file_error_get)() are deprecated. Use
[zip_error_code_system(3)](zip_error_code_system.md),
[zip_error_code_zip(3)](zip_error_code_zip.md),
[zip_file_get_error(3)](zip_file_get_error.md), and
[zip_get_error(3)](zip_get_error.md) instead.

For [`zip_error_get`](#zip_error_get~2)(), replace

    int ze, se;
    zip_error_get(za, &ze, &se);

with

    int ze, se;
    zip_error_t *error = zip_get_error(za);
    ze = zip_error_code_zip(error);
    se = zip_error_code_system(error);

For [`zip_file_error_get`](#zip_file_error_get~2)(), replace

    int ze, se;
    zip_file_error_get(zf, &ze, &se);

with

    int ze, se;
    zip_error_t *error = zip_file_get_error(zf);
    ze = zip_error_code_zip(error);
    se = zip_error_code_system(error);

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_system(3)](zip_error_code_system.md),
[zip_error_code_zip(3)](zip_error_code_zip.md),
[zip_file_get_error(3)](zip_file_get_error.md),
[zip_get_error(3)](zip_get_error.md)

# [HISTORY](#HISTORY)

`zip_error_get`() was added in libzip 0.6. It was deprecated in libzip
1.0, use `zip_get_error`(), `zip_error_code_zip`(), /
`zip_error_code_system`() instead.

`zip_file_error_get`() was added in libzip 0.6. It was deprecated in
libzip 1.0, use `zip_file_get_error`(), `zip_error_code_zip`(), /
`zip_error_code_system`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
