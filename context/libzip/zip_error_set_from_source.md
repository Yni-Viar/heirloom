# [NAME](#NAME)

`zip_error_set_from_source` — fill in zip_error structure from source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_error_set_from_source`(`zip_error_t *ze`, `zip_source_t *src`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_set_from_source`](#zip_error_set_from_source)() function
sets the zip_error pointed to by `ze` to the error reported by `src` as
returned by [zip_error_source(3)](zip_error_source.md). `ze` must be
allocated and initialized with [zip_error_init(3)](zip_error_init.md)
before calling `zip_error_set_from_source`().

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_error_init(3)](zip_error_init.md),
[zip_error_set(3)](zip_error_set.md)

# [HISTORY](#HISTORY)

`zip_error_set_from_source`() was added in libzip 1.10.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| December 5, 2022 | NiH |
