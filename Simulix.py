#!/usr/bin/env python3
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

from unpack import generate_files, generate_files_fmu
from build import main as build, cross_compile
import argparse
import time
from os import getcwd, path
import sys
from platform import system

def main():
    if args.ONLY_BUILD and args.NO_MAKE and args.NO_CMAKE:
        exit("Can't run without building and making")

    if not args.ONLY_BUILD:
        if not args.FMU:
            print("Generating files")
            generate_files(args.t, args.p, args.zp, args.ZN, args.e)
        else:
            generate_files_fmu(args.t, args.p, args.zp, args.ZN)

    if not args.CC:
        build(args.p, args.f, args.m, args.NO_CMAKE, args.NO_MAKE)
    elif args.CC and "Linux" in system():
        # We need to somehow tell the program that if we're going to cross-compile we need to use a different CMakeLists
        # Or rewrite the one we currently have so we supply it with a default toolchain or no toolchain but it still works for all cases
        cross_compile(args.p, args.f)
    else:
        exit("Cross compiling is not supported in other systems than Linux based ones.")




if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates an FMU from a Simulink model",prog="Simulix",usage="%(prog)s [options]")
    parser.add_argument('ZN', nargs='?', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('-p', default=getcwd(), help='Path for generated C file')
    parser.add_argument('-t', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('-zp', help='Path to zipfile', default=getcwd())
    parser.add_argument('-m', help='Makefile program')
    parser.add_argument('-f', help='Build folder name', default='build')
    parser.add_argument('-e', help='Path to extension')
    parser.add_argument('--CC', help='Crosscompile (ALPHA! LINUX ONLY!)', action='store_true')
    parser.add_argument('--FMU', help='Use existing FMU instead of ZIP (All arguments are still supported)', action='store_true')
    parser.add_argument('--ONLY_BUILD', help='Only build, do not generate files', action='store_true')
    parser.add_argument('--NO_CMAKE', help='Don\'t execute CMAKE', action='store_false')
    parser.add_argument('--NO_MAKE', help='Don\'t execute MAKE program.', action='store_false')
    args = parser.parse_args()
    main()
