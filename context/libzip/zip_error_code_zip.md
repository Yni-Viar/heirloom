# [NAME](#NAME)

`zip_error_code_zip` — get libzip error part of zip_error

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_error_code_zip`(`const zip_error_t *ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_code_zip`](#zip_error_code_zip)() function returns the
libzip specific part of the error from the zip_error error `ze`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_system(3)](zip_error_code_system.md)

# [HISTORY](#HISTORY)

`zip_error_code_zip`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
