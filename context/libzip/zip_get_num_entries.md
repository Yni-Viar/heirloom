# [NAME](#NAME)

`zip_get_num_entries` — get number of entries in archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_get_num_entries`(`zip_t *archive`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_get_num_entries`](#zip_get_num_entries)() function returns the
number of entries in `archive`. Entries are all files that are present
in the original archive or that were added while the archive is open.
This includes deleted files, since indices are not renumbered until the
archive is closed. (This allows one to refer to deleted files, e. g. to
undelete them.)

If `flags` is set to `ZIP_FL_UNCHANGED`, the original number of files is
returned.

# [RETURN VALUES](#RETURN_VALUES)

`zip_get_num_entries`() returns the number of entries in the zip
archive, or -1 if `archive` is `NULL`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_fopen_index(3)](zip_fopen_index.md),
[zip_stat_index(3)](zip_stat_index.md)

# [HISTORY](#HISTORY)

`zip_get_num_entries`() was added in libzip 0.10. In libzip 0.11 the
return type was changed from `zip_uint64_t` to `zip_int64_t`. In libzip
0.11 the type of `flags` was changed from `int` to `zip_flags_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                 |     |
|-----------------|-----|
| August 19, 2022 | NiH |
