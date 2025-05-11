#!/bin/bash
set -eo pipefail

# Change to the root directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ..

PAGER=cat git diff --cached
