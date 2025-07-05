# [NAME](#NAME)

`zip_file_get_error` — extract zip_error from zip_file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_error_t *`  
`zip_file_get_error`(`zip_file_t *zf`);

# [DESCRIPTION](#DESCRIPTION)

[`zip_file_get_error`](#zip_file_get_error)() function returns the
zip_error associated with the zip_file `zf`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md)

# [HISTORY](#HISTORY)

`zip_file_get_error`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
