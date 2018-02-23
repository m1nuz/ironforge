include_directories(../../include)
include_directories(../../../lib/GLcore/include)
include_directories(../../../lib/GL/include)
include_directories(../../../lib/glm)

file(GLOB_RECURSE headers "../../include/renderer/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(renderer_lib "ironforge-null-renderer")

add_library(${renderer_lib} ${headers} ${detail_headers} ${sources})
#set_property(TARGET ${renderer_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${renderer_lib} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
