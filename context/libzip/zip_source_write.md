# [NAME](#NAME)

`zip_source_write` — write data to zip source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_source_write`(`zip_source_t *source`, `const void *data`,
`zip_uint64_t len`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_write`](#zip_source_write)() writes `len`
bytes from the buffer `data` to the zip source `source` at the current
write offset.

The zip source `source` has to be prepared for writing by calling
[zip_source_begin_write(3)](zip_source_begin_write.md) first.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion the number of bytes written is returned.
Otherwise, -1 is returned and the error information in `source` is set
to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_begin_write(3)](zip_source_begin_write.md),
[zip_source_commit_write(3)](zip_source_commit_write.md),
[zip_source_rollback_write(3)](zip_source_rollback_write.md),
[zip_source_seek_write(3)](zip_source_seek_write.md),
[zip_source_tell_write(3)](zip_source_tell_write.md)

# [HISTORY](#HISTORY)

`zip_source_write`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
