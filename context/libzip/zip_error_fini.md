# [NAME](#NAME)

`zip_error_fini` — clean up zip_error structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_fini`(`zip_error_t *ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_fini`](#zip_error_fini)() function cleans up and frees
internally allocated memory of the zip_error pointed to by `ze`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_error_init(3)](zip_error_init.md)

# [HISTORY](#HISTORY)

`zip_error_fini`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
