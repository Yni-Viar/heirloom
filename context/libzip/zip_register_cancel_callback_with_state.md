# [NAME](#NAME)

`zip_register_cancel_callback_with_state` — allow cancelling during
zip_close

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`typedef int (*zip_cancel_callback)(zip_t *, void *);`

`void`  
`zip_register_cancel_callback_with_state`(`zip_t *archive`,
`zip_cancel_callback callback`, `void (*ud_free)(void *)`, `void *ud`);

# [DESCRIPTION](#DESCRIPTION)

This function can be used to cancel writing of a zip archive during
[zip_close(3)](zip_close.md).

The
[`zip_register_cancel_callback_with_state`](#zip_register_cancel_callback_with_state)()
function registers a callback function `callback` for the zip archive
`archive`. The `ud_free` function is called during cleanup for deleting
the userdata supplied in `ud`.

The callback function is called during [zip_close(3)](zip_close.md) in
regular intervals (after every zip archive entry that's completely
written to disk, and while writing data for entries) with zip archive
`archive` and the user-provided user-data `ud` as arguments. When the
callback function returns a non-zero value, writing is cancelled and
[zip_close(3)](zip_close.md) returns an error.

The callback function should be fast, since it will be called often.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md),
[zip_register_progress_callback_with_state(3)](zip_register_progress_callback_with_state.md)

# [HISTORY](#HISTORY)

`zip_register_cancel_callback_with_state`() was added in libzip 1.6.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|               |     |
|---------------|-----|
| June 18, 2022 | NiH |
