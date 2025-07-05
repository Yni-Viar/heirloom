# [NAME](#NAME)

`zip_source_tell` — report current read offset in zip source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_source_tell`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_tell`](#zip_source_tell)() returns the current
read offset for `source`. The return value can be passed to
[zip_source_seek(3)](zip_source_seek.md) with `whence` set to
`SEEK_SET` to return to the same location in the source.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion the current read offset is returned.
Otherwise, -1 is returned and the error information in `source` is set
to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_read(3)](zip_source_read.md),
[zip_source_tell_write(3)](zip_source_tell_write.md)

# [HISTORY](#HISTORY)

`zip_source_tell`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
