# [NAME](#NAME)

`zip_get_num_files` — get number of files in archive (obsolete
interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_get_num_files`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

[*This function is deprecated*](#This). [*Use*](#Use)
[zip_get_num_entries(3)](zip_get_num_entries.md)
[*instead*](#instead).

The [`zip_get_num_files`](#zip_get_num_files)() function returns the
number of files in `archive`.

# [RETURN VALUES](#RETURN_VALUES)

`zip_get_num_files`() returns the number of files in the zip archive, or
-1 if `archive` is `NULL`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fopen_index(3)](zip_fopen_index.md),
[zip_stat_index(3)](zip_stat_index.md)

# [HISTORY](#HISTORY)

`zip_get_num_files`() was added in libzip 0.6. It was deprecated in
libzip 0.11, use `zip_get_num_entries`(`instead`) instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
