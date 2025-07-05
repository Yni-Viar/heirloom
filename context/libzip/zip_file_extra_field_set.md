# [NAME](#NAME)

`zip_file_extra_field_set` — set extra field for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_extra_field_set`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t extra_field_id`, `zip_uint16_t extra_field_index`,
`const zip_uint8_t *extra_field_data`, `zip_uint16_t len`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_extra_field_set`](#zip_file_extra_field_set)() function
sets the extra field with ID (two-byte signature) `extra_field_id` and
index `extra_field_index` for the file at position `index` in the zip
archive. The extra field's data will be set to `extra_field_data` and
length `len`. If a new entry shall be appended, set `extra_field_index`
to `ZIP_EXTRA_FIELD_NEW`.

At least one of the following `flags` must be set:

[`ZIP_FL_CENTRAL`](#ZIP_FL_CENTRAL)  
Set extra field in the archive's central directory.

[`ZIP_FL_LOCAL`](#ZIP_FL_LOCAL)  
Set extra field in the local file headers.

Please note that the extra field IDs 0x0001 (ZIP64 extension), 0x6375
(Infozip UTF-8 comment), and 0x7075 (Infozip UTF-8 file name) can not be
set using [`zip_file_extra_field_set`](#zip_file_extra_field_set~2)()
since they are set by [libzip(3)](libzip.md) automatically when
needed.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_extra_field_set`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
The extra field size is too large (ID and length need 4 bytes; the
maximum length of all extra fields for one file combined is 65536
bytes). This error also occurs if `extra_field_index` is too large.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_NOENT`](#ZIP_ER_NOENT)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_extra_field_delete(3)](zip_file_extra_field_delete.md),
[zip_file_extra_field_get(3)](zip_file_extra_field_get.md),
[zip_file_extra_fields_count(3)](zip_file_extra_fields_count.md)

# [HISTORY](#HISTORY)

`zip_file_extra_field_set`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
