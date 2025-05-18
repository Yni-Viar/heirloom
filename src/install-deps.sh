#!/bin/bash
# Set DOWNLOAD_ONLY to skip installation.
# Set USE_MIRROR to use the heirloom-archive.com mirror.
set -euo pipefail

function verify_sha256() {
    local file="$1"
    local sha256="$2"
    local actual=$(sha256sum "$file" | cut -d' ' -f1)
    if [ "$actual" != "$sha256" ]; then
        echo "Hash mismatch for $file"
        echo "Expected: $sha256"
        echo "Actual:   $actual"
        exit 1
    fi

    echo "Hash verified for $file ($sha256)"
}

# Setup
cd "$( dirname "${BASH_SOURCE[0]}" )"
rm -rf tmp
mkdir -p tmp downloads
cd tmp

# ---------------------------------------------------------------------------------------------------------------------
echo "--- Windows Implementation Library (WIL) ---"
WIL_VERSION="158863d89f6b462c60fb56b65ae4db6afee98adf"
WIL_SHA256="fae2ddc25832fcea0face8751a0561a5a4bf09ba8a31ddafdecbfe32972b2163"

if [ ! -f "../downloads/wil.zip" ]; then
    if [ -z "${USE_MIRROR+x}" ]; then
        WIL_URL="https://github.com/microsoft/wil/archive/$WIL_VERSION.zip"
    else
        WIL_URL="https://heirloom-archive.com/deps/wil/wil-$WIL_VERSION.zip"
    fi
    echo "Downloading: $WIL_URL"
    curl -Lo ../downloads/wil.zip "$WIL_URL"
fi

verify_sha256 "../downloads/wil.zip" "$WIL_SHA256"

if [ -z "${DOWNLOAD_ONLY+x}" ]; then
    unzip -q ../downloads/wil.zip
    rm -rf ../wil/
    mv wil-$WIL_VERSION/include/wil ../
    cat wil-$WIL_VERSION/LICENSE > ../wil/LICENSE
    echo >> ../wil/LICENSE
    cat wil-$WIL_VERSION/ThirdPartyNotices.txt >> ../wil/LICENSE
fi

# Cleanup
cd ..
rm -rf tmp
