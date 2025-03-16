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

// SPZ
#include "load-spz.h"

// VOLK
#include "volk.h"

// Common
#include "3dgs/CommonDefines.h"