set(LIB_NAME ironforge-scene)

file(GLOB_RECURSE HEADERS "../../include/scene/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

add_library(${LIB_NAME} ${HEADERS} ${DETAIL_HEADERS} ${SOURCES})
set_property(TARGET ${LIB_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})

target_include_directories(${LIB_NAME} PUBLIC
    ${GLCORE_INCLUDE_PATH}
    ${GL_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
    ${LUA_INCLUDE_DIR}
    ../../../lib/xxhash/include
    ../../include
)

target_link_libraries(${LIB_NAME} PUBLIC
    xxhash
    ironforge-video-gl330
    ${LUA_LIBRARIES}
    -ldl
)
