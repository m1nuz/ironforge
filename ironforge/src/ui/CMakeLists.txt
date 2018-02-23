include_directories(../../include)
include_directories(../../../lib/glm)

file(GLOB_RECURSE headers "../../include/ui/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(ui_lib "ironforge-ui")

add_library(${ui_lib} ${headers} ${detail_headers} ${sources})
#set_property(TARGET ${ui_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${ui_lib} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(${ui_lib} -lSDL2 -lSDL2_ttf)
