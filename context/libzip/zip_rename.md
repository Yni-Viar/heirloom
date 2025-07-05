# [NAME](#NAME)

`zip_rename` — rename file in zip archive (obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_rename`(`zip_t *archive`, `zip_uint64_t index`,
`const char *name`);

# [DESCRIPTION](#DESCRIPTION)

[`zip_rename`](#zip_rename)() is the obsolete version of
[zip_file_rename(3)](zip_file_rename.md). It is the same as calling
[zip_file_rename(3)](zip_file_rename.md) with an empty flags argument.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_rename(3)](zip_file_rename.md)

# [HISTORY](#HISTORY)

`zip_rename`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`. It was deprecated in
libzip 0.11, use `zip_file_rename`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
