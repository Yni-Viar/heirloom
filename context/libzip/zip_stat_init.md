# [NAME](#NAME)

`zip_stat_init` — initialize zip_stat structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_stat_init`(`zip_stat_t *sb`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_stat_init`](#zip_stat_init)() function initializes the members
of a struct zip_stat. The current members are described in
[zip_stat(3)](zip_stat.md), but this function should be used to
initialize it to make sure none are missed. The structure pointed to by
`sb` must be allocated before calling `zip_stat_init`().

This function should be used by functions provided to
[zip_source_function(3)](zip_source_function.md) when returning
`ZIP_SOURCE_STAT` information to make sure all fields are initialized.

# [RETURN VALUES](#RETURN_VALUES)

If `sb` is valid, the function is always successful.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_stat(3)](zip_stat.md)

# [HISTORY](#HISTORY)

`zip_stat_init`() was added in libzip 0.8.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
