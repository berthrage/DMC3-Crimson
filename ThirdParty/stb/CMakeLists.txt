project(STB LANGUAGES C)

set(SRC
    stb_image.h
    dummy.c # Cause fucking cmake
)

source_group("STB Image" FILES ${SRC})

add_library(${PROJECT_NAME} STATIC
            ${SRC}         
)