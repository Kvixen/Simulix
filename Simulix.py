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

from unpack import generate_files
from build import main as bm
import argparse
import time
from os import getcwd, path, listdir
import sys

def main(dst, make_prog, folder_name):
    if not args.ONLY_BUILD:
        print("Generating files")
        generate_files(args.t, args.p, args.zp, args.ZN)
    bm(dst, folder_name, make_prog,)
    





if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates an FMU from a Simulink model",prog="Simulix",usage="%(prog)s [options]")
    parser.add_argument('ZN', nargs='?', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('-p', default=getcwd(), help='Path for generated C file')
    parser.add_argument('-t', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('-zp', help='Path to zipfile', default=getcwd())
    parser.add_argument('-m', help='Makefile program')
    parser.add_argument('-f', help='Build folder name', default='build')
    parser.add_argument('--ONLY_BUILD', help='Only build, do not generate files', action='store_true')
    args = parser.parse_args()
    main(args.p, args.m, args.f)
