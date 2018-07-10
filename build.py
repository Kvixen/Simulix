"""
FGen generates an FMU from a simulink model source code.
 
Copyright (C) 2018 Scania and FGen contributors

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
from sys import exit


BUILD_ONLY = False

def _build(compile_program,make_program,dst):
    command = "cmake -G {0} ..".format(compile_program)
    print("Executing {0}".format(command))
    cmake_p = subprocess.Popen(command, stderr=subprocess.PIPE)
    cmake_errs = cmake_p.communicate()[1]
    if cmake_errs:
        print("Something went wrong with CMake. Error message = \n")
        print(cmake_errs)
        #handle_error(cmake_errs)
    command = "{0}".format(make_program)
    make_p = subprocess.Popen(command, stderr=subprocess.PIPE)
    make_errs = make_p.communicate()[1]
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
    _build(compprog, makeprog, dst)

def handle_error(err):
    err_msg = str(err).split(r'\n')
    if "CMake Error:" in err_msg[0]:
        print("CMake crashed with the following error:")
        err_msg_s = err_msg[0].split(':')[1][1:]
        if "generator" in err_msg_s:
            print(err_msg_s)
            print("Please use one of CMakes supported generators below: ")
            for i in range(3, len(err_msg)-1):
                print(err_msg[i])
            exit("Specified CMake generator was not found")
        elif "source directory" in err_msg_s:
            print(err)
            exit("The source directory {0} doesn't contain CMakeLists.txt".format(path.join(getcwd(),"..")))
    else:
        print("HANDLE ERROR NOT KNOWIGNG WHAT ERROR")
        print(err)
        exit("Unknown error, please post issue to Github")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generating a C file from a template",prog="cgen",usage="%(prog)s [options]")
    parser.add_argument('--Path', default=getcwd(), help='Path for generated C file')
    parser.add_argument('--G', help='Cmake supported makefile generator (Ex. MinGW Makefiles)', default="MinGW Makefiles")
    parser.add_argument('--M', help='Make program that supports generated makefile', default="mingw32-make")
    parser.add_argument('--FN', help='Build folder name', default='build')
    args = parser.parse_args()
    BUILD_ONLY = args.ONLY_BUILD
    #build(args.G, args.M, args.FN)



    
