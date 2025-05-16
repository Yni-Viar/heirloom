#!/bin/bash
set -eo pipefail

# Change to src directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"

rm -rf \
    ARM64 \
    x64 \
    libprogman/ARM64 \
    libprogman/x64 \
    progman/ARM64 \
    progman/x64 \
    tests/ARM64 \
    tests/x64 \
    winfile/ARM64 \
    winfile/x64 \
    ;
