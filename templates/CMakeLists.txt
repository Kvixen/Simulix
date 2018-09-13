function(ADD_PREFIX OUT_LIST PREFIX INPUT_LIST)
    set(TEMP_LIST "")
    foreach(LIST_FILE ${{INPUT_LIST}})
        LIST(APPEND TEMP_LIST "${{PREFIX}}${{LIST_FILE}}")
    endforeach()
    set(${{OUT_LIST}} "${{TEMP_LIST}}" PARENT_SCOPE)
endfunction()

cmake_minimum_required(VERSION 3.3)
project({modelName} C CXX)
add_definitions(-DFMI_COSIMULATION)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
include(CMakeForceCompiler)

set(MODEL_NAME {modelName})
set(PYTHON_PATH {path})
set(PROJECT_DIR ${{{modelName}_SOURCE_DIR}})
set(FMU_PATH "${{PROJECT_DIR}}/${{MODEL_NAME}}")
set(GENERATED_FILES_PATH "$ENV{{SIMX_TEMP_DIR}}")
set(BAD_SIMX_SOURCES
    "fmuTemplate.c"
)

# Prepare FMU folders
file(MAKE_DIRECTORY "${{FMU_PATH}}")
file(MAKE_DIRECTORY "${{FMU_PATH}}/sources")
file(MAKE_DIRECTORY "${{FMU_PATH}}/documentation")
file(MAKE_DIRECTORY "${{FMU_PATH}}/binaries")

if ("${{BINARY_PREFIX}}" STREQUAL "")
    if(UNIX)
        if(APPLE)
            # Support for APPLE? (DARWIN)
            # set(BINARY_PREFIX "darwin")
        else()
            set(BINARY_PREFIX "linux")
        endif()
    elseif(WIN32)
        set(BINARY_PREFIX "win")
    endif()
endif()

set(SRCS "")

foreach(FILE_PATH $ENV{{SIMX_SOURCES_LIST}})
    configure_file(${{FILE_PATH}} ${{FMU_PATH}}/sources COPYONLY)
    string(REGEX REPLACE ".+[\\/]" "" FILE_NAME ${{FILE_PATH}})
    if ("${{FILE_NAME}}" MATCHES ".*\.(c|cpp)$" AND NOT "${{FILE_NAME}}" IN_LIST BAD_SIMX_SOURCES)
        list(APPEND SRCS ${{FILE_NAME}})
    endif()
endforeach()

ADD_PREFIX(PRESRCS "${{FMU_PATH}}/sources/" "${{SRCS}}")

get_directory_property(PC_DEFINITIONS COMPILE_DEFINITIONS)
if(PC_DEFINITIONS MATCHES "^SIMULIX" OR PC_DEFINITIONS MATCHES ";SIMULIX")


    if($ENV{{SIMX_EXE}})

        # These sources are generated or copied by Simulix, needed to build the executable
        set(EXECUTABLE_SOURCES
            "${{GENERATED_FILES_PATH}}/Simulix_exemain.c"
            "${{GENERATED_FILES_PATH}}/includes/cJSON.c"
            "${{GENERATED_FILES_PATH}}/includes/Simulix_capi_utils.c"
        )

        find_package(PythonInterp 3 REQUIRED)
        if (${{PYTHONINTERP_FOUND}})
            set(PYTHON_COMMAND ${{PYTHON_EXECUTABLE}})
        else()
            set(PYTHON_COMMAND python)
            if (UNIX)
                set(PYTHON_COMMAND python3)
            endif(UNIX)
        endif()

        add_executable(
            main
            ${{EXECUTABLE_SOURCES}}
            ${{PRESRCS}}
        )

        target_include_directories(
            main
            PUBLIC
            "${{GENERATED_FILES_PATH}}/includes"
        )

        add_custom_command(
            OUTPUT ${{FMU_PATH}}/sources/Simulix_dllmain.c
            DEPENDS main
            COMMAND echo "Executing main executable"
            COMMAND ./main
            COMMAND echo "JSON to XML with Python"
            COMMAND ${{PYTHON_COMMAND}} ${{PYTHON_PATH}}/content-builder.py -o ${{FMU_PATH}}/sources
            COMMAND ${{CMAKE_COMMAND}} -E copy ${{CMAKE_BINARY_DIR}}/modelDescription.xml ${{FMU_PATH}}
        )

        set(LIBRARY_SOURCES
            "fmi2Functions.h"
            "fmi2FunctionTypes.h"
            "fmi2TypesPlatform.h"
            "fmuTemplate.c"
            "fmuTemplate.h"
        )
        message(STATUS "GENERATED_FILES_PATH: ${{GENERATED_FILES_PATH}}")
        foreach(FILE_NAME ${{LIBRARY_SOURCES}})
            configure_file("${{GENERATED_FILES_PATH}}/libraryincludes/${{FILE_NAME}}" "${{FMU_PATH}}/sources" COPYONLY)
        endforeach()
        configure_file("${{GENERATED_FILES_PATH}}/libraryincludes/fmusdk_license.txt" "${{FMU_PATH}}/documentation")

        if(UNIX)
            target_link_libraries(
                main
                m
            )
        endif()
    endif()
    list(APPEND PRESRCS "${{FMU_PATH}}/sources/Simulix_dllmain.c")
endif()

add_library(
    ${{CMAKE_PROJECT_NAME}}
    SHARED
    ${{PRESRCS}}
)

if (WIN32)
    target_link_libraries(
        ${{CMAKE_PROJECT_NAME}}
        Shlwapi.lib
    )
elseif(UNIX)
    target_link_libraries(
        ${{CMAKE_PROJECT_NAME}}
        m
    )
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BINARY_PREFIX "${{BINARY_PREFIX}}64")
else()
    set(BINARY_PREFIX "${{BINARY_PREFIX}}32")
endif()

set(LIBRARY_PATH "${{FMU_PATH}}/binaries/${{BINARY_PREFIX}}")
file(MAKE_DIRECTORY "${{LIBRARY_PATH}}")

set_target_properties(
    ${{CMAKE_PROJECT_NAME}}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${{LIBRARY_PATH}}"
    RUNTIME_OUTPUT_DIRECTORY "${{LIBRARY_PATH}}"
    PREFIX ""
)

include_directories("${{FMU_PATH}}/sources")
