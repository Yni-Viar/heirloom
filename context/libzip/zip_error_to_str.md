# [NAME](#NAME)

`zip_error_to_str` — get string representation of zip error (obsolete
interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_error_to_str`(`char *buf`, `zip_uint64_t len`, `int ze`, `int se`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_error_to_str`](#zip_error_to_str)() is deprecated;
use [zip_error_init_with_code(3)](zip_error_init_with_code.md) and
[zip_error_strerror(3)](zip_error_strerror.md) instead.

Replace

    char buf[BUFSIZE];
    zip_error_to_str(buf, sizeof(buf), ze, se);
    printf("%s", buf);

with

    zip_error_t error;
    zip_error_init_with_code(&error, ze);
    printf("%s", zip_error_strerror(&error));
    zip_error_fini(&error);

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_init_with_code(3)](zip_error_init_with_code.md),
[zip_error_strerror(3)](zip_error_strerror.md)

# [HISTORY](#HISTORY)

`zip_error_to_str`() was added in libzip 0.6. In libzip 0.10 the type of
`len` was changed from `size_t` to `zip_uint64_t`. It was deprecated in
libzip 1.0, use `zip_error_init_with_code`() and `zip_error_strerror`()
instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
