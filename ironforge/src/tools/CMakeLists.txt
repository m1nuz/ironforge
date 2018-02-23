# generate main
add_executable(generate_main generate_main.cpp)

target_include_directories(generate_main PUBLIC
    ${INCLUDE_PATH}
)

#set_target_properties(generate_main PROPERTIES
#    CXX_STANDARD 14
#    CXX_STANDARD_REQUIRED YES
#    CXX_EXTENSIONS NO
#)

target_compile_options(generate_main PUBLIC
    -pthread
    -pedantic
    -Wall
    -Wextra
    -Wshadow
    -Wpointer-arith
    -Wcast-qual
    -Wunused-result
    -g
)
