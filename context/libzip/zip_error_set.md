# [NAME](#NAME)

`zip_error_set` — fill in zip_error structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_set`(`zip_error_t *ze`, `int le`, `int se`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_set`](#zip_error_set)() function sets the zip_error
pointed to by `ze` to the libzip error code `le` and the system error
code `se`.

`ze` must be allocated and initialized with
[zip_error_init(3)](zip_error_init.md) before calling
[`zip_error_set`](#zip_error_set~2)().

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_error_init(3)](zip_error_init.md),
[zip_error_set_from_source(3)](zip_error_set_from_source.md)

# [HISTORY](#HISTORY)

`zip_error_set`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| December 5, 2022 | NiH |
