set(LIB_NAME ironforge-ui)

file(GLOB_RECURSE HEADERS "../../include/ui/*.hpp")
file(GLOB_RECURSE DETAIL_HEADERS "./*.hpp")
file(GLOB_RECURSE SOURCES "./*.cpp")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
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
    ${SDL2_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ../../include
)

target_link_libraries(${LIB_NAME} PUBLIC
    ${SDL2_LIBRARY}
    ${SDL2_TTF_LIBRARIES}
)
