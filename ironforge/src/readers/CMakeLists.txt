set(LIB_NAME ironforge-readers)

file(GLOB_RECURSE HEADERS "../../include/readers/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

find_package(SDL2 REQUIRED)
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
    ${GLM_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ../../include
)

target_link_libraries(${LIB_NAME} PUBLIC
    ${SDL2_LIBRARY}
    -lstdc++fs
)
