# [NAME](#NAME)

`zip_error_to_data` — convert zip_error to return value suitable for
ZIP_SOURCE_ERROR

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_error_to_data`(`const zip_error_t *ze`, `void *data`,
`zip_uint64_t len`);

# [DESCRIPTION](#DESCRIPTION)

[`zip_error_to_data`](#zip_error_to_data)() function converts the
zip_error `ze` into data suitable as return value for
`ZIP_SOURCE_ERROR`. The data is written into the buffer `data` of size
`len`. If the buffer is not large enough to hold 2 ints, an error is
returned.

# [RETURN VALUES](#RETURN_VALUES)

`zip_error_to_data`() returns 2\*(sizeof int) on success, and -1 on
error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_source_function(3)](zip_source_function.md)

# [HISTORY](#HISTORY)

`zip_error_to_data`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
