# [NAME](#NAME)

`zip_fclose` — close file in zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_fclose`(`zip_file_t *file`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_fclose`](#zip_fclose)() function closes `file` and frees the
memory allocated for it.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, the error code is
returned.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fopen(3)](zip_fopen.md),
[zip_fread(3)](zip_fread.md), [zip_fseek(3)](zip_fseek.md)

# [HISTORY](#HISTORY)

`zip_fclose`() was added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
