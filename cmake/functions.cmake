function(declare_library)

    cmake_parse_arguments(
            PARSED_ARGS # prefix of output variables
            "STATIC" # list of names of the boolean arguments (only defined ones will be true)
            "NAME" # list of names of mono-valued arguments
            "SRCS;DEPS;P_DEPS" # list of names of multi-valued arguments (output variables are lists)
            ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "declare_library must provide a name")
    endif(NOT PARSED_ARGS_NAME)

    if(PARSED_ARGS_STATIC)
        message("Building static library for ${PARSED_ARGS_NAME}")
    endif(PARSED_ARGS_STATIC)


    if (${EditorBuild} AND NOT PARSED_ARGS_STATIC)
        add_library(${PARSED_ARGS_NAME}.lib SHARED ${PARSED_ARGS_SRCS})
    else ()
        add_library(${PARSED_ARGS_NAME}.lib STATIC ${PARSED_ARGS_SRCS})
    endif ()

    target_link_libraries(${PARSED_ARGS_NAME}.lib PUBLIC ${PARSED_ARGS_DEPS})
    target_link_libraries(${PARSED_ARGS_NAME}.lib PRIVATE ${PARSED_ARGS_P_DEPS})
endfunction()