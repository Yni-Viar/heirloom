#!/bin/bash
set -euo pipefail

# Setup
cd "$( dirname "${BASH_SOURCE[0]}" )"
rm -rf tmp
mkdir -p tmp
cd tmp

# Windows Implementation Library (WIL)
WIL_VERSION="158863d89f6b462c60fb56b65ae4db6afee98adf"
curl -Lo wil.zip "https://github.com/microsoft/wil/archive/$WIL_VERSION.zip"
unzip -q wil.zip
rm -rf ../wil/
mv wil-$WIL_VERSION/include/wil ../
cat wil-$WIL_VERSION/LICENSE > ../wil/LICENSE
echo >> ../wil/LICENSE
cat wil-$WIL_VERSION/ThirdPartyNotices.txt >> ../wil/LICENSE

# Cleanup
cd ..
rm -rf tmp
