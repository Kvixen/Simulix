set(CMAKE_SYSTEM_NAME Linux)
set(PREFIX arm-linux-gnueabi)
set(BINARY_PREFIX "linux")

set(CMAKE_C_COMPILER ${PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${PREFIX}-g++)
	
set(DISABLE_WARNINGS 1)

# CMake makes multiple calls to the Toolchain file
# In order to debug something but not make it happen mutliple times, we check the variable
# CMAKE_TOOLCHAIN_FILE 
# Which is only set the first time the file is called

if("${DISABLE_WARNINGS}" AND "${CMAKE_TOOLCHAIN_FILE}")

    # Print warning messages
    # message(STATUS Warning messages enabled in ${TOOLCHAIN_PREFIX} toolchain file)

    elseif(${CMAKE_TOOLCHAIN_FILE})

    # message(STATUS Warning messages disabled in ${TOOLCHAIN_PREFIX} toolchain file)
    
endif()

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH /usr/${PREFIX})


# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)