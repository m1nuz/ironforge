include_directories(../../include)
include_directories(../../../lib/GLcore/include)
include_directories(../../../lib/GL/include)

file(GLOB_RECURSE headers "../../include/core/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(core_lib "ironforge-core")

add_library(${core_lib} ${headers} ${detail_headers} ${sources})
set_property(TARGET ${core_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${core_lib} APPEND_STRING PROPERTY COMPILE_FLAGS "-pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes")
target_link_libraries(${core_lib} -lSDL2 -lSDL2_ttf)
