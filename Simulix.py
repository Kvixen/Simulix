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
from build import main as build
from crosscompile import main as cross_compile
from pack import main as pack_fmu
import argparse
import time
from os import getcwd, path, mkdir, environ
import sys
from platform import system
from shutil import rmtree


def main():
    args.p = path.join(getcwd(), args.p)
    temp_folder_path = path.join(args.p, args.ez)
    # CMake outputs syntax er
    temp_folder_path = temp_folder_path.replace('\\', '/')
    environ['SIMX_TEMP_DIR'] = temp_folder_path

    if path.isdir(temp_folder_path):
        rmtree(temp_folder_path)
    mkdir(temp_folder_path)

    if args.ONLY_BUILD and not args.NO_MAKE and not args.NO_CMAKE:
        exit("Can't run without building and making")
    if not args.ONLY_BUILD:
        print("Generating files")
        if not args.FMU:
            generate_files(args.t, args.p, args.zp, args.ZN, args.e, temp_folder_path)
        else:
            generate_files_fmu(args.t, args.p, args.zp, args.ZN, temp_folder_path)
    build(args.p, args.f, args.m, args.NO_CMAKE, args.NO_MAKE)

    pack_fmu(args.p, path.join(args.p, environ['SIMX_MODEL_NAME']), environ['SIMX_MODEL_NAME'])
    if not args.NO_TEMP:
        rmtree(temp_folder_path)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates an FMU from a Simulink model",prog="Simulix",usage="%(prog)s [options]")
    parser.add_argument('ZN', nargs='?', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('-p', default=getcwd(), help='Path for generated FMU file')
    parser.add_argument('-t', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('-zp', help='Path to zipfile', default=getcwd())
    parser.add_argument('-m', help='Makefile program')
    parser.add_argument('-f', help='Build folder name', default='build')
    parser.add_argument('-e', help='Path to extension')
    parser.add_argument('-tf', help='Name of temp folder', default='temp_folder')
    #parser.add_argument('--CC', help='Crosscompile (ALPHA! LINUX ONLY!)', action='store_true')
    parser.add_argument('--NO-TEMP', help='Doesn\'t delete the temp folder', action='store_true')
    #parser.add_argument('--FMU', help='Use existing FMU instead of ZIP (All arguments are still supported)', action='store_true')
    parser.add_argument('--ONLY-BUILD', help='Only build, doesn\'t generate files', action='store_true')
    parser.add_argument('--NO-CMAKE', help='Doesn\'t execute CMAKE', action='store_false')
    parser.add_argument('--NO-MAKE', help='Doesn\'t execute MAKE program.', action='store_false')
    args = parser.parse_args()
    main()
