# [NAME](#NAME)

`zip_ftell` — tell position in file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_ftell`(`zip_file_t *file`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_ftell`](#zip_ftell)() function reports the current offset in
the file.

# [RETURN VALUES](#RETURN_VALUES)

If successful, `zip_ftell` returns the current file position. Otherwise,
-1 is returned.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fclose(3)](zip_fclose.md),
[zip_file_get_error(3)](zip_file_get_error.md),
[zip_fopen(3)](zip_fopen.md), [zip_fread(3)](zip_fread.md),
[zip_fseek(3)](zip_fseek.md)

# [HISTORY](#HISTORY)

`zip_ftell`() was added in libzip 1.2.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 11, 2024 | NiH |
