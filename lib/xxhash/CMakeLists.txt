cmake_minimum_required(VERSION 2.8)

include_directories("./include")
add_library(xxhash STATIC include/xxhash.h src/xxhash.c)
