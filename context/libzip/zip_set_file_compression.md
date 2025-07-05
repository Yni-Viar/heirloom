# [NAME](#NAME)

`zip_set_file_compression` — set compression method for file in zip

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_set_file_compression`(`zip_t *archive`, `zip_uint64_t index`,
`zip_int32_t comp`, `zip_uint32_t comp_flags`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_set_file_compression`](#zip_set_file_compression)() function
sets the compression method for the file at position `index` in the zip
archive to `comp` with the compression method specific `comp_flags`. The
`comp` is the same as returned by [zip_stat(3)](zip_stat.md). For the
`comp` argument, currently only the following values are supported:

[`ZIP_CM_DEFAULT`](#ZIP_CM_DEFAULT)  
default compression; currently the same as `ZIP_CM_DEFLATE`, but `flags`
are ignored.

[`ZIP_CM_STORE`](#ZIP_CM_STORE)  
Store the file uncompressed.

[`ZIP_CM_BZIP2`](#ZIP_CM_BZIP2)  
Compress the file using the
[bzip2(1)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/bzip2.md)
algorithm.

[`ZIP_CM_DEFLATE`](#ZIP_CM_DEFLATE)  
Deflate the file with the
[zlib(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/zlib.md)
algorithm and default options.

[`ZIP_CM_XZ`](#ZIP_CM_XZ)  
Use the
[xz(1)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/xz.md)
algorithm for compression

[`ZIP_CM_ZSTD`](#ZIP_CM_ZSTD)  
Use the
[zstd(1)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/zstd.md)
algorithm for compression

[*NOTE*](#NOTE): Only the deflate and store methods can be assumed to be
universally supported.

The `comp_flags` argument defines the compression level. This value is
dependent on the compression algorithm. In general, lower numbers mean
faster de/compression and higher numbers mean slower de/compression. For
`ZIP_CM_BZIP`, `ZIP_CM_DEFLATE`, and `ZIP_CM_XZ` 1 is the fastest
compression and 9 the best, 0 chooses the default. For `ZIP_CM_ZSTD`
possible values are
[ZSTD_minCLevel(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/ZSTD_minCLevel.md)
to
[ZSTD_maxCLevel(3)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/ZSTD_maxCLevel.md);
negative values must be cast to `zip_uint32_t`.

Further compression method specific flags might be added over time.

The current compression method for a file in a zip archive can be
determined using [zip_stat(3)](zip_stat.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion 0 is returned. Otherwise, -1 is returned and
the error information in `archive` is set to indicate the error.

# [ERRORS](#ERRORS)

`zip_set_file_compression`() fails if:

\[[`ZIP_ER_COMPNOTSUPP`](#ZIP_ER_COMPNOTSUPP)\]  
Unsupported compression method requested.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`index` is not a valid file index in `archive`, or the argument
combination is invalid.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
Read-only zip file, no changes allowed.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_compression_method_supported(3)](zip_compression_method_supported.md),
[zip_stat(3)](zip_stat.md)

# [HISTORY](#HISTORY)

`zip_set_file_compression`() was added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                  |     |
|------------------|-----|
| February 2, 2023 | NiH |
