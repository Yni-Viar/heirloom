# [NAME](#NAME)

`zip_file_attributes_init` — initialize zip file attributes structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`void`  
`zip_file_attributes_init`(`zip_file_attributes_t *attributes`);

# [DESCRIPTION](#DESCRIPTION)

The [`zip_file_attributes_init`](#zip_file_attributes_init)()
initializes a `zip_file_attributes_t` structure with default values. It
must be called before modifying such a structure for the first time.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_source_function(3)](zip_source_function.md)

# [HISTORY](#HISTORY)

`zip_file_attributes_init`() was added in libzip 1.7.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| April 17, 2020 | NiH |
