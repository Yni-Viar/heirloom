#!/bin/bash
set -euo pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"
PLATFORM=$(./get-native-arch.sh)
"./$PLATFORM/Debug/progman.exe"
