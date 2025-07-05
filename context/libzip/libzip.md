# [NAME](#NAME)

`libzip` — library for manipulating zip archives

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

# [DESCRIPTION](#DESCRIPTION)

`libzip` is a library for reading, creating, and modifying zip archives.

The main design criteria for `libzip` were:

- Maintain a stable API without breaking backwards compatibility.
- Do not create corrupt files, even in case of errors.
- Do not delete data.
- Be efficient.

For this reason, when modifying zip archives, `libzip` writes to a
temporary file and replaces the original zip archive atomically.

# [GENERAL NOTES](#GENERAL_NOTES)

When adding files to an archive, the file data is only read when the new
archive is written. Therefore all files added must remain valid until
the archive is closed with [zip_close(3)](zip_close.md) or
[zip_discard(3)](zip_discard.md).

Unless explicitly documented, functions should not be passed `NULL`
pointers as arguments.

# [DATA TYPES](#DATA_TYPES)

These data types correspond to central concepts in `libzip`. Most of
them are private, meaning you can't allocate them or access their
members directly. This allows extending the structures in the future
without breaking compatibility.

## [zip_t](#zip_t)

This type represents an opened archive. See [zip(5)](zip.md).

## [zip_file_t](#zip_file_t)

This type represents a file from an archive that has been opened for
reading. See [zip_file(5)](zip_file.md).

## [zip_source_t](#zip_source_t)

This type represents a source (or destination) of data. It is used in
`libzip` for providing data when adding or replacing files, accessing
data from a file inside an archive, and the data for the archive as a
whole. See [zip_source(5)](zip_source.md).

## [zip_error_t](#zip_error_t)

This type represents information about an error. Its type can be checked
against pre-defined constants and it can be converted to a human
readable string. See [zip_error(5)](zip_error.md).

# [FILE NAMES](#FILE_NAMES)

## [Encoding](#Encoding)

Names of files in the host file system are expected in UTF-8 encoding.
On Windows, variants for ASCII and UTF-16 are also available.

Names of files inside archives are by default expected in UTF-8
encoding. Other encodings can be requested by using the flags
`ZIP_FL_ENC_CP437` and `ZIP_FL_ENC_RAW`.

For details see the relevant man pages.

## [Directory Separator](#Directory_Separator)

The zip format requires the use of forward slash (‘/’) as directory
separator. Since backslash (‘\\) can be part of a valid file name on
Unix systems, `libzip` does not automatically convert them, even on
Windows. It is the responsibility of the programmer to ensure that all
directory separators are passed as forward slashes to `libzip`.

# [THREAD SAFETY](#THREAD_SAFETY)

In general, different zip archives opened by `libzip` are independent of
each other and can be used by parallel-running threads without locking.
If you want to use an archive from multiple threads, you have to
synchronize access to it yourself. If you use an archive as a source for
[zip_file_add(3)](zip_file_add.md) or
[zip_file_replace(3)](zip_file_replace.md), access to the target
archive must be synchronized with access to the source archive as well.

# [READING ZIP ARCHIVES](#READING_ZIP_ARCHIVES)

## [Open Archive](#Open_Archive)

- [zip_open(3)](zip_open.md)
- [zip_open_from_source(3)](zip_open_from_source.md)
- [zip_fdopen(3)](zip_fdopen.md)

## [Get Archive Attributes](#Get_Archive_Attributes)

- [zip_get_archive_comment(3)](zip_get_archive_comment.md)
- [zip_get_archive_flag(3)](zip_get_archive_flag.md)
- [zip_get_num_entries(3)](zip_get_num_entries.md)

## [Find Files](#Find_Files)

- [zip_name_locate(3)](zip_name_locate.md)

## [Read Files](#Read_Files)

- [zip_fopen(3)](zip_fopen.md)
- [zip_fopen_encrypted(3)](zip_fopen_encrypted.md)
- [zip_fopen_index(3)](zip_fopen_index.md)
- [zip_fopen_index_encrypted(3)](zip_fopen_index_encrypted.md)
- [zip_fread(3)](zip_fread.md)
- [zip_file_is_seekable(3)](zip_file_is_seekable.md)
- [zip_fseek(3)](zip_fseek.md) (uncompressed files only)
- [zip_ftell(3)](zip_ftell.md)
- [zip_fclose(3)](zip_fclose.md)

## [Close Archive](#Close_Archive)

- [zip_close(3)](zip_close.md)

## [Get File Attributes](#Get_File_Attributes)

- [zip_stat(3)](zip_stat.md)
- [zip_file_get_comment(3)](zip_file_get_comment.md)
- [zip_file_get_external_attributes(3)](zip_file_get_external_attributes.md)
- [zip_get_name(3)](zip_get_name.md)

## [Miscellaneous](#Miscellaneous)

- [zip_compression_method_supported(3)](zip_compression_method_supported.md)
- [zip_encryption_method_supported(3)](zip_encryption_method_supported.md)
- [zip_set_default_password(3)](zip_set_default_password.md)

