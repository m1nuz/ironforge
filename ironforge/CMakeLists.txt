cmake_minimum_required(VERSION 3.0)

project("ironforge")

set(cpp_flags "-pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wunused-result")

subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR}/src)

foreach(subdir ${subdirs})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/${subdir})
endforeach()

set(audio_lib "ironforge-audio-al")

add_library(lua STATIC IMPORTED)
set_property(TARGET lua PROPERTY IMPORTED_LOCATION -usr/local/lib/liblua.a)
