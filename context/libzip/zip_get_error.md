# [NAME](#NAME)

`zip_get_error` — get zip error for archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_error_t *`  
`zip_get_error`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_error`](#zip_get_error)() function returns the zip error
for the zip archive `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_system(3)](zip_error_code_system.md),
[zip_error_code_zip(3)](zip_error_code_zip.md)

# [HISTORY](#HISTORY)

`zip_get_error`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
