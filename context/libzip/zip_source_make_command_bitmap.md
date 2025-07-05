# [NAME](#NAME)

`zip_source_make_command_bitmap` — create bitmap of supported source
operations

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_source_make_command_bitmap`(`zip_source_cmd_t command`, `...`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_source_make_command_bitmap`](#zip_source_make_command_bitmap)()
function returns a bitmap of source commands suitable as return value
for `ZIP_SOURCE_SUPPORTS`. It includes all the commands from the
argument list, which must be terminated by -1.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_source_function(3)](zip_source_function.md)

# [HISTORY](#HISTORY)

`zip_source_make_command_bitmap`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
