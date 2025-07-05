# [NAME](#NAME)

`zip_file_add`, `zip_file_replace` — add file to zip archive or replace
file in zip archive

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_int64_t`  
`zip_file_add`(`zip_t *archive`, `const char *name`,
`zip_source_t *source`, `zip_flags_t flags`);

`int`  
`zip_file_replace`(`zip_t *archive`, `zip_uint64_t index`,
`zip_source_t *source`, `zip_flags_t flags`);

# [DESCRIPTION](#DESCRIPTION)

The function [`zip_file_add`](#zip_file_add)() adds a file to a zip
archive, while [`zip_file_replace`](#zip_file_replace)() replaces an
existing file in a zip archive. The argument `archive` specifies the zip
archive to which the file should be added. `name` is the file's name in
the zip archive (for `zip_file_add`()), while `index` specifies which
file should be replaced (for `zip_file_replace`()). The `flags` argument
can be any combination of `ZIP_FL_OVERWRITE` with one of `ZIP_FL_ENC_*`:

[`ZIP_FL_OVERWRITE`](#ZIP_FL_OVERWRITE)  
Overwrite any existing file of the same name. For `zip_file_add` only.

[`ZIP_FL_ENC_GUESS`](#ZIP_FL_ENC_GUESS)  
Guess encoding of `name` (default). (Only CP-437 and UTF-8 are
recognized.)

[`ZIP_FL_ENC_UTF_8`](#ZIP_FL_ENC_UTF_8)  
Interpret `name` as UTF-8.

[`ZIP_FL_ENC_CP437`](#ZIP_FL_ENC_CP437)  
Interpret `name` as code page 437 (CP-437).

The data is obtained from the `source` argument, see
[zip_source(5)](zip_source.md).

[*NOTE*](#NOTE): [zip_source_free(3)](zip_source_free.md) should not
be called on a `source` after it was used successfully in a
`zip_file_add` or `zip_file_replace` call.

Please also note that when using `zip_replace`, the target file's extra
field information will be deleted since this usually is dependent on the
file contents. If you want to keep them, query them beforehand with
[zip_file_extra_field_get(3)](zip_file_extra_field_get.md) and restore
them after `zip_replace` with
[zip_file_extra_field_set(3)](zip_file_extra_field_set.md).

# [RETURN VALUES](#RETURN_VALUES)

Upon successful completion, `zip_file_add`() returns the index of the
new file in the archive, and `zip_file_replace`() returns 0. Otherwise,
-1 is returned and the error code in `archive` is set to indicate the
error.

# [EXAMPLES](#EXAMPLES)

    zip_source_t *s;
    const char buf[]="teststring";

    if ((s=zip_source_buffer(archive, buf, sizeof(buf), 0)) == NULL ||
        zip_file_add(archive, name, s, ZIP_FL_ENC_UTF_8) < 0) {
        zip_source_free(s);
        printf("error adding file: %s\n", zip_strerror(archive));
    }

# [ERRORS](#ERRORS)

`zip_file_add`() and `zip_file_replace`() fail if:

\[[`ZIP_ER_EXISTS`](#ZIP_ER_EXISTS)\]  
There is already a file called `name` in the archive. (Only applies to
`zip_file_add`(), and only if `ZIP_FL_OVERWRITE` is not provided).

\[[`ZIP_ER_INVAL`](#ZIP_ER_INVAL)\]  
`source` or `name` are `NULL`, or `index` is invalid.

\[[`ZIP_ER_MEMORY`](#ZIP_ER_MEMORY)\]  
Required memory could not be allocated.

\[[`ZIP_ER_RDONLY`](#ZIP_ER_RDONLY)\]  
Archive was opened in read-only mode.

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md), [zip_source(5)](zip_source.md)

# [HISTORY](#HISTORY)

`zip_file_add`() and `zip_file_replace`() were added in libzip 0.11.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 18, 2024 | NiH |
