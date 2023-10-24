set(MASM_SRC
    masm/SampleMod.asm
    masm/GuardGravity.asm
)

set (ASM_LIBS)
set (ASM_FLAGS)

set(CXX_LIBS
    d3d11.lib
    d3dcompiler.lib
    dinput8.lib
    #xinput.lib
    Xinput9_1_0.lib
    dxguid.lib

    ImGui
    libzip
    zlib
    Detours
)

set(CXX_FLAGS
    /nologo
    /c
    #/std:c++latest
    #/modulesearch
    /permissive-
    /experimental:module
    /Zc:forScope
    /Zc:inline
    /Zc:preprocessor
    /Zc:wchar_t
    /Zc:externC-
    /W3
    /wd4005 # macro redefinition
    /wd4996 # was declared deprecated
    /wd5105 # macro expansion producing 'defined' has undefined behavior
    /wd5202 # a global module fragment can only contain preprocessor directives
    /wd4244 # float to int conversion
    #/O2
    /Oi
    /Gy
    /Zi # make pdb file
    /EHsc
    /GS-
    /GR-
)

project(Crimson LANGUAGES CXX C ASM_MASM)

file(GLOB FONTS_HDR Fonts/*.hpp)
file(GLOB GLOBAL_MODULES Global/*.ixx)

file(GLOB MAIN_MODULES *.ixx)
file(GLOB MAIN_SRC 
    Crimson.cpp
)

file(GLOB CORE_MODULES Core/*.ixx)
file(GLOB CORE_HDR Core/*.h)

set(UTILITY_SRC
    Utility/Address.cpp
    Utility/Address.hpp
    Utility/Detour.cpp
    Utility/Detour.hpp
)

source_group("Global" FILES ${GLOBAL_MODULES})
source_group("Crimson" FILES ${MAIN_MODULES} ${MAIN_SRC})
source_group("Core" FILES ${CORE_MODULES} ${CORE_HDR})
source_group("Utility" FILES ${UTILITY_SRC})
source_group("ASM" FILES ${MASM_SRC})
source_group("Fonts" FILES ${FONTS_HDR})

add_library(${PROJECT_NAME} SHARED
            ${MAIN_SRC}
            ${CORE_HDR}
            ${UTILITY_SRC}
            ${MASM_SRC}
            ${FONTS_HDR}

            ${GLOBAL_MODULES}
            ${MAIN_MODULES}
            ${CORE_MODULES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME Crimson)

project(dinput8)

set(DI_SOURCE
    dinput8.cpp  
)

set(DI_CORE_SOURCE
    Core/DI8.ixx
    Core/Core.ixx
    Core/DataTypes.ixx
    Core/Windows.ixx
)

set(DI_CORE_HEADER
    Core/Macros.h
)

source_group("Core" FILES ${DI_CORE_SOURCE} ${DI_CORE_HEADER})
source_group("Source" FILES ${DI_SOURCE})

add_library(${PROJECT_NAME} SHARED
            ${DI_SOURCE}
            ${DI_CORE_SOURCE}
            ${DI_CORE_HEADER}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${CXX_FLAGS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME dinput8)