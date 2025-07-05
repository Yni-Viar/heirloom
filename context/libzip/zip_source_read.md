# [NAME](#NAME)

`zip_source_read` — read data from zip source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_source_read`(`zip_source_t *source`, `void *data`,
`zip_uint64_t len`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_read`](#zip_source_read)() reads up to `len`
bytes of data from `source` at the current read offset into the buffer
`data`.

The zip source `source` has to be opened for reading by calling
[zip_source_open(3)](zip_source_open.md) first.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion the number of bytes read is returned. When
`zip_source_read`() is called after reaching the end of the file, 0 is
returned. Otherwise, -1 is returned and the error information in
`source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_seek(3)](zip_source_seek.md),
[zip_source_tell(3)](zip_source_tell.md),
[zip_source_write(3)](zip_source_write.md)

# [HISTORY](#HISTORY)

`zip_source_read`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 28, 2021 | NiH |
