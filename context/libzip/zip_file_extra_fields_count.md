# [NAME](#NAME)

`zip_file_extra_fields_count`, `zip_file_extra_fields_count_by_id` —
count extra fields for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int16_t`  
`zip_file_extra_fields_count`(`zip_t *archive`, `zip_uint64_t index`,
`zip_flags_t flags`);

`zip_int16_t`  
`zip_file_extra_fields_count_by_id`(`zip_t *archive`,
`zip_uint64_t index`, `zip_uint16_t extra_field_id`,
`zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_extra_fields_count`](#zip_file_extra_fields_count)()
function counts the extra fields for the file at position `index` in the
zip archive.

The following `flags` are supported:

[`ZIP_FL_CENTRAL`](#ZIP_FL_CENTRAL)  
Count extra fields from the archive's central directory.

[`ZIP_FL_LOCAL`](#ZIP_FL_LOCAL)  
Count extra fields from the local file headers.

[`ZIP_FL_UNCHANGED`](#ZIP_FL_UNCHANGED)  
Count the original unchanged extra fields, ignoring any changes made.

The
[`zip_file_extra_fields_count_by_id`](#zip_file_extra_fields_count_by_id)()
function counts the extra fields with ID (two-byte signature)
`extra_field_id`. The other arguments are the same as for
`zip_file_extra_fields_count`().

Extra fields that are the same in the central directory and the local
file header are merged into one. Therefore, the counts with
`ZIP_FL_CENTRAL` and `ZIP_FL_LOCAL` do not need to add up to the same
value as when given `ZIP_FL_CENTRAL|ZIP_FL_LOCAL` at the same time.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the requested number of extra fields is
returned. Otherwise, -1 is returned and the error code in `archive` is
set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_extra_fields_count`() and
`zip_file_extra_fields_count_by_id`() fail if:

\[[`ZIP_ER_NOENT`](#ZIP_ER_NOENT)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_extra_field_delete(3)](zip_file_extra_field_delete.md),
[zip_file_extra_field_get(3)](zip_file_extra_field_get.md),
[zip_file_extra_field_set(3)](zip_file_extra_field_set.md)

# [HISTORY](#HISTORY)

`zip_file_extra_fields_count`() and
`zip_file_extra_fields_count_by_id`() were added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
