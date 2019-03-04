cmake_minimum_required(VERSION 2.8)

set(NFD_INCLUDE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "Path to include nfd")

add_library(nfd STATIC
    src/nfd_common.c
    src/nfd_zenity.c)

target_include_directories(nfd PUBLIC ${NFD_INCLUDE_PATH})
