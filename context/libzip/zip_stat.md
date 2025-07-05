# [NAME](#NAME)

`zip_stat`, `zip_stat_index` — get information about file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_stat`(`zip_t *archive`, `const char *fname`, `zip_flags_t flags`,
`zip_stat_t *sb`);

`int`  
`zip_stat_index`(`zip_t *archive`, `zip_uint64_t index`,
`zip_flags_t flags`, `zip_stat_t *sb`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_stat`](#zip_stat)() function obtains information about the
file named `fname` in `archive`. The `flags` argument specifies how the
name lookup should be done. Its values are described in
[zip_name_locate(3)](zip_name_locate.md). Also, `ZIP_FL_UNCHANGED` may
be [*or*](#or)'ed to it to request information about the original file
in the archive, ignoring any changes made.

The [`zip_stat_index`](#zip_stat_index)() function obtains information
about the file at position `index`.

The `sb` argument is a pointer to a `struct zip_stat` (shown below),
into which information about the file is placed.

    struct zip_stat {
        zip_uint64_t valid;                 /* which fields have valid values */
        const char *name;                   /* name of the file */
        zip_uint64_t index;                 /* index within archive */
        zip_uint64_t size;                  /* size of file (uncompressed) */
        zip_uint64_t comp_size;             /* size of file (compressed) */
        time_t mtime;                       /* modification time */
        zip_uint32_t crc;                   /* crc of file data */
        zip_uint16_t comp_method;           /* compression method used */
        zip_uint16_t encryption_method;     /* encryption method used */
        zip_uint32_t flags;                 /* reserved for future use */
    };

The structure pointed to by `sb` must be allocated before calling
[`zip_stat`](#zip_stat~2)() or `zip_stat_index`().

The `valid` field of the structure specifies which other fields are
valid. Check if the flag defined by the following defines are in `valid`
before accessing the fields:

[`ZIP_STAT_NAME`](#ZIP_STAT_NAME)  
`name`

[`ZIP_STAT_INDEX`](#ZIP_STAT_INDEX)  
`index`

[`ZIP_STAT_SIZE`](#ZIP_STAT_SIZE)  
`size`

[`ZIP_STAT_COMP_SIZE`](#ZIP_STAT_COMP_SIZE)  
`comp_size`

[`ZIP_STAT_MTIME`](#ZIP_STAT_MTIME)  
`mtime`

[`ZIP_STAT_CRC`](#ZIP_STAT_CRC)  
`crc`

[`ZIP_STAT_COMP_METHOD`](#ZIP_STAT_COMP_METHOD)  
`comp_method`

[`ZIP_STAT_ENCRYPTION_METHOD`](#ZIP_STAT_ENCRYPTION_METHOD)  
`encryption_method`

[`ZIP_STAT_FLAGS`](#ZIP_STAT_FLAGS)  
`flags`

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

The function `zip_stat`() can fail for any of the errors specified for
the routine [zip_name_locate(3)](zip_name_locate.md).

The function `zip_stat_index`() fails and sets the error information to
`ZIP_ER_INVAL` if `index` is invalid. If `ZIP_FL_UNCHANGED` is not set
and no information can be obtained from the source callback, the error
information is set to `ZIP_ER_CHANGED`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_get_num_entries(3)](zip_get_num_entries.md),
[zip_name_locate(3)](zip_name_locate.md),
[zip_stat_init(3)](zip_stat_init.md)

# [HISTORY](#HISTORY)

`zip_stat`() was added in libzip 0.6. In libzip 0.11 the type of `flags`
was changed from `int` to `zip_flags_t`.

`zip_stat_index`() was added in libzip 0.6. In libzip 0.10 the type of
`index` was changed from `int` to `zip_uint64_t`. In libzip 0.11 the
type of `flags` was changed from `int` to `zip_flags_t`.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
