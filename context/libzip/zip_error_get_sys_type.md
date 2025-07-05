# [NAME](#NAME)

`zip_error_get_sys_type` — get type of system error code (obsolete
interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_error_get_sys_type`(`int ze`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_error_get_sys_type`](#zip_error_get_sys_type)() is
deprecated; use
[zip_error_init_with_code(3)](zip_error_init_with_code.md) and
[zip_error_system_type(3)](zip_error_system_type.md) instead.

Replace

    int i = zip_error_get_sys_type(ze);

with

    zip_error_t error;
    zip_error_init_with_code(&error, ze);
    int i = zip_error_system_type(&error);

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_init_with_code(3)](zip_error_init_with_code.md),
[zip_error_system_type(3)](zip_error_system_type.md)

# [HISTORY](#HISTORY)

`zip_error_get_sys_type`() was added in libzip 0.6. It was deprecated in
libzip 1.0, use `zip_error_system_type`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
