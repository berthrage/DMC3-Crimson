project(SDL2SimpleAudio)

file(GLOB SRC_FILES *.c)

file(GLOB HDR_FILES *.h)

set_source_files_properties(SRC_FILES PROPERTIES LANGUAGE CXX)

source_group("Source" FILES ${SRC_FILES})
source_group("Header" FILES ${HDR_FILES})

add_library(${PROJECT_NAME} STATIC
            ${SRC_FILES}
            ${HDR_FILES}
)