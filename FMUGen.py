from cgen import generate_files
from build import build
import argparse
from os import getcwd, path
import sys

compprog = '"MinGW Makefiles"'
makeprog = '"mingw32-make"'


def main():
    generate_files(args.TP, args.Path, path.join(args.ZP, args.ZN))
    build(compprog, makeprog)
    





if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generating and building step 1",prog="FMUGen",usage="%(prog)s [options]")
    parser.add_argument('Path', nargs='?', default=getcwd(), help='Path for generated C file')
    parser.add_argument('--TP', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('--ZN', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('--ZP', help='Path to zipfile, if not executing folder', default=getcwd())
    args = parser.parse_args()
    main()

