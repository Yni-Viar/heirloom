# [NAME](#NAME)

`zip_error_code_system` — get operating system error part of zip_error

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_error_code_system`(`const zip_error_t *ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_code_system`](#zip_error_code_system)() function returns
the system specific part of the error from the zip_error error `ze`. For
finding out what system reported the error, use
[zip_error_system_type(3)](zip_error_system_type.md).

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_zip(3)](zip_error_code_zip.md),
[zip_error_system_type(3)](zip_error_system_type.md)

# [HISTORY](#HISTORY)

`zip_error_code_system`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
