# [NAME](#NAME)

`zip_add_dir` — add directory to zip archive (obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_add_dir`(`zip_t *archive`, `const char *name`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_add_dir`](#zip_add_dir)() is the obsolete version of
[zip_dir_add(3)](zip_dir_add.md). It is the same as calling
[zip_dir_add(3)](zip_dir_add.md) with an empty flags argument.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_dir_add(3)](zip_dir_add.md)

# [HISTORY](#HISTORY)

`zip_add_dir`() was added in libzip 0.8. In libzip 0.10 the return type
was changed from `int` to `zip_int64_t`. It was deprecated in libzip
0.11, use `zip_dir_add`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
