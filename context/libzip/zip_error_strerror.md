# [NAME](#NAME)

`zip_error_strerror` — create human-readable string for zip_error

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_error_strerror`(`zip_error_t *ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_strerror`](#zip_error_strerror)() function returns an
error message string corresponding to `ze` like
[strerror(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/strerror.md).
This string will stay valid until the next call to
`zip_error_strerror`() or until [zip_error_fini(3)](zip_error_fini.md)
is called.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[strerror(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/strerror.md),
[zip_error_fini(3)](zip_error_fini.md)

# [HISTORY](#HISTORY)

`zip_error_strerror`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
