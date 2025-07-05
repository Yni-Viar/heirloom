# [NAME](#NAME)

`zip_set_file_comment` — set comment for file in zip (obsolete
interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_set_file_comment`(`zip_t *archive`, `zip_uint64_t index`,
`const char *comment`, `int len`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_set_file_comment`](#zip_set_file_comment)() function is the
obsolete version of
[zip_file_set_comment(3)](zip_file_set_comment.md). The only
differences are the type of the `len` argument and the additional
`flags` argument. `zip_set_file_comment`() is the same as calling
[zip_file_set_comment(3)](zip_file_set_comment.md) with an empty
`flags` argument.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_set_comment(3)](zip_file_set_comment.md)

# [HISTORY](#HISTORY)

`zip_set_file_comment`() was added in libzip 0.7. In libzip 0.10 the
type of `index` was changed from `int` to `zip_uint64_t`. It was
deprecated in libzip 0.11, use `zip_file_set_comment`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
