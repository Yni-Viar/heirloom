# [NAME](#NAME)

`zip_source_keep` — increment reference count of zip data source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_source_keep`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_keep`](#zip_source_keep)() increments the
reference count of `source`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_free(3)](zip_source_free.md)

# [HISTORY](#HISTORY)

`zip_source_keep`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
