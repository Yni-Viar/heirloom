#!/bin/bash
set -euo pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"
PLATFORM=$(./get-native-arch.sh)
"../src/$PLATFORM/Debug/progman.exe"
