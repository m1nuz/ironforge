set(LIB_NAME ironforge-core)

file(GLOB_RECURSE HEADERS "../../include/core/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

find_package(SDL2 REQUIRED)
find_package(GLM REQUIRED)

add_library(${LIB_NAME} ${HEADERS} ${DETAIL_HEADERS} ${SOURCES})
set_property(TARGET ${LIB_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})

target_include_directories(${LIB_NAME} PUBLIC
    ${GLCORE_INCLUDE_PATH}
    ${GL_INCLUDE_PATH}
    ${GLM_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ../../include
)

target_link_libraries(${LIB_NAME} PUBLIC
    ${SDL2_LIBRARY}
)
