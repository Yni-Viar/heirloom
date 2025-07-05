# [NAME](#NAME)

`zip_file_get_external_attributes` — get external attributes for file in
zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_get_external_attributes`(`zip_t *archive`,
`zip_uint64_t index`, `zip_flags_t flags`, `zip_uint8_t *opsys`,
`zip_uint32_t *attributes`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_file_get_external_attributes`](#zip_file_get_external_attributes)()
function returns the operating system and external attributes for the
file at position `index` in the zip archive. The external attributes
usually contain the operating system-specific file permissions. If
`flags` is set to `ZIP_FL_UNCHANGED`, the original unchanged values are
returned. If `opsys` or `attributes` are `NULL`, they are not filled in.

The following operating systems are defined by the zip specification:

- [`ZIP_OPSYS_ACORN_RISC`](#ZIP_OPSYS_ACORN_RISC)
- [`ZIP_OPSYS_ALTERNATE_MVS`](#ZIP_OPSYS_ALTERNATE_MVS)
- [`ZIP_OPSYS_AMIGA`](#ZIP_OPSYS_AMIGA)
- [`ZIP_OPSYS_ATARI_ST`](#ZIP_OPSYS_ATARI_ST)
- [`ZIP_OPSYS_BEOS`](#ZIP_OPSYS_BEOS)
- [`ZIP_OPSYS_CPM`](#ZIP_OPSYS_CPM)
- [`ZIP_OPSYS_DOS`](#ZIP_OPSYS_DOS)
- [`ZIP_OPSYS_MACINTOSH`](#ZIP_OPSYS_MACINTOSH)
- [`ZIP_OPSYS_MVS`](#ZIP_OPSYS_MVS)
- [`ZIP_OPSYS_OPENVMS`](#ZIP_OPSYS_OPENVMS)
- [`ZIP_OPSYS_OS_2`](#ZIP_OPSYS_OS_2)
- [`ZIP_OPSYS_OS_400`](#ZIP_OPSYS_OS_400)
- [`ZIP_OPSYS_OS_X`](#ZIP_OPSYS_OS_X)
- [`ZIP_OPSYS_TANDEM`](#ZIP_OPSYS_TANDEM)
- [`ZIP_OPSYS_UNIX`](#ZIP_OPSYS_UNIX)
- [`ZIP_OPSYS_VFAT`](#ZIP_OPSYS_VFAT)
- [`ZIP_OPSYS_VM_CMS`](#ZIP_OPSYS_VM_CMS)
- [`ZIP_OPSYS_VSE`](#ZIP_OPSYS_VSE)
- [`ZIP_OPSYS_WINDOWS_NTFS`](#ZIP_OPSYS_WINDOWS_NTFS) (uncommon, use
  `ZIP_OPSYS_DOS` instead)
- [`ZIP_OPSYS_Z_SYSTEM`](#ZIP_OPSYS_Z_SYSTEM)

The defines above follow the PKWARE Inc. Appnote; please note that the
InfoZIP Appnote has a slightly different mapping.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, 0 is returned. In case of an error, `-1` is
returned and the error code in `archive` is set to indicate the error.

# [EXAMPLES](#EXAMPLES)

The following code can be used to expand `attributes` if the operating
system is `ZIP_OPSYS_DOS`.

    #include <sys/stat.h>

    #define FA_RDONLY       0x01            // FILE_ATTRIBUTE_READONLY
    #define FA_DIREC        0x10            // FILE_ATTRIBUTE_DIRECTORY

    static mode_t
    _zip_dos_attr2mode(zip_uint32_t attr)
    {
       mode_t m = S_IRUSR | S_IRGRP | S_IROTH;
       if (0 == (attr & FA_RDONLY))
          m |= S_IWUSR | S_IWGRP | S_IWOTH;

       if (attr & FA_DIREC)
          m = (S_IFDIR | (m & ~S_IFMT)) | S_IXUSR | S_IXGRP | S_IXOTH;

       return m;
    }

# [ERRORS](#ERRORS)

`zip_file_get_external_attributes`() fails if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_file_set_external_attributes(3)](zip_file_set_external_attributes.md)

# [HISTORY](#HISTORY)

`zip_file_get_external_attributes`() was added in libzip 0.11.2.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
