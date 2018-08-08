"""
Simulix generates an FMU from a simulink model source code.
 
Copyright (C) 2018 Scania CV AB and Simulix contributors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
from os import mkdir, path, getcwd, chdir, system
import argparse
import subprocess
from shutil import which
from platform import system


BUILD_ONLY = False

def execute_build_commands(compile_program,make_program,dst):
    command = "cmake -G {0} ..".format(compile_program)
    print("Executing {0}".format(command))
    cmake_p = subprocess.Popen(command, shell=True)
    cmake_p.communicate()
    command = "{0}".format(make_program)
    make_p = subprocess.Popen(command, shell=True)
    make_p.communicate() # Wait for program to finish
    return

def prepare_build_directory(dst, folder_name):
    #If theres no build folder or we're currently in the build folder
    if not path.isdir(path.join(dst, folder_name)) and dst.split('/')[-1:] != folder_name:
        mkdir(path.join(dst, folder_name))
        chdir(path.join(dst, folder_name))
    #If theres a build folder but we're currently not in it
    elif dst.split('/')[-1:] != folder_name:
        chdir(path.join(dst, folder_name))

    

def build(compprog, makeprog, dst, folder_name=None):  
    prepare_build_directory(dst, folder_name)
    execute_build_commands(compprog, makeprog, dst)

def cross_compile(dst, folder_name=None):
    pass

def find_generate_prog(x):
    return {
        'mingw32-make' : "\"MinGW Makefiles\"",
        'nmake'        : "\"NMake Makefiles\"",
        'make'         : "\"Unix Makefiles\"",
        'ninja'        : "\"Ninja\""
    }[x]

def find_make_prog():
    if("Windows" in system()):
        if(which("NMAKE")):
            return ("\"NMake Makefiles\"", "NMAKE")
        elif(which("mingw32-make")):
            return ("\"MinGW Makefiles\"", "mingw32-make")
    elif("Linux" in system()):
        if(which("make")):
            return ("\"Unix Makefiles\"", "make")
    elif(which("Ninja")):
        return ("\"Ninja\"", "Ninja")

def main(dst, folder_name, make_prog):
    if not path.isabs(dst):
        dst = path.join(getcwd(), dst)
    if not path.isdir(dst):
        exit("Path specified doesn't exist")
    if not make_prog or not which(make_prog):
        make_program = find_make_prog()
    elif which(make_prog):
        make_program = (find_generate_prog(make_prog.lower()), make_prog)
   
    if not make_program:
        exit("Couldn't find Make program")
    print("Building")
    build(make_program[0], make_program[1], dst, folder_name) 

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Executes CMake with Makefile generator",prog="build",usage="%(prog)s [options]")
    parser.add_argument('p', nargs='?', help='Path to files for building', default=getcwd())
    parser.add_argument('-m', help='Makefile program')
    parser.add_argument('-f', help='Build folder name', default='build')
    args = parser.parse_args()

    main(args.p, args.f, args.m)