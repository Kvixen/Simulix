cmake_minimum_required(VERSION 3.3) # 3.3 is the first version with IN_LIST
project({modelName} C CXX)
add_definitions(-DFMI_COSIMULATION)

set(MODEL_NAME {modelName})
set(PYTHON_PATH {path})
set(PROJECT_DIR ${{{modelName}_SOURCE_DIR}})
set(PYTHON_COMMAND python)
set(BINARY_PREFIX $ENV{{BINARY_PREFIX}})

if (NOT ${{BINARY_PREFIX}})
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


if (UNIX)
    set(PYTHON_COMMAND python3)
endif(UNIX)

# FindMatlab has a hardcoded list of all Matlab versions
# Because of older installs om CMake (i.e 3.3)
# Some Matlab versions might not exist
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

get_directory_property(PC_DEFINITIONS COMPILE_DEFINITIONS)
if(PC_DEFINITIONS MATCHES "^SIMULIX" OR PC_DEFINITIONS MATCHES ";SIMULIX")

    if (WIN32)

        target_link_libraries(
            ${{CMAKE_PROJECT_NAME}}
            Shlwapi.lib
        )

    endif()



    if($ENV{{SIMX_EXE}})

        add_executable(
            main 
            ${{PROJECT_DIR}}/exemain.c
            ${{PROJECT_DIR}}/includes/cJSON.c
            ${{PROJECT_DIR}}/includes/capi_utils.c
            ${{C_SOURCE_FILES}}
            ${{CPP_SOURCE_FILES}}
        )
        
        target_include_directories(
            main
            ${{PROJECT_DIR}}/includes
        )
    
        add_custom_command(
            OUTPUT ${{PROJECT_DIR}}/dllmain.c
            DEPENDS main
            COMMAND echo "Executing main executable"
            COMMAND ./main
            COMMAND echo "JSON to XML with Python"
            COMMAND ${{PYTHON_COMMAND}} ${{PYTHON_SOURCE_DIR}}/content-builder.py -o ${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/
        )

        list(APPEND C_SOURCE_FILES "${{PROJECT_DIR}}/${{MODEL_NAME}}/sources/dllmain.c")
    endif()
endif()

add_library(
    ${{CMAKE_PROJECT_NAME}}
    SHARED
    ${{C_SOURCE_FILES}}
    ${{CPP_SOURCE_FILES}}
)

set_target_properties(
    ${{CMAKE_PROJECT_NAME}}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${{BUILD_PATH}}"
    PREFIX ""    
)

if(CMAKE_C_FLAGS MATCHES "^-m32" OR CMAKE_C_FLAGS MATCHES " -m32")
    set(BINARY_PREFIX "${{BINARY_PREFIX}}32")
else()
    set(BINARY_PREFIX "${{BINARY_PREFIX}}64")
endif()



include_directories(${{PROJECT_DIR}}/${{MODEL_NAME}}/sources)

