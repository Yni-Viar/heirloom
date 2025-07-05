# [NAME](#NAME)

`zip_source_file`, `zip_source_file_create` — create data source from a
file

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *`  
`zip_source_file`(`zip_t *archive`, `const char *fname`,
`zip_uint64_t start`, `zip_int64_t len`);

`zip_source_t *`  
`zip_source_file_create`(`const char *fname`, `zip_uint64_t start`,
`zip_int64_t len`, `zip_error_t *error`);

# [DESCRIPTION](#DESCRIPTION)

The functions [`zip_source_file`](#zip_source_file)() and
[`zip_source_file_create`](#zip_source_file_create)() create a zip
source from a file. They open `fname` and read `len` bytes from offset
`start` from it.

When passing `ZIP_LENGTH_TO_END` (or -1, which is deprecated) as `len`,
`zip_source_file` determines the file size when it is called and uses
that as the expected file size. If the file size grows between creating
and reading from the source, the additional data is ignored. If the file
shrinks, `libzip` treats it as an error (`ZIP_ER_DATA_LENGTH`).

When passing `ZIP_LENGTH_UNCHECKED` as `len`, `zip_source_file` assumes
the file's size is unknown. Reading from the source returns as much data
as is there at that time (usually when calling
[zip_close(3)](zip_close.md)).

`libzip` can do various optimizations if the size of a source is known
when it's created, so `ZIP_LENGTH_TO_END` is preferable. If you deal
with files that are likely to change while you are processing them, you
can use the less efficient `ZIP_LENGTH_UNCHECKED`.

If the file supports seek, the source can be used to open a zip archive
from.

The file is opened and read when the data from the source is used,
usually by [`zip_close`](#zip_close)() or
[`zip_open_from_source`](#zip_open_from_source)().

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, the created source is returned. Otherwise,
`NULL` is returned and the error code in `archive` or `error` is set to
indicate the error.

# [ERRORS](#ERRORS)

`zip_source_file`() and `zip_source_file_create`() fail if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`fname`, `start`, or `len` are invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_OPEN`](#ZIP_ER_OPEN)\]  
Opening `fname` failed.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_file_add(3)](zip_file_add.md),
[zip_file_replace(3)](zip_file_replace.md),
[zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_source_file`() and `zip_source_file_create`() were added in libzip
1.0.

`ZIP_LENGTH_TO_END` and `ZIP_LENGTH_UNCHECKED` were added in libzip
1.10.1.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| June 30, 2023 | NiH |
