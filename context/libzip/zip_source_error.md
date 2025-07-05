# [NAME](#NAME)

`zip_source_error` — get zip error for data source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_error_t *`  
`zip_source_error`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_source_error`](#zip_source_error)() function returns the zip
error for the data source `source`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_system(3)](zip_error_code_system.md),
[zip_error_code_zip(3)](zip_error_code_zip.md)

# [HISTORY](#HISTORY)

`zip_source_error`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
