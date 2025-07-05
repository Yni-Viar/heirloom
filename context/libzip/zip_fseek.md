# [NAME](#NAME)

`zip_fseek`, `zip_file_is_seekable` — seek in file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int8_t`  
`zip_fseek`(`zip_file_t *file`, `zip_int64_t offset`, `int whence`);

`int`  
`zip_file_is_seekable`(`zip_file_t *file`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_fseek`](#zip_fseek)() function seeks to the specified `offset`
relative to `whence`, just like
[fseek(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fseek.md).

`zip_fseek` only works on uncompressed (stored), unencrypted data. When
called on compressed or encrypted data it will return an error.

The [`zip_file_is_seekable`](#zip_file_is_seekable)() function returns 1
if a file is seekable.

# [RETURN VALUES](#RETURN_VALUES)

If successful, `zip_fseek`() returns 0. Otherwise, -1 is returned.

`zip_file_is_seekable`() returns 1 if a file is seekable and 0 if not.
On an invalid argument, it returns -1.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fclose(3)](zip_fclose.md),
[zip_file_get_error(3)](zip_file_get_error.md),
[zip_fopen(3)](zip_fopen.md), [zip_fread(3)](zip_fread.md),
[zip_ftell(3)](zip_ftell.md)

# [HISTORY](#HISTORY)

`zip_fseek`() was added in libzip 1.2.0. `zip_file_is_seekable`() was
added in libzip 1.9.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 11, 2024 | NiH |
