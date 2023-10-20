file(GLOB ROOT_MODULES ${PROJECT_ROOT}/*.ixx)

file(GLOB MAIN_MODULES *.ixx)
file(GLOB MAIN_SRC *.cpp)

file(GLOB CORE_MODULES Core/*.ixx)
file(GLOB CORE_HDR Core/*.h)

source_group("Root" FILES ${ROOT_MODULES})
source_group("Crimson" FILES ${MAIN_MODULES} ${MAIN_SRC})
source_group("Core" FILES ${CORE_MODULES} ${CORE_HDR})

add_library(${CMAKE_PROJECT_NAME} SHARED
            ${ROOT_MODULES}
            ${MAIN_MODULES}
            ${MAIN_SRC}
            ${CORE_MODULES}
            ${CORE_HDR}        
)