# [NAME](#NAME)

`zip_unchange_archive` — undo global changes to zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_unchange_archive`(`zip_t *archive`);

# [DESCRIPTION](#DESCRIPTION)

Revert all global changes to the archive `archive`. This reverts changes
to the archive comment and global flags.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_unchange(3)](zip_unchange.md),
[zip_unchange_all(3)](zip_unchange_all.md)

# [HISTORY](#HISTORY)

`zip_unchange_archive`() was added in libzip 0.7.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