# [CREATING/MODIFYING ZIP ARCHIVES](#CREATING/MODIFYING_ZIP_ARCHIVES)

## [Create/Open Archive](#Create/Open_Archive)

- [zip_open(3)](zip_open.md)

## [Add/Change Files and Directories](#Add/Change_Files_and_Directories)

- [zip_dir_add(3)](zip_dir_add.md)
- [zip_file_add(3)](zip_file_add.md)
- [zip_file_replace(3)](zip_file_replace.md)
- [zip_file_set_comment(3)](zip_file_set_comment.md)
- [zip_file_set_dostime(3)](zip_file_set_dostime.md)
- [zip_file_set_external_attributes(3)](zip_file_set_external_attributes.md)
- [zip_file_set_encryption(3)](zip_file_set_encryption.md)
- [zip_file_set_mtime(3)](zip_file_set_mtime.md)
- [zip_set_file_compression(3)](zip_set_file_compression.md)
- [zip_source_buffer(3)](zip_source_buffer.md)
- [zip_source_file(3)](zip_source_file.md)
- [zip_source_filep(3)](zip_source_filep.md)
- [zip_source_zip(3)](zip_source_zip.md)

## [Rename Files](#Rename_Files)

- [zip_rename(3)](zip_rename.md)

## [Delete Files](#Delete_Files)

- [zip_delete(3)](zip_delete.md)

## [Revert Changes](#Revert_Changes)

- [zip_unchange(3)](zip_unchange.md)
- [zip_unchange_all(3)](zip_unchange_all.md)
- [zip_unchange_archive(3)](zip_unchange_archive.md)

## [Read/Modify Extra Fields](#Read/Modify_Extra_Fields)

- [zip_file_extra_field_delete(3)](zip_file_extra_field_delete.md)
- [zip_file_extra_field_delete_by_id(3)](zip_file_extra_field_delete_by_id.md)
- [zip_file_extra_field_get(3)](zip_file_extra_field_get.md)
- [zip_file_extra_field_get_by_id(3)](zip_file_extra_field_get_by_id.md)
- [zip_file_extra_field_set(3)](zip_file_extra_field_set.md)
- [zip_file_extra_fields_count(3)](zip_file_extra_fields_count.md)
- [zip_file_extra_fields_count_by_id(3)](zip_file_extra_fields_count_by_id.md)

## [Close Archive (Writing)](#Close_Archive_(Writing))

- [zip_close(3)](zip_close.md)
- [zip_discard(3)](zip_discard.md)

## [Miscellaneous (Writing)](#Miscellaneous_(Writing))

- [zip_file_attributes_init(3)](zip_file_attributes_init.md)
- [zip_libzip_version(3)](zip_libzip_version.md)
- [zip_register_cancel_callback_with_state(3)](zip_register_cancel_callback_with_state.md)
- [zip_register_progress_callback_with_state(3)](zip_register_progress_callback_with_state.md)
- [zip_set_archive_comment(3)](zip_set_archive_comment.md)
- [zip_set_archive_flag(3)](zip_set_archive_flag.md)
- [zip_source(5)](zip_source.md)

# [SOURCES](#SOURCES)

## [Create Source](#Create_Source)

- [zip_source_buffer(3)](zip_source_buffer.md)
- [zip_source_file(3)](zip_source_file.md)
- [zip_source_filep(3)](zip_source_filep.md)
- [zip_source_function(3)](zip_source_function.md)
- [zip_source_layered(3)](zip_source_layered.md)
- [zip_source_zip(3)](zip_source_zip.md)

## [Using Source](#Using_Source)

- [zip_file_add(3)](zip_file_add.md)
- [zip_file_replace(3)](zip_file_replace.md)
- [zip_open_from_source(3)](zip_open_from_source.md)

## [Implementing Source](#Implementing_Source)

- [zip_source_pass_to_lower_layer(3)](zip_source_pass_to_lower_layer.md)

## [Source Life Cycle](#Source_Life_Cycle)

- [zip_source_free(3)](zip_source_free.md)
- [zip_source_keep(3)](zip_source_keep.md)

# [ERROR HANDLING](#ERROR_HANDLING)

- [zip_error_strerror(3)](zip_error_strerror.md)
- [zip_strerror(3)](zip_strerror.md)
- [zip_file_strerror(3)](zip_file_strerror.md)
- [zip_file_get_error(3)](zip_file_get_error.md)
- [zip_get_error(3)](zip_get_error.md)
- [zip_error_init_with_code(3)](zip_error_init_with_code.md)
- [zip_error_set(3)](zip_error_set.md)
- [zip_error_set_from_source(3)](zip_error_set_from_source.md)
- [zip_error_system_type(3)](zip_error_system_type.md)
- [zip_errors(3)](zip_errors.md)

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|             |     |
|-------------|-----|
| May 5, 2025 | NiH |
