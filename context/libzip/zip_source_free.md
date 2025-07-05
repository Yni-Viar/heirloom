# [NAME](#NAME)

`zip_source_free` — free zip data source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_source_free`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_free`](#zip_source_free)() decrements the
reference count of `source` and frees it if the reference count drops to
0. If `source` is `NULL`, it does nothing.

[*NOTE*](#NOTE): This function should not be called on a `source` after
it was used successfully in a
[zip_open_from_source(3)](zip_open_from_source.md),
[zip_file_add(3)](zip_file_add.md), or
[zip_file_replace(3)](zip_file_replace.md) call.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_keep(3)](zip_source_keep.md)

# [HISTORY](#HISTORY)

`zip_source_free`() was added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
