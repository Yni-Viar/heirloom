# [NAME](#NAME)

`zip_register_progress_callback` — provide updates during zip_close
(obsolete interface)

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`typedef void (*zip_progress_callback_t)(double);`

`void`  
`zip_register_progress_callback`(`zip_t *archive`,
`zip_progress_callback_t progress_callback`);

# [DESCRIPTION](#DESCRIPTION)

The function
[`zip_register_progress_callback`](#zip_register_progress_callback)() is
the obsolete version of
[zip_register_progress_callback_with_state(3)](zip_register_progress_callback_with_state.md).

The
[`zip_register_progress_callback`](#zip_register_progress_callback~2)()
function registers a callback function `progress_callback` for the zip
archive `archive`. This function is called during
[zip_close(3)](zip_close.md) after every zip archive entry that's
completely written to disk. The value is a `double` in the range from
0.0 to 1.0. This can be used to provide progress indicators for user
interfaces.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md)

# [HISTORY](#HISTORY)

`zip_register_progress_callback`() was added in libzip 1.2.0. It was
deprecated in libzip 1.3.0, use
`zip_register_progress_callback_with_state`() instead.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
