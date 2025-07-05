# [NAME](#NAME)

`zip_get_file_comment` — get comment for file in zip (obsolete
interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_get_file_comment`(`zip_t *archive`, `zip_uint64_t index`,
`int *lenp`, `int flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_file_comment`](#zip_get_file_comment)() function is the
obsolete version of
[zip_file_get_comment(3)](zip_file_get_comment.md). The only
differences are the types of the `lenp` and `flags` arguments.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_get_comment(3)](zip_file_get_comment.md)

# [HISTORY](#HISTORY)

`zip_get_file_comment`() was added in libzip 0.7. In libzip 0.10 the
type of `index` was changed from `int` to `zip_uint64_t`. It was
deprecated in libzip 0.11, use `zip_file_get_comment`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
