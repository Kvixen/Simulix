function(prepare_simulix)
    if ("${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/fmuTemplate.c" IN_LIST C_SOURCE_FILES)
        set(EXIST_FMU_TEMPLATE TRUE)
    else()
        set(EXIST_FMU_TEMPLATE FALSE)
    endif()

    if (WIN32)

        target_link_libraries(
            ${{CMAKE_PROJECT_NAME}}
            Shlwapi.lib
        )

    endif()

    if(NOT ${{EXIST_FMU_TEMPLATE}})
        # This shouldn't happen unless someone deleted the fmuTemplate and fmi2<name> files after running Simulix.
        # As we have changed fmuTemplate.c to match our main.c file we need to supply our file or it wont work.
        # It's a very unnecessary procedure because, as mentioned above, it wont happen if the user knows what they are doing
        list(APPEND C_SOURCE_FILES ${{PYTHON_PATH}}/libraryincludes/fmuTemplate.c)
        file(GLOB FMUSDK_HEADERS ${{PYTHON_PATH}}/libraryincludes/*.h)
        add_custom_command(
            TARGET ${{CMAKE_PROJECT_NAME}}
            POST_BUILD
            COMMAND ${{CMAKE_COMMAND}} -E copy ${{PYTHON_PATH}}/libraryincludes/fmuTemplate.c ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/
            COMMAND ${{CMAKE_COMMAND}} -E copy ${{FMUSDK_HEADERS}} ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/
        )
        set(EXIST_FMU_TEMPLATE TRUE)
    endif()
endfunction()