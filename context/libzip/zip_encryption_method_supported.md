# [NAME](#NAME)

`zip_encryption_method_supported` — return if an encryption method is
supported

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`int`  
`zip_encryption_method_supported`(`zip_int16_t method`, `int encrypt`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_encryption_method_supported`](#zip_encryption_method_supported)()
returns if the encryption method `method` is supported for encryption
(if `encrypt` is zero) or decryption (otherwise).

# [RETURN VALUES](#RETURN_VALUES)

Returns 1 if the method is supported, 0 otherwise.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_compression_method_supported(3)](zip_compression_method_supported.md),
[zip_file_set_encryption(3)](zip_file_set_encryption.md)

# [HISTORY](#HISTORY)

`zip_encryption_method_supported`() was added in libzip 1.7.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| April 2, 2020 | NiH |
