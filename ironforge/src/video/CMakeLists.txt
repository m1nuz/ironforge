include_directories(../../include)
include_directories(../../../lib/GLcore/include)
include_directories(../../../lib/GL/include)

file(GLOB_RECURSE headers "../../include/video/*.hpp")
file(GLOB_RECURSE detail_headers "./*.hpp")
file(GLOB_RECURSE sources "./*.cpp")

set(video_lib "ironforge-video-gl330")

add_library(${video_lib} ${headers} ${detail_headers} ${sources})
set_property(TARGET ${video_lib} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${video_lib} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})
target_link_libraries(${video_lib} GLcore330)
