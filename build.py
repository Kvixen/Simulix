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
    print(dst)
    dstlist = dst.split("\\")
    print(dstlist)
    chdir('..')
    for i, _ in enumerate(dstlist):
        chdir(dstlist[i])
    # mkdir("build")
    # chdir("build")
    command = "cmake -G {0}".format(compile_program)
    # command = "cmake -G {0} ..".format(compile_program)
    print("Executing {0}".format(command))
    p = subprocess.Popen(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    errs = p.communicate()[1]
    if errs:
        handle_error(errs)

    subprocess.call(make_program, shell=True)
    subprocess.Popen([r"main.exe", "call"], shell=True)

def prepare_build_directory(folderName):
    if not folderName:
        folderName = "build"
    if not path.isdir(path.join(getcwd(), folderName)) and getcwd().split('/')[-1:] != folderName:
        mkdir(folderName)
        chdir(folderName)
    elif getcwd().split('/')[-1:] != folderName:
        chdir(folderName)

    

def build(compprog, makeprog, CML, folderName=None):
    if not BUILD_ONLY:  
        prepare_build_directory(folderName)
    _build(compprog, makeprog, CML)

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
    parser.add_argument('Path', nargs='?', default=getcwd(), help='Path for generated C file')
    parser.add_argument('--G', help='Cmake supported makefile generator (Ex. MinGW Makefiles)', default="MinGW Makefiles")
    parser.add_argument('--M', help='Make program that supports generated makefile', default="mingw32-make")
    parser.add_argument('--FN', help='Build folder name', default='build')
    parser.add_argument('--ONLY_BUILD', help='Only build, dont make build directory', action='store_true')
    args = parser.parse_args()
    BUILD_ONLY = args.ONLY_BUILD
    build(args.G, args.M, args.FN)



    
