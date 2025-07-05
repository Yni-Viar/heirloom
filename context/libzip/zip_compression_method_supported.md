# [NAME](#NAME)

`zip_compression_method_supported` — return if a compression method is
supported

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_compression_method_supported`(`zip_int32_t method`,
`int compress`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_compression_method_supported`](#zip_compression_method_supported)()
returns if the compression method `method` is supported for compression
(if `compress` is zero) or decompression (otherwise).

# [RETURN VALUES](#RETURN_VALUES)

Returns 1 if the method is supported, 0 otherwise.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_encryption_method_supported(3)](zip_encryption_method_supported.md),
[zip_set_file_compression(3)](zip_set_file_compression.md)

# [HISTORY](#HISTORY)

`zip_compression_method_supported`() was added in libzip 1.7.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| April 2, 2020 | NiH |
