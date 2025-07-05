# [NAME](#NAME)

`zip_error_clear`, `zip_file_error_clear` — clear error state for
archive or file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_clear`(`zip_t *archive`);

`void`  
`zip_file_error_clear`(`zip_file_t *file`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_clear`](#zip_error_clear)() function clears the error
state for the zip archive `archive`.

The [`zip_file_error_clear`](#zip_file_error_clear)() function does the
same for the zip file `file`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_get_error(3)](zip_get_error.md)

# [HISTORY](#HISTORY)

`zip_error_clear`() and `zip_file_error_clear`() were added in libzip
0.8.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
