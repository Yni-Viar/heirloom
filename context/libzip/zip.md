# [NAME](#NAME)

`zip` — zip archive structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_t *archive`;

# [DESCRIPTION](#DESCRIPTION)

A `zip` represents an open zip archive and is used for all functions
accessing and modifying archives.

It is created with [zip_open(3)](zip_open.md),
[zip_open_from_source(3)](zip_open_from_source.md), or
[zip_fdopen(3)](zip_fdopen.md).

It is closed with [zip_close(3)](zip_close.md) (keeping changes) or
[zip_discard(3)](zip_discard.md) (discarding changes).

All objects representing parts of an archive (like `zip_file_t`) are
only valid while the archive remains open.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md),
[zip_discard(3)](zip_discard.md), [zip_fopen(3)](zip_fopen.md),
[zip_open(3)](zip_open.md),
[zip_open_from_source(3)](zip_open_from_source.md)

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|             |     |
|-------------|-----|
| May 5, 2025 | NiH |
