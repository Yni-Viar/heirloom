#!/bin/bash
set -euo pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"
SOLUTION=progman.sln ./build-solution.sh
PROJECT=libprogman_tests ./test-project.sh
