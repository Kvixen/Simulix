# Toolchain file for building Windows 32bit

set(CMAKE_SYSTEM_NAME Windows)

set(PREFIX i686-w64-mingw32)
set(CMAKE_C_COMPILER ${PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${PREFIX}-g++)
set(CMAKE_RC_COMPILER ${PREFIX}-windres)
	
set(DISABLE_WARNINGS 1)

# CMake makes multiple calls to the Toolchain file
# In order to debug something but not make it happen mutliple times, we check the variable
# CMAKE_TOOLCHAIN_FILE 
# Which is only set the first time the file is called

if(${DISABLE_WARNINGS} and ${CMAKE_TOOLCHAIN_FILE})

    # Print warning messages
    # message(STATUS Warning messages enabled in ${TOOLCHAIN_PREFIX} toolchain file)

    elseif(${CMAKE_TOOLCHAIN_FILE})

    # message(STATUS Warning messages disabled in ${TOOLCHAIN_PREFIX} toolchain file)
    
endif()
