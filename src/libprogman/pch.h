#pragma once

// This is the precompiled header.

#include "windows10.h"

// C++ Standard Library
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Windows API
#include <ShlObj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shellapi.h>

// WIL -- must come after Windows API includes
#include <wil/resource.h>
#include <wil/com.h>
