# [NAME](#NAME)

`zip_fdopen` — open zip archive using open file descriptor

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_t *`  
`zip_fdopen`(`int fd`, `int flags`, `int *errorp`);

# [DESCRIPTION](#DESCRIPTION)

The zip archive specified by the open file descriptor `fd` is opened and
a pointer to a `struct zip`, used to manipulate the archive, is
returned. In contrast to [zip_open(3)](zip_open.md), using
`zip_fdopen` the archive can only be opened in read-only mode. The `fd`
argument may not be used any longer after calling `zip_fdopen`. The
`flags` are specified by [*or*](#or)'ing the following values, or 0 for
none of them.

[`ZIP_CHECKCONS`](#ZIP_CHECKCONS)  
Perform additional stricter consistency checks on the archive, and error
if they fail.

If an error occurs and `errorp` is non-`NULL`, it will be set to the
corresponding error code.

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion `zip_fdopen`() returns a `struct zip`
pointer, and `fd` should not be used any longer, nor passed to
[close(2)](http://pubs.opengroup.org/onlinepubs/9699919799/functions/close.md).
Otherwise, `NULL` is returned and `*errorp` is set to indicate the
error. In the error case, `fd` remains unchanged.

# [ERRORS](#ERRORS)

The file specified by `fd` is prepared for use by
[libzip(3)](libzip.md) unless:

\[[`ZIP_ER_INCONS`](#ZIP_ER_INCONS)\]  
Inconsistencies were found in the file specified by `path`. This error
is often caused by specifying `ZIP_CHECKCONS` but can also happen
without it.

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
The `flags` argument is invalid. Not all [zip_open(3)](zip_open.md)
flags are allowed for `zip_fdopen`, see [DESCRIPTION](#DESCRIPTION).

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_NOZIP`](#ZIP_ER_NOZIP)\]  
The file specified by `fd` is not a zip archive.

\[[`ZIP_ER_OPEN`](#ZIP_ER_OPEN)\]  
The file specified by `fd` could not be prepared for use by
[libzip(3)](libzip.md).

\[[`ZIP_ER_OPNOTSUPP`](#ZIP_ER_OPNOTSUPP)\]  
This functionality has been disabled at compile time.

\[[`ZIP_ER_READ`](#ZIP_ER_READ)\]  
A read error occurred; see `errno` for details.

\[[`ZIP_ER_SEEK`](#ZIP_ER_SEEK)\]  
The file specified by `fd` does not allow seeks.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_close(3)](zip_close.md),
[zip_error_strerror(3)](zip_error_strerror.md),
[zip_open(3)](zip_open.md)

# [HISTORY](#HISTORY)

`zip_fdopen`() was added in libzip 1.0.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                    |     |
|--------------------|-----|
| September 23, 2022 | NiH |
