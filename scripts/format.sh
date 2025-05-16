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

# Change to repository root directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ..

# Format a project's source files
format_project() {
    local project_name=$1
    echo "Formatting $project_name..."
    find $project_name -type f \( -iname \*.h -o -iname \*.cpp \) | xargs clang-format -i
}

format_project "winfile"
format_project "progman"
