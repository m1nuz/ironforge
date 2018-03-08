set(LIB_NAME ironforge-renderer)

file(GLOB_RECURSE HEADERS "../../include/renderer/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

add_library(${LIB_NAME} ${HEADERS} ${DETAIL_HEADERS} ${SOURCES})
set_property(TARGET ${LIB_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})

target_include_directories(${LIB_NAME} PUBLIC
    ${GLCORE_INCLUDE_PATH}
    ${GL_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
    ../../include
)
