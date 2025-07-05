# [NAME](#NAME)

`zip_add`, `zip_replace` — add file to zip archive or replace file in
zip archive (obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_add`(`zip_t *archive`, `const char *name`, `zip_source_t *source`);

`int`  
`zip_replace`(`zip_t *archive`, `zip_uint64_t index`,
`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_add`](#zip_add)() is the obsolete version of
[zip_file_add(3)](zip_file_add.md). It is the same as calling
[zip_file_add(3)](zip_file_add.md) with an empty `flags` argument.
Similarly, the [`zip_replace`](#zip_replace)() function is the obsolete
version of [zip_file_replace(3)](zip_file_replace.md). It is the same
as calling [zip_file_replace(3)](zip_file_replace.md) with an empty
`flags` argument.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md)

# [HISTORY](#HISTORY)

`zip_add`() was added in libzip 0.6. In libzip 0.10 the return type was
changed from `int` to `zip_int64_t`. It was deprecated in libzip 0.11,
use `zip_file_add`() instead.

`zip_replace`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`. It was deprecated in
libzip 0.11, use `zip_file_replace`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
