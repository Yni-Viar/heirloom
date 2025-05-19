#pragma once

// This is the precompiled header.

// Windows API
#include "windows10.h"
#include <shellapi.h>
#include <ShlObj.h>

// C++ Standard Library
#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// WIL
#include <wil/com.h>
#include <wil/resource.h>

// Immer
#pragma warning(push)
#pragma warning(disable : 4267)  // conversion from 'size_t' to 'immer::detail::rbts::count_t', possible loss of data
#include <immer/map_transient.hpp>
#include <immer/map.hpp>
#pragma warning(pop)
