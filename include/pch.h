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
#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

// SPZ
#include "load-spz.h"

// VOLK
#include "volk.h"

// SLANG
#include "slang-com-ptr.h"
#include "slang.h"

// Common
#include "3dgs/CommonDefines.h"