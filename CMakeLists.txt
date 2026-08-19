cmake_minimum_required(VERSION 3.28)

project(DMC3Crimson)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/zlib)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/libzip)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/ImGui)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/rapidjson)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/stb)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/SDL2)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glm)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Detours/Detours/src)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/debug-draw)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/curl/include)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/openssl/include)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Effekseer/src/Effekseer)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Effekseer/src/EffekseerRendererDX11)
include_directories(${CMAKE_SOURCE_DIR}/src/Utility)

add_compile_options(/W0)
add_compile_options("$<$<CONFIG:Release>:/MT>")
add_compile_options("$<$<CONFIG:Debug>:/MTd>")

# /Ob1 (inline expansion) — keep for Release only; skip in Debug for speed
add_compile_options("$<$<CONFIG:Release>:/Ob1>")

# Disable ImGui's built-in XInput gamepad handler (slot 0 only).
# Crimson provides its own multi-controller navigation via SDL
# in CoreImGui::XI::UpdateGamepad().
add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

# Prevent windows.h from defining min/max as preprocessor macros.
# These corrupt GLM's use of std::numeric_limits<T>::max() and similar
# patterns. This must be a global compile definition (not just in the PCH)
# because the first build pass compiles sources before the PCH is ready.
add_compile_definitions(NOMINMAX)

# --- Release flags ---------------------------------------------------------
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:FULL /INCREMENTAL /OPT:NOREF /OPT:NOICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG:FULL /INCREMENTAL /OPT:NOREF /OPT:NOICF")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /DEBUG:FULL /INCREMENTAL /OPT:NOREF /OPT:NOICF")

# --- Debug flags (optimized for compilation speed) -------------------------
# /O1 (minimize size) compiles faster than /Od while still being debuggable
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /O1")
# /Z7 embeds debug info in .obj files — avoids mspdbsrv.exe bottleneck on slow CPUs
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
# /DEBUG:FASTLINK for faster incremental linking in Debug
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG:FASTLINK /INCREMENTAL")

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:threadSafeInit- /MP")

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    set(UOUTPUTCONFIG OUTPUTCONFIG)
    string(TOUPPER ${UOUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UOUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UOUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UOUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

link_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/lib)

# Store the root folder
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(ThirdParty)
add_subdirectory(src)

