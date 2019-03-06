set(LIB_NAME ironforge-renderer)

file(GLOB_RECURSE HEADERS "../../include/renderer/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

find_package(GLM REQUIRED)

add_library(${LIB_NAME} ${HEADERS} ${DETAIL_HEADERS} ${SOURCES})
target_compile_options(${LIB_NAME} PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
        -pthread
        -pedantic
        -Wall
        -Wextra
        -Werror
        -Wshadow
        -Wpointer-arith
        -Wcast-qual
        -Wunused-result
        )

target_include_directories(${LIB_NAME} PUBLIC
    ${GLCORE_INCLUDE_PATH}
    ${GL_INCLUDE_PATH}
    ${GLM_INCLUDE_DIRS}
    ../../include
)
