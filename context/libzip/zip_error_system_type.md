# [NAME](#NAME)

`zip_error_system_type` — return type of system error

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_error_system_type`(`const zip_error_t *ze`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_error_system_type`](#zip_error_system_type)() function returns
the type of the system specific part for the zip_error `ze`. Currently,
the following system types are defined:

[`ZIP_ET_NONE`](#ZIP_ET_NONE)  
System specific part of `ze` is unused.

[`ZIP_ET_SYS`](#ZIP_ET_SYS)  
System specific part of `ze` is an
[errno(2)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/errno.md).

[`ZIP_ET_ZLIB`](#ZIP_ET_ZLIB)  
System specific part of `ze` is a
[zlib(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/zlib.md)
error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_code_system(3)](zip_error_code_system.md)

# [HISTORY](#HISTORY)

`zip_error_system_type`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
