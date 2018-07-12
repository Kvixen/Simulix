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
    if not folder_name:
        folder_name = "build"
    if not path.isdir(path.join(dst, folder_name)) and dst.split('/')[-1:] != folder_name:
        mkdir(path.join(dst, folder_name))
        chdir(path.join(dst, folder_name))
    elif dst.split('/')[-1:] != folder_name:
        chdir(path.join(dst, folder_name))

    

def build(compprog, makeprog, dst, folder_name=None):  
    prepare_build_directory(dst, folder_name)
    execute_build_commands(compprog, makeprog, dst)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Executes CMake with Makefile generator",prog="build",usage="%(prog)s [options]")
    parser.add_argument('p', nargs='?', help='Path to files for building', default=getcwd())
    parser.add_argument('-g', help='Makefile generator', default="MinGW Makefiles")
    parser.add_argument('-m', help='Makefile program', default="mingw32-make")
    parser.add_argument('-f', help='Build folder name', default='build')
    args = parser.parse_args()
    build(args.g, args.m, args.p, args.f)



    
