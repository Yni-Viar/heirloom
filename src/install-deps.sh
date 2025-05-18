#!/bin/bash
# Set DOWNLOAD_ONLY to skip installation.
set -euo pipefail

# Setup
cd "$( dirname "${BASH_SOURCE[0]}" )"
rm -rf tmp
mkdir -p tmp downloads
cd tmp

echo "--- Windows Implementation Library (WIL) ---"
WIL_VERSION="158863d89f6b462c60fb56b65ae4db6afee98adf"
if [ ! -f "../downloads/wil.zip" ]; then
    curl -Lo ../downloads/wil.zip "https://github.com/microsoft/wil/archive/$WIL_VERSION.zip"
fi

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
