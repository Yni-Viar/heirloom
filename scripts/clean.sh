#!/bin/bash
set -eo pipefail

# Change to src directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ../src

rm -rf \
    ../.vscode \
    ARM64 \
    x64 \
    libprogman_tests/*.vcxproj.user \
    libprogman_tests/ARM64 \
    libprogman_tests/x64 \
    libprogman/*.vcxproj.user \
    libprogman/ARM64 \
    libprogman/x64 \
    progman/*.vcxproj.user \
    progman/ARM64 \
    progman/x64 \
    winfile/*.vcxproj.user \
    winfile/ARM64 \
    winfile/x64 \
    ;
