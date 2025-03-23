#pragma once

#if defined(_WIN32)
#define NOMINMAX
#define UNICODE
#include <windows.h>
#else
#error 0
#endif

// CRT
#include <cassert>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <numbers>
#include <string>
#include <unordered_map>
#include <unordered_set>

// SPZ
#include "load-spz.h"

// VOLK
#include "volk.h"

// SLANG
#include "slang-com-ptr.h"
#include "slang.h"

// Common
#include "3dgs/CommonDefines.h"

#include "3dgs/math/Matrix.hpp"
#include "3dgs/math/Vector.h"