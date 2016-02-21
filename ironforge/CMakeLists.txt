cmake_minimum_required(VERSION 3.0)

project("ironforge")

set(cpp_flags "-pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wunused-result")

add_subdirectory(src/core)
add_subdirectory(src/video)
add_subdirectory(src/audio)
add_subdirectory(src/readers)

set(audio_lib "ironforge-audio-al")
