# Toolchain file for building Windows 32bit

set(CMAKE_SYSTEM_NAME Windows)
set(PREFIX i686-w64-mingw32)
set(BINARY_PREFIX "win")

set(CMAKE_C_COMPILER ${PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${PREFIX}-g++)
set(CMAKE_RC_COMPILER ${PREFIX}-windres)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	
set(DISABLE_WARNINGS 1)

# CMake makes multiple calls to the Toolchain file
# In order to debug something but not make it happen mutliple times, we check the variable
# CMAKE_TOOLCHAIN_FILE 
# Which is only set the first time the file is called

if"${DISABLE_WARNINGS}" AND "${CMAKE_TOOLCHAIN_FILE}")

    # Print warning messages
    # message(STATUS Warning messages enabled in ${TOOLCHAIN_PREFIX} toolchain file)

    elseif(${CMAKE_TOOLCHAIN_FILE})

    # message(STATUS Warning messages disabled in ${TOOLCHAIN_PREFIX} toolchain file)
    
endif()


# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)