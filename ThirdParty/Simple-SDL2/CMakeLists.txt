project(SDL2SimpleAudio)

file(GLOB HDR_FILES *.h)

set_source_files_properties(SRC_FILES PROPERTIES LANGUAGE CXX)

source_group("Header" FILES ${HDR_FILES})

add_library(${PROJECT_NAME} INTERFACE
            ${HDR_FILES}
)

target_include_directories(${PROJECT_NAME} INTERFACE .)