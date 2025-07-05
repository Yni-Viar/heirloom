# [NAME](#NAME)

`zip_file_extra_field_get`, `zip_file_extra_field_get_by_id` — get extra
field for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const zip_uint8_t *`  
`zip_file_extra_field_get`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t extra_field_index`, `zip_uint16_t *idp`,
`zip_uint16_t *lenp`, `zip_flags_t flags`);

`const zip_uint8_t *`  
`zip_file_extra_field_get_by_id`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t extra_field_id`, `zip_uint16_t extra_field_index`,
`zip_uint16_t *lenp`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_extra_field_get`](#zip_file_extra_field_get)() function
returns the extra field with index `extra_field_index` for the file at
position `index` in the zip archive. This pointer should not be modified
or
[free(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.md)'d,
and becomes invalid when `archive` is closed. If `idp` is not `NULL`,
the integer to which it points will be set to the ID (two-byte
signature) of the selected extra field. If `lenp` is not `NULL`, the
integer to which it points will be set to the length of the extra field.
Generally speaking, `lenp` and `idp` should be passed since only the
extra field data is returned (i.e., neither the ID nor the length, if
the `idp` and `lenp` arguments are not provided).

The following `flags` are supported:

[`ZIP_FL_CENTRAL`](#ZIP_FL_CENTRAL)  
Return extra fields from the archive's central directory.

[`ZIP_FL_LOCAL`](#ZIP_FL_LOCAL)  
Return extra fields from the local file headers.

[`ZIP_FL_UNCHANGED`](#ZIP_FL_UNCHANGED)  
Return the original unchanged extra fields, ignoring any changes made.

The
[`zip_file_extra_field_get_by_id`](#zip_file_extra_field_get_by_id)()
function returns the extra field with ID (two-byte signature)
`extra_field_id` and index `extra_field_index` (in other words, the
`extra_field_index`'th extra field with ID `extra_field_id`) The other
arguments are the same as for `zip_file_extra_field_get`().

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, a pointer to an extra field is returned, or
`NULL` if there is no extra field with that `extra_field_index` for the
file with index `index`. In case of an error, `NULL` is returned and the
error code in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_extra_field_get`() and `zip_file_extra_field_get_by_id`() fail
if:

\[[`ZIP_ER_NOENT`](#ZIP_ER_NOENT)\]  
`index` is not a valid file index in `archive`, or `extra_field_index`
is not a valid extra file index (for ID `extra_field_id`).

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_extra_field_delete(3)](zip_file_extra_field_delete.md),
[zip_file_extra_field_set(3)](zip_file_extra_field_set.md),
[zip_file_extra_fields_count(3)](zip_file_extra_fields_count.md)

# [HISTORY](#HISTORY)

`zip_file_extra_field_get`() and `zip_file_extra_field_get_by_id`() were
added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

# [CAVEATS](#CAVEATS)

Please note that the extra field IDs 0x0001 (ZIP64 extension), 0x6375
(Infozip UTF-8 comment), and 0x7075 (Infozip UTF-8 file name) can not be
read using `zip_file_extra_field_get`() since they are used by
[libzip(3)](libzip.md) internally.

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
