project(ImGui)

file(GLOB SRC_FILES *.cpp)

file(GLOB HDR_FILES *.h)

file(GLOB BKND_FILES Backend/*.cpp
                     Backend/*.h)

source_group("Source" FILES ${SRC_FILES})
source_group("Header" FILES ${HDR_FILES})
source_group("Backend" FILES ${BKND_FILES})

add_library(${PROJECT_NAME} STATIC
            ${SRC_FILES}
            ${HDR_FILES}
            ${BKND_FILES}
)