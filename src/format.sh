#!/bin/bash
set -eo pipefail

# Check for clang-format.
if ! command -v clang-format &> /dev/null; then
    echo "clang-format could not be found."
    echo "Use Visual Studio Installer to install the clang feature."
    echo 'Open Developer Command Prompt and run "C:\Program Files\Git\git-bash.exe".'
    echo "Then, run this script again."
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
    echo "ERROR: The following files are not UTF-8 encoded:"
    for file in "${non_utf8_files[@]}"; do
        echo "  $file"
    done
    echo "Please convert these files to UTF-8 encoding before proceeding."
    exit 1
fi

# Change to src directory.
cd src

# Format a project's source files
format_project() {
    local project_name=$1
    echo "Formatting $project_name..."
    find $project_name -type f \( -iname \*.h -o -iname \*.cpp \) | xargs clang-format -i
}

format_project "winfile"
format_project "libprogman"
format_project "libprogman_tests"
format_project "progman"
