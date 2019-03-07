cmake_minimum_required(VERSION 3.5)

project("ironforge projects")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(TOOLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools)
set(GL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/GL/include CACHE PATH "GL include path")

add_subdirectory(external)
add_subdirectory(lib/GLcore)
add_subdirectory(lib/xxhash)
add_subdirectory(lib/nativefiledialog)
add_subdirectory(ironforge)
add_subdirectory(sandbox)
add_subdirectory(worldeditor)

message(${GLCORE_INCLUDE_PATH})
