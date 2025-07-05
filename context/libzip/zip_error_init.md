# [NAME](#NAME)

`zip_error_init`, `zip_error_init_with_code` — initialize zip_error
structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_init`(`zip_error_t *error`);

`void`  
`zip_error_init_with_code`(`zip_error_t *error`, `int ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_init`](#zip_error_init)() function initializes the
zip_error pointed to by `error`. `*error` must be allocated before
calling `zip_error_init`().

The [`zip_error_init_with_code`](#zip_error_init_with_code)() function
does the same, but additionally sets the zip error code to `ze` and sets
the system error code to the current
[errno(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/errno.md)
value, if appropriate.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_error_fini(3)](zip_error_fini.md)

# [HISTORY](#HISTORY)

`zip_error_init`() and `zip_error_init_with_code`() were added in libzip
1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
