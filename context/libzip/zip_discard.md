# [NAME](#NAME)

`zip_discard` — close zip archive and discard changes

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_discard`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_discard`](#zip_discard)() function closes `archive` and frees
the memory allocated for it. Any changes to the archive are not written
to disk and discarded.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md)

# [HISTORY](#HISTORY)

`zip_discard`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
