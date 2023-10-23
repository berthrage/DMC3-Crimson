project(Detours)

set(DETOURS_SOURCE Detours/src)
add_library(Detours STATIC
             ${DETOURS_SOURCE}/creatwth.cpp
             ${DETOURS_SOURCE}/detours.cpp
             ${DETOURS_SOURCE}/detours.h
             ${DETOURS_SOURCE}/detver.h
             ${DETOURS_SOURCE}/disasm.cpp
             ${DETOURS_SOURCE}/disolarm.cpp
             ${DETOURS_SOURCE}/disolarm64.cpp
             ${DETOURS_SOURCE}/disolia64.cpp
             ${DETOURS_SOURCE}/disolx64.cpp
             ${DETOURS_SOURCE}/disolx86.cpp
             ${DETOURS_SOURCE}/image.cpp
             ${DETOURS_SOURCE}/modules.cpp
             ${DETOURS_SOURCE}/uimports.cpp)

            
# This file is included and not compiled on its own
set_property (
    SOURCE ${DETOURS_SOURCE}/uimports.cpp
    APPEND PROPERTY HEADER_FILE_ONLY true)