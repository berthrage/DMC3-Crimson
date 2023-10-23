set(MASM_SRC
    masm/SampleMod.asm
    masm/GuardGravity.asm
)

set (ASM_LIBS)
set (ASM_FLAGS)

set(CXX_LIBS
    msvcrt.lib
    vcruntime.lib
    ucrt.lib
    libcpmt.lib
    kernel32.lib
    user32.lib
    shell32.lib
    imm32.lib
    advapi32.lib
    d3d11.lib
    d3dcompiler.lib
    dinput8.lib
    #xinput.lib
    Xinput9_1_0.lib
    dxguid.lib

    SDL2.lib
    SDL2_mixer.lib
    libzip.lib
    zlib.lib

    ImGui
    SKinHook
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

file(GLOB GLOBAL_MODULES Global/*.ixx)

file(GLOB MAIN_MODULES *.ixx)
file(GLOB MAIN_SRC Crimson.cpp)

file(GLOB CORE_MODULES Core/*.ixx)
file(GLOB CORE_HDR Core/*.h)

source_group("Global" FILES ${GLOBAL_MODULES})
source_group("Crimson" FILES ${MAIN_MODULES} ${MAIN_SRC})
source_group("Core" FILES ${CORE_MODULES} ${CORE_HDR})
source_group("masm" FILES ${MASM_SRC})

add_library(${PROJECT_NAME} SHARED
            ${MAIN_SRC}
            ${CORE_HDR}
            ${MASM_SRC}     
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET crimson_modules TYPE CXX_MODULES FILES
    ${GLOBAL_MODULES}
    ${MAIN_MODULES}
    ${CORE_MODULES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CXX_LIBS}) # Dependencies
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME Crimson)

project(dinput8)

source_group("Global" FILES ${GLOBAL_MODULES})
source_group("Core" FILES ${CORE_MODULES} ${CORE_HDR})
source_group("Source" FILES "dinput8.cpp")

add_library(${PROJECT_NAME} SHARED
            "dinput8.cpp"
            ${CORE_HDR}
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET dinput8_modules TYPE CXX_MODULES FILES
    ${CORE_MODULES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${CXX_FLAGS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME dinput8)