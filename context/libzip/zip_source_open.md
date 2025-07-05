# [NAME](#NAME)

`zip_source_open` — open zip_source for reading

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_open`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_open`](#zip_source_open)() opens `source` for
reading.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_begin_write(3)](zip_source_begin_write.md),
[zip_source_close(3)](zip_source_close.md),
[zip_source_read(3)](zip_source_read.md),
[zip_source_seek(3)](zip_source_seek.md),
[zip_source_tell(3)](zip_source_tell.md)

# [HISTORY](#HISTORY)

`zip_source_open`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
