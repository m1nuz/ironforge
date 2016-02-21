cmake_minimum_required(VERSION 3.0)

project("testgame")

include_directories(../ironforge/include)
include_directories(../lib/GLcore/include)
include_directories(../lib/GL/include)

set(cpp_flags "-pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wunused-result")

add_executable(simple-game-test src/config.hpp src/main.cpp)
set_property(TARGET simple-game-test PROPERTY CXX_STANDARD 14)
set_property(TARGET simple-game-test APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(simple-game-test ironforge-core ironforge-video-gl330 ironforge-readers)
