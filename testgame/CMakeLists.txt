cmake_minimum_required(VERSION 3.0)

project("testgame")

include_directories(${INCLUDE_PATH})
include_directories(${LIB_PATH}/GLcore/include)
include_directories(${LIB_PATH}/GL/include)
include_directories(${LIB_PATH}/glm)
include_directories(${LIB_PATH}/xxhash/include)
include_directories(${LUA_INCLUDE_DIR})

set(cpp_flags "-std=c++17 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wunused-result")

set(libs
    ironforge-core
    ironforge-video-gl330
    ironforge-readers
    ironforge-null-renderer
    ironforge-scene
    ironforge-ui
    )

message(${INCLUDE_PATH})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()


##set(GENERATE_MAIN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)


add_executable(simple-game-test src/main.cpp)
##generate_main(simple-game-test ../assets/test/scripts/test.lua)
#set_property(TARGET simple-game-test PROPERTY CXX_STANDARD 14)
set_property(TARGET simple-game-test APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(simple-game-test ${libs})
