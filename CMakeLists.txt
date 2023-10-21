cmake_minimum_required(VERSION 3.28)

project(DMC3Crimson)

set(CMAKE_CXX_STANDARD 20)

#SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")
SET(CMAKE_CONFIGURATION_TYPES "Release")

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/zlib)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/libzip)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/ImGui)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/rapidjson)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/stb)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/SDL2)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glm)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Simple-SDL2)

add_compile_options(/W0)
add_compile_options("$<$<CONFIG:Release>:/MT>")
add_compile_options("$<$<CONFIG:Debug>:/MTd>")
add_compile_options(/Ob1)

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
link_directories(${CMAKE_CURRENT_LIST_DIR}/ThirdParty/bin)

# Store the root folder
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(ThirdParty)
add_subdirectory(src)
