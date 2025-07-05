# [NAME](#NAME)

`zip_source_seek` — set read offset in zip source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_seek`(`zip_source_t *source`, `zip_int64_t offset`,
`int whence`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_seek`](#zip_source_seek)() sets the current
read offset for `source`. Just like in
[fseek(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fseek.md),
depending on the `whence` argument, the `offset` is counted relative
from:

[`SEEK_SET`](#SEEK_SET)  
start of file

[`SEEK_CUR`](#SEEK_CUR)  
current read offset in file

[`SEEK_END`](#SEEK_END)  
end of file

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_is_seekable(3)](zip_source_is_seekable.md),
[zip_source_read(3)](zip_source_read.md),
[zip_source_tell(3)](zip_source_tell.md)

# [HISTORY](#HISTORY)

`zip_source_seek`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 10, 2023 | NiH |
