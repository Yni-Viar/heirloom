# [NAME](#NAME)

`zip_file_extra_field_delete`, `zip_file_extra_field_delete_by_id` —
delete extra field for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_extra_field_delete`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t extra_field_index`, `zip_flags_t flags`);

`int`  
`zip_file_extra_field_delete_by_id`(`zip_t *archive`,
`zip_uint64_t index`, `zip_uint16_t extra_field_id`,
`zip_uint16_t extra_field_index`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_extra_field_delete`](#zip_file_extra_field_delete)()
function deletes the extra field with index `extra_field_index` for the
file at position `index` in the zip archive.

If `extra_field_index` is `ZIP_EXTRA_FIELD_ALL`, then all extra fields
will be deleted.

The following `flags` are supported:

[`ZIP_FL_CENTRAL`](#ZIP_FL_CENTRAL)  
Delete extra fields from the archive's central directory.

[`ZIP_FL_LOCAL`](#ZIP_FL_LOCAL)  
Delete extra fields from the local file headers.

The
[`zip_file_extra_field_delete_by_id`](#zip_file_extra_field_delete_by_id)()
function deletes the extra field with ID (two-byte signature)
`extra_field_id` and index `extra_field_index` (in other words, the
`extra_field_index`'th extra field with ID `extra_field_id`) The other
arguments are the same as for `zip_file_extra_field_delete`()
(`ZIP_EXTRA_FIELD_ALL` will delete all extra fields of the specified
ID).

Please note that due to the library design, the index of an extra field
may be different between central directory and local file headers. For
this reason, it is not allowed to specify both `ZIP_FL_CENTRAL` and
`ZIP_FL_LOCAL` in `flags`, except when deleting all extra fields (i.e.,
`extra_field_index` being `ZIP_EXTRA_FIELD_ALL`).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_extra_field_delete`() and
`zip_file_extra_field_delete_by_id`() fail if:

\[[`ZIP_ER_NOENT`](#ZIP_ER_NOENT)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_extra_field_get(3)](zip_file_extra_field_get.md),
[zip_file_extra_field_set(3)](zip_file_extra_field_set.md),
[zip_file_extra_fields_count(3)](zip_file_extra_fields_count.md)

# [HISTORY](#HISTORY)

`zip_file_extra_field_delete`() and
`zip_file_extra_field_delete_by_id`() were added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
