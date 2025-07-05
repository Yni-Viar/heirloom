# [NAME](#NAME)

`zip_source_begin_write`, `zip_source_begin_write_cloning` — prepare zip
source for writing

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_begin_write`(`zip_source_t *source`);

`int`  
`zip_source_begin_write_cloning`(`zip_source_t *source`,
`zip_uint64_t offset`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_begin_write`](#zip_source_begin_write)() and
`zip_source_begin_write_cloning`() prepare `source` for writing. Usually
this involves creating temporary files or allocating buffers.

[`zip_source_begin_write_cloning`](#zip_source_begin_write_cloning)()
preserves the first `offset` bytes of the original file. This is done
efficiently, and writes to `source` won't overwrite the original data
until [`zip_commit_write`](#zip_commit_write)() is called.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_commit_write(3)](zip_source_commit_write.md),
[zip_source_rollback_write(3)](zip_source_rollback_write.md),
[zip_source_seek_write(3)](zip_source_seek_write.md),
[zip_source_tell_write(3)](zip_source_tell_write.md),
[zip_source_write(3)](zip_source_write.md)

# [HISTORY](#HISTORY)

`zip_source_begin_write`() was added in libzip 1.0.

`zip_source_begin_write_cloning`() was added in libzip 1.4.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
