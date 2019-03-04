add_subdirectory(lib/imgui)

set(cpp_flags "-std=c++17 -pedantic -Wall -Wextra")

set(APP_NAME world-editor)

set(SOURCES src/main.cpp)

add_executable(${APP_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${APP_NAME} PUBLIC
    ${SHARED_INCLUDE_PATH}
    ${IRONFORGE_INCLUDE_PATH}
    ${GLCORE_INCLUDE_PATH}
    lib/imgui
    ${NFD_INCLUDE_PATH}
)

set_property(TARGET ${APP_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${cpp_flags})

target_link_libraries(${APP_NAME} PUBLIC
    ironforge-core
    ironforge-video-gl330
    ironforge-readers
    ironforge-renderer
    ironforge-scene
    ironforge-ui
    imgui
    nfd
)
