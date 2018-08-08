cmake_minimum_required(VERSION 3.3) # 3.3 is the first version with IN_LIST
set(MODEL_NAME {modelName})
set(PYTHON_PATH {path})
project({modelName} C CXX)
set(PROJECT_DIR ${{{modelName}_SOURCE_DIR}})
add_definitions(-DFMI_COSIMULATION)
get_directory_property(PC_DEFINITIONS COMPILE_DEFINITIONS)

# Get the latest install as FindMatlab module fails to do this
# This is because of older CMake installs
# FindMatlab has a hardcoded list of all Matlab versions
# Here we register extra versions to cover all bases
set(MATLAB_ADDITIONAL_VERSIONS
    "R2017b=9.3"
    "R2017a=9.2"
    "R2016b=9.1"
    "R2016a=9.0"
    "R2015b=8.6"
    "R2015a=8.5"
    "R2014b=8.4"
    )

include(FindMatlab)



file(GLOB C_SOURCE_FILES ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/*.c)
file(GLOB CPP_SOURCE_FILES  ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/*.cpp)

add_library(
    ${{CMAKE_PROJECT_NAME}}
    SHARED
    ${{C_SOURCE_FILES}}
    ${{CPP_SOURCE_FILES}}
)

message(STATUS "List C_SOURCE_FILES: ${{C_SOURCE_FILES}}")

if ("fmuTemplate.c" IN_LIST C_SOURCE_FILES)
    set(EXIST_FMU_TEMPLATE TRUE)
else()
    set(EXIST_FMU_TEMPLATE FALSE)
endif()

# These are special Simulix includes and other things.
if(PC_DEFINITIONS MATCHES "^SIMULIX" OR PC_DEFINITIONS MATCHES ";SIMULIX")
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
            COMMAND ${{CMAKE_COMMAND}} -E copy ${{PYTHON_PATH}}/libraryincludes/fmuTemplate.c ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/
            COMMAND ${{CMAKE_COMMAND}} -E copy ${{FMUSDK_HEADERS}} ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/
        )

        set(EXIST_FMU_TEMPLATE TRUE)
    endif()
endif()




include_directories(${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/*.c)

set_target_properties(${{CMAKE_PROJECT_NAME}} PROPERTIES PREFIX "")

