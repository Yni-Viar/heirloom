#!/bin/bash
set -eo pipefail

# Change to src directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"

rm -rf \
    ../.vscode \
    .vs \
    ARM64 \
    x64 \
    libprogman_tests/.vs \
    libprogman_tests/*.vcxproj.user \
    libprogman_tests/ARM64 \
    libprogman_tests/x64 \
    libprogman/.vs \
    libprogman/*.vcxproj.user \
    libprogman/ARM64 \
    libprogman/x64 \
    progman/.vs \
    progman/*.vcxproj.user \
    progman/ARM64 \
    progman/x64 \
    winfile/.vs \
    winfile/*.vcxproj.user \
    winfile/ARM64 \
    winfile/x64 \
    ;
