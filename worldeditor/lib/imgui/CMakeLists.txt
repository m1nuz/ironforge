find_package(SDL2 REQUIRED)

set(LIB_NAME imgui)

set(SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_impl_sdl_gl3.cpp
    imgui_dock.cpp)

add_library(${LIB_NAME} STATIC ${SOURCES} ${HEADERS})

set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(${LIB_NAME} PUBLIC
    ${SDL2_INCLUDE_DIR}
    ../../../lib/GLcore/include
)
