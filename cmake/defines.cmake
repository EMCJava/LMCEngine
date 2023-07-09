if (NOT DEFINED EditorBuild)
    set(EditorBuild false)
endif ()

if (${EditorBuild})
    add_compile_definitions(LMC_API_EXPORTS)
else ()
    add_compile_definitions(LMC_API_STATIC)
endif ()