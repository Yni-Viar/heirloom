# [NAME](#NAME)

`zip_error` — error information

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_error_t error`;

# [DESCRIPTION](#DESCRIPTION)

A `zip_error` represents information about an error. It is usually
allocated directly on the stack or as member of another structure, not
via a pointer.

It is initialized with [zip_error_init(3)](zip_error_init.md) or
[zip_error_init_with_code(3)](zip_error_init_with_code.md).

The type of error can be accessed with
[zip_error_code_zip(3)](zip_error_code_zip.md) and
[zip_error_code_system(3)](zip_error_code_system.md). It can be
converted to a human readable string with
[zip_error_strerror(3)](zip_error_strerror.md).

After use, it should be cleaned up with
[zip_error_fini(3)](zip_error_fini.md).

# [SEE ALSO](#SEE_ALSO)

[zip_error_code_system(3)](zip_error_code_system.md),
[zip_error_code_zip(3)](zip_error_code_zip.md),
[zip_error_fini(3)](zip_error_fini.md),
[zip_error_init(3)](zip_error_init.md),
[zip_error_init_with_code(3)](zip_error_init_with_code.md),
[zip_error_strerror(3)](zip_error_strerror.md)

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|             |     |
|-------------|-----|
| May 5, 2025 | NiH |
