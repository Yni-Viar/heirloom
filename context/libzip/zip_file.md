# [NAME](#NAME)

`zip_file` — file in archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_file_t *file`;

# [DESCRIPTION](#DESCRIPTION)

A `zip_file` represents a file inside an archive, opened for reading. It
is created with [zip_fopen(3)](zip_fopen.md),
[zip_fopen_index(3)](zip_fopen_index.md),
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md), or
[zip_fopen_index_encrypted(3)](zip_fopen_index_encrypted.md).

Data is accessed with [zip_fread(3)](zip_fread.md),
[zip_file_is_seekable(3)](zip_file_is_seekable.md),
[zip_ftell(3)](zip_ftell.md), or [zip_fseek(3)](zip_fseek.md).

It is closed with [zip_fclose(3)](zip_fclose.md).

If the containing `zip_t` is closed, all further uses of the
`zip_file_t` return an error. It is a convenience wrapper around a
`zip_source_t` as created by [zip_source_zip(3)](zip_source_zip.md).

# [SEE ALSO](#SEE_ALSO)

[zip_fclose(3)](zip_fclose.md),
[zip_file_is_seekable(3)](zip_file_is_seekable.md),
[zip_fopen(3)](zip_fopen.md),
[zip_fopen_encrypted(3)](zip_fopen_encrypted.md),
[zip_fopen_index(3)](zip_fopen_index.md),
[zip_fopen_index_encrypted(3,)](zip_fopen_index_encrypted.md)
[zip_fread(3)](zip_fread.md), [zip_fseek(3)](zip_fseek.md),
[zip_ftell(3)](zip_ftell.md),
[zip_source_zip(3)](zip_source_zip.md),

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|             |     |
|-------------|-----|
| May 5, 2025 | NiH |
