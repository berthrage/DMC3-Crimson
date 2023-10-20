project(RapidJson)

file(GLOB ROOT_SOURCE *.cpp)
file(GLOB ROOT_HEADER *.h)

file(GLOB_RECURSE ERROR_HEADER ./error/*.h)

file(GLOB_RECURSE INTERNAL_HEADER ./internal/*.h)

file(GLOB_RECURSE MSINTTYPES_HEADER ./msinttypes/*.h)

source_group("Souce" FILES ${ROOT_SOURCE})
source_group("Header" FILES ${ROOT_HEADER})
source_group("Error" FILES ${ERROR_HEADER})
source_group("Inernal" FILES ${INTERNAL_HEADER})
source_group("MS INT Types" FILES ${MSINTTYPES_HEADER})

add_library(${PROJECT_NAME} STATIC
            ${ROOT_SOURCE}
            ${ROOT_HEADER}
			${ERROR_HEADER}
			${INTERNAL_HEADER}
			${MSINTTYPES_HEADER}
)