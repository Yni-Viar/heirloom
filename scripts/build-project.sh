#!/bin/bash
set -euo pipefail

# Check if project name is provided as first argument
if [ $# -eq 0 ]; then
    echo "Error: Project name must be provided as the first argument"
    echo "Usage: $0 <project_name>"
    exit 1
fi

PROJECT_NAME="$1"
echo "Building $PROJECT_NAME..."

# Change to the project directory.
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd "../$PROJECT_NAME"

# Use vswhere to locate msbuild.exe
vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
if [ ! -f "$vswhere" ]; then
    echo "Could not find vswhere.exe!"
    exit 1
fi

MSBUILD=$("$vswhere" -latest -requires Microsoft.Component.MSBuild -find "MSBuild/**/Bin/MSBuild.exe" | head -n 1)
if [ ! -f "$MSBUILD" ]; then
    echo "Could not find msbuild.exe!"
    exit 1
fi

# Detect the native architecture: x64 or ARM64?
# 
# Don't use $PROCESSOR_ARCHITECTURE directly; that reports the architecture of bash.exe which is always x64 even in the
# supposedly arm64 build of Git for Windows.
#
# Instead, invoke PowerShell and check it there, because powershell.exe is always the correct architecture.
ARCH=$(powershell.exe -NoProfile -Command "[System.Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE', 'Machine')")
case "$ARCH" in
    "ARM64") PLATFORM="ARM64" ;;
    *) PLATFORM="x64" ;;  # Default to x64 for all other values
esac

# Clean the project
rm -rf Win32 x64 ARM64

# Build the project
set +e
"$MSBUILD" "$PROJECT_NAME.vcxproj" --p:Configuration=Debug --p:Platform=$PLATFORM --verbosity:quiet --nologo 2>&1
MSBUILD_EXIT_CODE=$?

if [ $MSBUILD_EXIT_CODE -ne 0 ]; then
    echo "Build failed!"
    exit $MSBUILD_EXIT_CODE
fi

echo "Build succeeded!"
exit 0
