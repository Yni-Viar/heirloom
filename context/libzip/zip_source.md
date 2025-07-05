# [NAME](#NAME)

`zip_source` — zip data source structure

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

`zip_source_t *source`;

# [DESCRIPTION](#DESCRIPTION)

A `zip_source_t` represents a data source (or destination when used for
writing). [libzip(3)](libzip.md) uses it for adding or replacing file
contents for a file in a zip archive. If the source supports seeking, it
can also be used to open zip archives from.

Data can come from a file on disk
([zip_source_file(3)](zip_source_file.md),
[zip_source_file_create(3)](zip_source_file_create.md),
[zip_source_filep(3)](zip_source_filep.md), or
[zip_source_filep_create(3)](zip_source_filep_create.md)), memory
([zip_source_buffer(3)](zip_source_buffer.md),
[zip_source_buffer_create(3)](zip_source_buffer_create.md),
[zip_source_buffer_fragment(3)](zip_source_buffer_fragment.md), or
[zip_source_buffer_fragment_create(3)](zip_source_buffer_fragment_create.md)),
a file inside an archive ([zip_source_zip(3)](zip_source_zip.md)), or
provided via a callback function
([zip_source_function(3)](zip_source_function.md) or
[zip_source_function_create(3)](zip_source_function_create.md)).
[zip_source_window(3)](zip_source_window.md) or
[zip_source_window_create(3)](zip_source_window_create.md) can be used
restrict access to a part of the contained data.

It can also be used as a filter to process the data provided by an
underlying `zip_source_t` (e.g., to compress it or compute a checksum),
created with [zip_source_layered(3)](zip_source_layered.md) or
[zip_source_layered_create(3)](zip_source_layered_create.md)).

Sources are freed with [zip_source_free(3)](zip_source_free.md).

Sources must support reading, and can optionally support seeking and
writing.

`zip_source_t` is reference counted, and created with a reference count
of 1. [zip_open_from_source(3)](zip_open_from_source.md),
[zip_file_add(3)](zip_file_add.md), and
[zip_file_replace(3)](zip_file_replace.md) will take ownership of the
passed source (decrement the reference count when they are done using
it), so [zip_source_free(3)](zip_source_free.md) only needs to be
called when these functions return an error. The underlying data (file
or buffer) must remain valid until the archive is closed. Use
[zip_source_keep(3)](zip_source_keep.md) to increase the reference
count, for example if you need the source after
[zip_close(3)](zip_close.md).

# [SEE ALSO](#SEE_ALSO)

[libzip(3)](libzip.md),
[zip_source_buffer(3)](zip_source_buffer.md),
[zip_source_file(3)](zip_source_file.md),
[zip_source_filep(3)](zip_source_filep.md),
[zip_source_free(3)](zip_source_free.md),
[zip_source_function(3)](zip_source_function.md),
[zip_source_keep(3)](zip_source_keep.md),
[zip_source_window(3)](zip_source_window.md),
[zip_source_zip(3)](zip_source_zip.md)

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|             |     |
|-------------|-----|
| May 5, 2025 | NiH |
