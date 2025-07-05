# [NAME](#NAME)

`zip_source_is_deleted` — check if zip_source is deleted

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_is_deleted`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_is_deleted`](#zip_source_is_deleted)() returns
whether the zip_source was deleted. This can for example happen when all
entries are removed from a zip archive.

# [RETURN VALUES](#RETURN_VALUES)

`zip_source_is_deleted`() returns 1 if the zip_source is deleted and 0
otherwise.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_is_deleted`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
