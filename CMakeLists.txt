cmake_minimum_required(VERSION 3.0)

project("ironforge projects")

include(SubdirList.cmake)
include(GenerateMain.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ironforge/include)
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools)

subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

list(REMOVE_ITEM SUBDIRS ".git")
list(REMOVE_ITEM SUBDIRS "build")
list(REMOVE_ITEM SUBDIRS "assets")
list(REMOVE_ITEM SUBDIRS "screenshots")

foreach(subdir ${SUBDIRS})
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt")
        add_subdirectory(${subdir})
    endif()
endforeach()
