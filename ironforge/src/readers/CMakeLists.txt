include_directories(../../include)
include_directories(../../../lib/glm)

file(GLOB_RECURSE headers "../../include/readers/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(readers_lib "ironforge-readers")

add_library(${readers_lib} ${headers} ${detail_headers} ${sources})
set_property(TARGET ${readers_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${readers_lib} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(${readers_lib} -lSDL2 -lSDL2_ttf -lboost_system -lboost_filesystem)
