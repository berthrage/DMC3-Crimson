// ============================================================================
// Crimson Precompiled Header
// Collects the heaviest, most-included system and third-party headers so they
// are parsed once instead of ~75 times. These headers rarely change.
// ============================================================================

#pragma once

// --- Windows SDK -----------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX                // Prevent windows.h from defining min/max macros
#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef DELETE
// NOTE: do NOT #undef CopyMemory here — ws2tcpip.h and other Windows SDK
// headers rely on it being available as a macro/function.

#include <d3d11.h>
#include <d3d10.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dinput.h>
#include <Xinput.h>

#include <shellapi.h>
#include <emmintrin.h>
#include <intrin.h>

// --- STL -------------------------------------------------------------------
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <filesystem>
#include <thread>
#include <chrono>
#include <tuple>
#include <utility>
#include <array>
#include <sstream>
#include <iostream>
#include <fstream>
#include <type_traits>

// --- ImGui -----------------------------------------------------------------
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include <ImGui/Backend/imgui_impl_win32.h>
#include <ImGui/Backend/imgui_impl_dx11.h>

// --- SDL bridge (SDL3 + SDL2_mixer compat, mirrors SDL.hpp) ----------------
#include <SDL3/SDL.h>
typedef SDL_IOStream SDL_RWops;
#define SDL_bool bool
#define SDL_MIX_MAXVOLUME 128
#define SDL_rwops_h_
struct SDL_version;
#include <SDL2/SDL_mixer.h>

// --- GLM is intentionally NOT in the PCH ------------------------------------
// Different translation units need different defines (GLM_FORCE_ROW_MAJOR,
// GLM_ENABLE_EXPERIMENTAL) before including GLM. Having GLM in the PCH
// prevents those defines from taking effect. GLM is also fast to parse
// compared to Windows/D3D/ImGui headers, so the compile-time savings are
// negligible. Each TU includes GLM directly as needed.

// --- RapidJSON -------------------------------------------------------------
#define RAPIDJSON_NO_SIZETYPEDEFINE
#define RAPIDJSON_HAS_STDSTRING 1
// SizeType must be defined before RapidJSON headers because
// RAPIDJSON_NO_SIZETYPEDEFINE tells RapidJSON to skip its own definition.
namespace rapidjson {
#ifdef _WIN64
typedef unsigned long long SizeType;
#else
typedef unsigned long SizeType;
#endif
} // namespace rapidjson
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// --- Project core types (rarely change) ------------------------------------
#include "Core/Macros.h"
#include "Core/DataTypes.hpp"

// --- Global convenience: bare min/max → std::min / std::max -----------------
// After all headers are processed and Windows macros are gone, provide
// std::min/std::max in the global namespace so existing code that uses bare
// min()/max() (formerly relying on the Windows macros) continues to compile.
using std::min;
using std::max;
