# [NAME](#NAME)

`zip_register_progress_callback_with_state` — provide updates during
zip_close

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`typedef void (*zip_progress_callback)(zip_t *, double, void *);`

`void`  
`zip_register_progress_callback_with_state`(`zip_t *archive`,
`double precision`, `zip_progress_callback callback`,
`void (*ud_free)(void *)`, `void *ud`);

# [DESCRIPTION](#DESCRIPTION)

The
[`zip_register_progress_callback_with_state`](#zip_register_progress_callback_with_state)()
function registers a callback function `callback` for the zip archive
`archive`. The `precision` argument is a double in the range from 0.00
to 1.0 that defines the smallest change for which the callback should be
called (to avoid too frequent calls). The `ud_free` function is called
during cleanup for deleting the userdata supplied in `ud`.

The callback function is called during [zip_close(3)](zip_close.md) in
regular intervals (after every zip archive entry that's completely
written to disk, and while writing data for entries) with zip archive
`archive`, the current progression state as a `double`, and the
user-provided user-data `ud` as arguments. The progression state is a
`double` in the range from 0.0 to 1.0. This can be used to provide
progress indicators for user interfaces.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md),
[zip_register_cancel_callback_with_state(3)](zip_register_cancel_callback_with_state.md)

# [HISTORY](#HISTORY)

`zip_register_progress_callback_with_state`() was added in libzip 1.3.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                   |     |
|-------------------|-----|
| December 18, 2017 | NiH |
