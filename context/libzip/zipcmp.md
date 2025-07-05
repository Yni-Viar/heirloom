# [NAME](#NAME)

`zipcmp` — compare contents of zip archives

# [SYNOPSIS](#SYNOPSIS)

|          |                                       |
|----------|---------------------------------------|
| `zipcmp` | \[`-ChipqsTtVv`\] `archive1 archive2` |

# [DESCRIPTION](#DESCRIPTION)

`zipcmp` compares the zip archives or directories `archive1` and
`archive2` and checks if they contain the same files, comparing their
names, uncompressed sizes, and CRCs. File order and compressed size
differences are ignored.

Supported options:

[`-C`](#C)  
Check consistency of archives. Results in an error if archive is
inconsistent or not valid according to the zip specification.

[`-h`](#h)  
Display a short help message and exit.

[`-i`](#i)  
Compare names ignoring case distinctions.

[`-p`](#p)  
Enable paranoid checks. Compares extra fields, comments, and other meta
data. (Automatically disabled if one of the archives is a directory.)
These checks are skipped for files where the data differs.

[`-q`](#q)  
Quiet mode. Compare `-v`.

[`-s`](#s)  
Print a summary of how many files where added and removed.

[`-T`](#T)  
Additionally compare the time stamps of the entries.

[`-t`](#t)  
Test zip files by comparing the contents to their checksums.

[`-V`](#V)  
Display version information and exit.

[`-v`](#v)  
Verbose mode. Print details about differences to stdout. (This is the
default.)

# [EXIT STATUS](#EXIT_STATUS)

`zipcmp` exits 0 if the two archives contain the same files, 1 if they
differ, and \>1 if an error occurred.

# [SEE ALSO](#SEE_ALSO)

[zipmerge(1)](zipmerge.md), [ziptool(1)](ziptool.md),
[libzip(3)](libzip.md)

# [HISTORY](#HISTORY)

`zipcmp` was added in libzip 0.6.

# [AUTHORS](#AUTHORS)

Dieter Baron \<[dillo@nih.at](mailto:dillo@nih.at)\> and Thomas Klausner
\<[wiz@gatalith.at](mailto:wiz@gatalith.at)\>

|                |     |
|----------------|-----|
| March 15, 2024 | NiH |
