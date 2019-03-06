set(cpp_flags "-std=c++17 -pedantic -Wall -Wextra")

set(APP_NAME sandbox-game)

set(SOURCES src/main.cpp)

add_executable(${APP_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${APP_NAME} PUBLIC
    ${SHARED_INCLUDE_PATH}
    ${IRONFORGE_INCLUDE_PATH}
    ${GLCORE_INCLUDE_PATH}
)

target_compile_options(${APP_NAME} PUBLIC
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

target_link_libraries(${APP_NAME} PUBLIC
    ironforge-core
    ironforge-video-gl330
    ironforge-readers
    ironforge-renderer
    ironforge-scene
    ironforge-ui
)
