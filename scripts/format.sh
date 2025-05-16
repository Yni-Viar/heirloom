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

cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ../winfile
find . -type f \( -iname \*.h -o -iname \*.cpp \) | xargs clang-format -i
