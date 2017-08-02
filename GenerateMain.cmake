macro(GENERATE_MAIN TARGET STARTUP)

    add_custom_command(OUTPUT ${GENERATE_MAIN_OUTPUT}
        COMMAND ${CMAKE_BINARY_DIR}/generate_main
        ARGS -s ${STARTUP} ${GENERATE_MAIN_OUTPUT}
        DEPENDS ${CMAKE_BINARY_DIR}/generate_main
        COMMENT "Generating source code for main.cpp"
        )

    add_custom_target(generate_main_${TARGET} DEPENDS ${GENERATE_MAIN_OUTPUT}
        COMMENT "Checking if re-generation is required"
        )

    add_dependencies(${TARGET} generate_main_${TARGET})
endmacro()
