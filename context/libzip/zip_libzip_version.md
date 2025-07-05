# [NAME](#NAME)

`zip_libzip_version` — return run-time version of library

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`const char *`  
`zip_libzip_version`(`void`);

# [DESCRIPTION](#DESCRIPTION)

`zip_libzip_version` returns the version number of the library as string
in the format “\$MAJOR.\$MINOR.\$MICRO\$SUFFIX” where `$MAJOR` is the
major version, `$MINOR` the minor, `$MICRO` the micro, and `$SUFFIX` a
suffix that's only set for development versions.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md)

# [HISTORY](#HISTORY)

`zip_libzip_version`() was added in libzip 1.3.1.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
