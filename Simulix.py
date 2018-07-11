"""
Simulix generates an FMU from a simulink model source code.
 
Copyright (C) 2018 Scania and Simulix contributors

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

from unpack import generate_files
from build import build
import argparse
import time
from os import getcwd, path, listdir
from shutil import which
from platform import system
import sys

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


def main(make_prog):
    if not path.isabs(args.p):
        args.p = path.join(getcwd(), args.p)
    if not path.isdir(args.p):
        exit("Path specified doesn't exist")
    if not args.ONLY_BUILD:
        print("Generating files")
        generate_files(args.t, args.p, args.zp, args.ZN)
    print("Building")
    print(make_prog)
    build(make_prog[0], make_prog[1], args.p)
    





if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates an FMU from a Simulink model",prog="Simulix",usage="%(prog)s [options]")
    parser.add_argument('ZN', nargs='?', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('-p', default=getcwd(), help='Path for generated C file')
    parser.add_argument('-t', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('-zp', help='Path to zipfile', default=getcwd())
    parser.add_argument('-m', help='Makefile program')
    parser.add_argument('--ONLY_BUILD', help='Only build, dont make build directory', action='store_true')
    args = parser.parse_args()
    if not args.m or not which(args.m):
        make_program = find_make_prog()
    elif which(args.m):
        make_program = (find_generate_prog(args.m.lower()), args.m) 
        
    if not make_program:
        exit("Couldn't find Make program")
    main(make_program)
