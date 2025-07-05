# [NAME](#NAME)

`zip_fread` — read from file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_fread`(`zip_file_t *file`, `void *buf`, `zip_uint64_t nbytes`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_fread`](#zip_fread)() function reads up to `nbytes` bytes from
`file` into `buf` from the current position in the file (see
[zip_fseek(3)](zip_fseek.md)). After reading, the current position is
updated by the number of bytes read.

# [RETURN VALUES](#RETURN_VALUES)

If successful, the number of bytes actually read is returned. When
`zip_fread`() is called after reaching the end of the file, 0 is
returned. In case of error, -1 is returned.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fclose(3)](zip_fclose.md),
[zip_file_get_error(3)](zip_file_get_error.md),
[zip_fopen(3)](zip_fopen.md), [zip_fseek(3)](zip_fseek.md)

# [HISTORY](#HISTORY)

`zip_fread`() was added in libzip 0.6. In libzip 0.10 the return type
was changed from `ssize_t` to `zip_int64_t`. In libzip 0.10 the type of
`nbytes` was changed from `size_t` to `zip_uint64_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 11, 2024 | NiH |
