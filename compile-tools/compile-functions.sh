#!/bin/bash
# This file exists to increase readability
# These functions will be imported to crosscompile-fmu.sh

# Checks if package $1 is installed. Returns 0 if it is and 1 if it's not. 
function check_install () {
    dpkg -s $1 &> /dev/null

    if [ $? -eq 0 ]; then
        return 0
    else
        return 1
    fi
}

# Checks if an element exist in an array
# First argument is element to match
# Second is array
# array=("Something" "To" "Search" "For")
# hello_world=("Hello" "World")
# Example use:
# 
# check_for_element "Hello" "${array[@]}"
# echo $?
# 1
# check_for_element "Hello" "${hello_world[@]}"
# echo $?
# 0
function check_for_element () {
    local arr match="$1"
    # 'If N is not present, it is assumed to be 1.'
    # This is equal to shift 1
    shift
    for arr; do [[ "$arr" == "$match" ]] && return 0; done
    return 1
}

# Check if user wants to accept or deny prompt.
# Gets called with a question string like:
# Would you like milk for your cookies?
# Y returns 0
# N returns 1
function select_menu () {
    echo "$1 [Y/N]"
    while true; do
    read -rsn1 input
        if [ "$input" = "y" ]; then 
            return 0
        elif [ "$input" = "n" ]; then
            return 1
        fi
    done
}

# Takes a path to delete and remake to prepare for build
function prepare_build_directory () {
    rm -rf $1
    mkdir -p $1
}

# Compile function. 
# REQUIRED
# $1 is a string explaining what is compiling (ex Windows 32bit dll)
# $2 is path to source
# $3 is the binary prefix
# $4 Set to 1 if you want CFLAGS and CXXFLAGS to be set to -m32
# OPTIONAL
# $5 is path to toolchain file
function compile () {
    local CFLAGS CXXFLAGS
    if [ "$#" -lt 4 ]; then
        echo "Too few arguments supplied"
        return
    fi

    echo "Compiling $1"
    B=$2/build/$3

    prepare_build_directory $B

    if [ $4 -ne 0 ]; then
        CFLAGS=-m32 
        CXXFLAGS=-m32
    fi

    echo "$CFLAGS"

    if [ -z "$5" ]; then
        cmake $1 -B$B
    else
        cmake $1 -B$B -DCMAKE_TOOLCHAIN_FILE=`readlink -f $5`
    fi


    make -C $B
}


# Special function that handles the global variable packages
# All elements in packages will be checked if they are installed
# If they are not, the user will be promted to install them.
# If Simulix is dependent on the package, the program will quit on deny to install.
# If it's soft dependent on the package, the program will remove the element from the array.
function handle_packages () {
    for i in "${packages[@]}"
    do
        check_install "$i"
        if [ $? -eq 1 ]; then
            select_menu "You're missing package $i. It's required to crosscompile to certain systems. Would you like to install it?"
            if [ $? -eq 1 ]; then
                if [ "$i" = "cmake" ] || [ "$i" = "python3" ]; then
                    echo "Program can not run without $i. Make sure to install it before using Simulix"
                    exit 1
                else
                    echo "Compiling will continue without package $i. Some binaries will be missing in the FMU."
                    packages=( "${packages[@]/$i}" )
                fi
            else
                echo "Will try to install $i using flag --assume-yes, please wait..."
                echo "This can take a minute."
                # Only closing STDOUT lets the user see if they input the wrong password from sudo command
                sudo apt-get --assume-yes install "$i" >&-
                error_code=$?
                if [ $error_code -ne 0 ]; then
                    echo "Failed to install $i with error code $error_code."
                    if [ "$i" = "cmake" ] || [ "$i" = "python3" ]; then
                        echo "The program can't continue without this package. Install it outside Simulix and re-run it"
                    else
                        echo "The program will continue without compiling using this package."
                        packages=( "${packages[@]/$i}" )
                    fi
                else
                    echo "$i sucessfully installed!"
                fi
            fi
        fi
    done
}
