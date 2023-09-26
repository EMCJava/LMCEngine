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

        if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
            target_link_options(${PARSED_ARGS_NAME}.lib PRIVATE /PDBALTPATH:$<TARGET_PDB_FILE_NAME:${PARSED_ARGS_NAME}.lib>)
        endif ()

    else ()
        add_library(${PARSED_ARGS_NAME}.lib STATIC ${PARSED_ARGS_SRCS})
    endif ()

    target_link_libraries(${PARSED_ARGS_NAME}.lib PUBLIC ${PARSED_ARGS_DEPS})
    target_link_libraries(${PARSED_ARGS_NAME}.lib PRIVATE ${PARSED_ARGS_P_DEPS})
endfunction()

function(declare_main_executable)
    add_executable(${ARGN} main.cpp)
    if (MSVC)
        set_target_properties(${ARGN} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<0:>)
    endif (MSVC)

    if (NOT ${EditorBuild})
        target_link_libraries(GameWindow.lib PRIVATE ${ROOT_CONCEPT_LIB})
        target_link_libraries(Engine.lib ${ROOT_CONCEPT_LIB})
    endif ()

    target_link_libraries(${ARGN} ${ROOT_CONCEPT_LIB})
    target_link_libraries(${ARGN} Engine.lib)
endfunction()