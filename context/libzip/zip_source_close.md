# [NAME](#NAME)

`zip_source_close` — close zip_source (which was open for reading)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_close`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_close`](#zip_source_close)() closes `source`,
indicating that no more data will be read.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_free(3)](zip_source_free.md),
[zip_source_open(3)](zip_source_open.md)

# [HISTORY](#HISTORY)

`zip_source_close`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
