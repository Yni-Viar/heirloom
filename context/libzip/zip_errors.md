# [NAME](#NAME)

`zip_errors` — list of all libzip error codes

# [LIBRARY](#LIBRARY)

libzip (-lzip)

# [SYNOPSIS](#SYNOPSIS)

`#include <``zip.h``>`

# [DESCRIPTION](#DESCRIPTION)

The following error codes are used by libzip:

\[`ZIP_ER_CANCELLED`\]  
Operation cancelled.

\[`ZIP_ER_CHANGED`\]  
Entry has been changed.

\[`ZIP_ER_CLOSE`\]  
Closing zip archive failed.

\[`ZIP_ER_COMPNOTSUPP`\]  
Compression method not supported.

\[`ZIP_ER_COMPRESSED_DATA`\]  
Compressed data invalid.

\[`ZIP_ER_CRC`\]  
CRC error.

\[`ZIP_ER_DATA_LENGTH`\]  
Unexpected length of data.

\[`ZIP_ER_DELETED`\]  
Entry has been deleted.

\[`ZIP_ER_ENCRNOTSUPP`\]  
Encryption method not supported.

\[`ZIP_ER_EOF`\]  
Premature end of file.

\[`ZIP_ER_EXISTS`\]  
File already exists.

\[`ZIP_ER_INCONS`\]  
Zip archive inconsistent.

\[`ZIP_ER_INTERNAL`\]  
Internal error.

\[`ZIP_ER_INUSE`\]  
Resource still in use.

\[`ZIP_ER_INVAL`\]  
Invalid argument.

\[`ZIP_ER_MEMORY`\]  
Malloc failure.

\[`ZIP_ER_MULTIDISK`\]  
Multi-disk zip archives not supported.

\[`ZIP_ER_NOENT`\]  
No such file.

\[`ZIP_ER_NOPASSWD`\]  
No password provided.

\[`ZIP_ER_NOT_ALLOWED`\]  
Not allowed in torrentzip.

\[`ZIP_ER_NOZIP`\]  
Not a zip archive.

\[`ZIP_ER_OK`\]  
No error.

\[`ZIP_ER_OPEN`\]  
Can't open file.

\[`ZIP_ER_OPNOTSUPP`\]  
Operation not supported.

\[`ZIP_ER_RDONLY`\]  
Read-only archive.

\[`ZIP_ER_READ`\]  
Read error.

\[`ZIP_ER_REMOVE`\]  
Can't remove file.

\[`ZIP_ER_RENAME`\]  
Renaming temporary file failed.

\[`ZIP_ER_SEEK`\]  
Seek error.

\[`ZIP_ER_TELL`\]  
Tell error.

\[`ZIP_ER_TMPOPEN`\]  
Failure to create temporary file.

\[`ZIP_ER_TRUNCATED_ZIP`\]  
Possibly truncated or corrupted zip archive.

\[`ZIP_ER_WRITE`\]  
Write error.

\[`ZIP_ER_WRONGPASSWD`\]  
Wrong password provided.

\[`ZIP_ER_ZIPCLOSED`\]  
Containing zip archive was closed.

\[`ZIP_ER_ZLIB`\]  
Zlib error.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 15, 2024 | NiH |
