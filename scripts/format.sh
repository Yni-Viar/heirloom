#!/bin/bash
set -eo pipefail

VSWHERE="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
if [ ! -f "$VSWHERE" ]; then
    echo "Could not find vswhere.exe!"
    exit 1
fi

VSPATH=$("$VSWHERE" -all -property installationPath)
CLANG_FORMAT="$VSPATH\\VC\\Tools\\Llvm\\bin\\clang-format.exe"
if [ ! -f "$CLANG_FORMAT" ]; then
    echo "Could not find clang-format.exe!"
    echo "Use Visual Studio Installer to install the clang feature."
    exit 1
fi

# Change to root directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ..

# Check for non-UTF-8 text files.
# It can be checked with: iconv -s -f UTF-8 $FILENAME > /dev/null
# This will return a non-zero exit code if the file is not UTF-8.
non_utf8_files=()
while IFS= read -r file; do
    if ! iconv -s -f UTF-8 "$file" > /dev/null 2>&1; then
        non_utf8_files+=("$file")
    fi
done < <(find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.rc" -o -name "*.txt" -o -name "*.md" -o -name "*.mdc" -o -name "*.json" -o -name "*.vcxproj" -o -name "*.manifest" -o -name "*.sh" \) \
    -not -path "*.git*" -not -path "*x64*" -not -path "*ARM64*")

if [ ${#non_utf8_files[@]} -gt 0 ]; then
    echo "Converting the following files to UTF-8 encoding:"
    for file in "${non_utf8_files[@]}"; do
        echo "  $file"
        # Create a temporary file with UTF-8 encoding
        iconv -f "$(file -bi "$file" | sed 's/.*charset=//')" -t UTF-8 "$file" > "${file}.tmp"
        # Replace original file with UTF-8 version
        mv "${file}.tmp" "$file"
    done
    echo "Conversion complete."
fi

# Change to src directory.
cd src

# Format a project's source files
format_project() {
    local project_name=$1
    echo "Formatting $project_name..."
    find $project_name -type f \( -iname \*.h -o -iname \*.cpp \) | xargs "$CLANG_FORMAT" -i
}

format_project "winfile"
format_project "libprogman"
format_project "libprogman_tests"
format_project "progman"
