cmake_minimum_required(VERSION 3.0)

project("ironforge projects")

include(SubdirList.cmake)

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ironforge/include)
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools)

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

list(REMOVE_ITEM SUBDIRS ".git")
list(REMOVE_ITEM SUBDIRS "build")
list(REMOVE_ITEM SUBDIRS "assets")
list(REMOVE_ITEM SUBDIRS "screenshots")

FOREACH(subdir ${SUBDIRS})
    ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()
