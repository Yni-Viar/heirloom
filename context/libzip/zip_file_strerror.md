# [NAME](#NAME)

`zip_file_strerror`, `zip_strerror` — get string representation for a
zip error

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_file_strerror`(`zip_file_t *file`);

`const char *`  
`zip_strerror`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_strerror`](#zip_strerror)() function returns a string
describing the last error for the zip archive `archive`, while the
[`zip_file_strerror`](#zip_file_strerror)() function does the same for a
zip file `file` (one file in an archive). The returned string must not
be modified or freed, and becomes invalid when `archive` or `file`,
respectively, is closed or on the next call to `zip_strerror`() or
`zip_file_strerror`(), respectively, for the same archive.

# [RETURN VALUES](#RETURN_VALUES)

`zip_file_strerror`() and `zip_strerror`() return a pointer to the error
string.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_error_strerror(3)](zip_error_strerror.md)

# [HISTORY](#HISTORY)

`zip_file_strerror`() and `zip_strerror`() were added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
