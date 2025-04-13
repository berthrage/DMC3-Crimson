project(STB LANGUAGES C)

set(SRC
    stb_image.h
    stb_image_resize2.h
    dummy.c # Cause fucking cmake
)

source_group("STB Image" FILES ${SRC})

add_library(${PROJECT_NAME} STATIC
            ${SRC}         
)