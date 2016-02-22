include_directories(../../include)
include_directories(../../../lib/GLcore/include)
include_directories(../../../lib/GL/include)
include_directories(../../../lib/glm)

file(GLOB_RECURSE headers "../../include/scene/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(scene_lib "ironforge-scene")

add_library(${scene_lib} ${headers} ${detail_headers} ${sources})
set_property(TARGET ${scene_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${scene_lib} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(${scene_lib} -ljansson)