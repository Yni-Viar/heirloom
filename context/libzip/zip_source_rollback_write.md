# [NAME](#NAME)

`zip_source_rollback_write` — undo changes to zip source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_source_rollback_write`(`zip_source_t *source`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_source_rollback_write`](#zip_source_rollback_write)()
reverts changes written to `source`, restoring the data before
[zip_source_begin_write(3)](zip_source_begin_write.md) was called.
Usually this removes temporary files or frees buffers.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md),
[zip_source_begin_write(3)](zip_source_begin_write.md),
[zip_source_commit_write(3)](zip_source_commit_write.md),
[zip_source_seek_write(3)](zip_source_seek_write.md),
[zip_source_tell_write(3)](zip_source_tell_write.md),
[zip_source_write(3)](zip_source_write.md)

# [HISTORY](#HISTORY)

`zip_source_rollback_write`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| November 3, 2021 | NiH |
