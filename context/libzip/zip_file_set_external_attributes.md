# [NAME](#NAME)

`zip_file_set_external_attributes` — set external attributes for file in
zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_set_external_attributes`(`zip_t *archive`,
`zip_uint64_t index`, `zip_flags_t flags`, `zip_uint8_t opsys`,
`zip_uint32_t attributes`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_file_set_external_attributes`](#zip_file_set_external_attributes)()
function sets the operating system and external attributes for the file
at position `index` in the zip archive. Currently, no `flags` are
supported.

For a list of known `opsys` values, see
[zip_file_get_external_attributes(3)](zip_file_get_external_attributes.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_set_external_attributes`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
The `archive` was opened in read-only mode.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_get_external_attributes(3)](zip_file_get_external_attributes.md)

# [HISTORY](#HISTORY)

`zip_file_set_external_attributes`() was added in libzip 0.11.2.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
