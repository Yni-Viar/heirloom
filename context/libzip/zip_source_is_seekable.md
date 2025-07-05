# [NAME](#NAME)

`zip_source_is_seekable` — check if a source supports seeking

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_is_seekable`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_is_seekable`](#zip_source_is_seekable)()
checks if `source` supports seeking via
[zip_source_seek(3)](zip_source_seek.md).

# [RETURN VALUES](#RETURN_VALUES)

If the source supports seeking, 1 is returned. Otherwise, 0 is returned.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_seek(3)](zip_source_seek.md)

# [HISTORY](#HISTORY)

`zip_source_is_seekable`() was added in libzip 1.10.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 10, 2023 | NiH |
