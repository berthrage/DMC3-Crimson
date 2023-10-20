project(SKinHook CXX)

file(GLOB SRC_FILES *.c)
file(GLOB HDR_FILES *.h)

file(GLOB HDE_SRC_FILES *.c)
file(GLOB HDE_HDR_FILES *.h)

source_group("Source" FILES ${HDE_SRC_FILES})
source_group("Header" FILES ${HDE_HDR_FILES})

source_group("HDE" ${HDE_SRC_FILES} ${HDE_HDR_FILES})

add_library(${PROJECT_NAME} STATIC
            ${SRC_FILES}
            ${HDR_FILES}
            ${HDE_SRC_FILES}
            ${HDE_HDR_FILES}
)