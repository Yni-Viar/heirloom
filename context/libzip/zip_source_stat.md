# [NAME](#NAME)

`zip_source_stat` — get information about zip_source

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_source_stat`(`zip_source_t *source`, `zip_stat_t *sb`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_source_stat`](#zip_source_stat)() function obtains information
about the zip source `source`

The `sb` argument is a pointer to a `struct zip_source_stat` (shown
below), into which information about the zip source is placed.

    struct zip_source_stat {
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

The structure pointed to by `sb` must be initialized with
[`zip_stat_init`](#zip_stat_init)(`3`) before calling
`zip_source_stat`().

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

[*NOTE*](#NOTE): Some fields may only be filled out after all data has
been read from the source, for example the `crc` or `size` fields.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `source` is set to indicate the error.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_stat`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
