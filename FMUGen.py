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

from cgen import generate_files
from build import build
import argparse
import time
from os import getcwd, path
import sys

compprog = '"MinGW Makefiles"'
makeprog = '"mingw32-make"'


def main():
    if not args.ONLY_BUILD:
        print("Generating files")
        print(args.Path, args.ZN)
        generate_files(args.TP, args.Path, args.ZP, args.ZN)
    print("Building")
    build(compprog, makeprog, args.Path)
    





if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generating and building step 1",prog="FMUGen",usage="%(prog)s [options]")
    parser.add_argument('--Path', default=getcwd(), help='Path for generated C file')
    parser.add_argument('--TP', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('ZN', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('--ZP', nargs='?', help='Path to zipfile, if not executing folder', default=getcwd())
    parser.add_argument('--ONLY_BUILD', help='Only build, dont make build directory', action='store_true')
    args = parser.parse_args()
    main()
