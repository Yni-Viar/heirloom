# [NAME](#NAME)

`zip_file_set_dostime`, `zip_file_set_mtime` — set last modification
time (mtime) for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_file_set_dostime`(`zip_t *archive`, `zip_uint64_t index`,
`zip_uint16_t dostime`, `zip_uint16_t dosdate`, `zip_flags_t flags`);

`int`  
`zip_file_set_mtime`(`zip_t *archive`, `zip_uint64_t index`,
`time_t mtime`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_set_mtime`](#zip_file_set_mtime)() function sets the last
modification time (mtime) for the file at position `index` in the zip
archive to `mtime`. Currently, no support for any `flags` is
implemented.

In the zip archive, the time and date are saved as two 16-bit integers.
To set the values directly, call the
[`zip_file_set_dostime`](#zip_file_set_dostime)() function. The values
of the time bytes are defined as follows:

0-4  
seconds divided by two (1-2 = 1, 3-4 = 2, ...)

5-10  
minute (0-59)

11-15  
hour (0-23)

The values of the date bytes are defined as follows:

0-4  
day of the month (1-31)

5-8  
month (January = 1, February = 2, ...)

9-15  
year offset from 1980 (1980 = 0, 1981 = 1, ...)

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_file_set_dostime`() and `zip_file_set_mtime`() fail if:

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_OPNOTSUPP`](#ZIP_ER_OPNOTSUPP)\]  
Traditional PKWare encryption uses the file's mtime, therefore it cannot
be changed without re-encrypting the data.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
The `archive` was opened in read-only mode.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_stat(3)](zip_stat.md)

# [HISTORY](#HISTORY)

`zip_file_set_mtime`() was added in libzip 1.0. `zip_file_set_dostime`()
was added in libzip 1.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

# [CAVEATS](#CAVEATS)

Following historical practice, the `zip_file_set_mtime`() function
translates the time from the zip archive into the local time zone. If
you want to avoid this, use the `zip_file_set_dostime`() function
instead.

|               |     |
|---------------|-----|
| June 18, 2022 | NiH |
