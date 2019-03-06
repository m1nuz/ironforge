cmake_minimum_required(VERSION 3.0)

project("ironforge" VERSION 0.2.0)

set(cpp_flags "-std=c++17 -g -pthread -pedantic -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-qual -Wunused-result")
set(IRONFORGE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "Ironforge include path")

configure_file("${IRONFORGE_INCLUDE_PATH}/config.h.in" "${IRONFORGE_INCLUDE_PATH}/config.h")

add_subdirectory(src/audio)
add_subdirectory(src/core)
add_subdirectory(src/readers)
add_subdirectory(src/renderer)
add_subdirectory(src/scene)
add_subdirectory(src/ui)
add_subdirectory(src/video)
