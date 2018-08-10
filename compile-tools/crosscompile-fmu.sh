#!/bin/bash
declare -a packages=("mingw-w64" "testing-fail-package" "cmake" "python3")

# Import functions from compile-functions.sh
# That file was created to increase readability
# Functions imported that are used in this script is 
# handle_packages
# check_for_element
# compile
. ./compile-functions.sh
handle_packages


# We need to pass path as argument
D=$1


# If user denied installing or something happend during the install the package is no longer in the array
# If the package is not in the array we can't compile those binaries.
check_for_element "mingw-w64" "${packages[@]}"
if [ $? -eq 0 ]; then
    compile "Windows 32bit dll" $D "win32" 0 "toolchains/Ubuntu-mingw32.cmake"
    compile "Windows 64bit dll" $D "win64" 0 "toolchains/Ubuntu-mingw64.cmake"
fi

# TODO: Add OSX support. 

# These can always compile, no need for a package
compile "Linux 32bit shared object" $D "linux32" 1 
compile "Linux 64bit shared object" $D "linux64" 0


