from pathlib import Path
from string import Template
from os import listdir, getcwd, path, chdir
import sys
import argparse

templateReplace = {
}


parser = argparse.ArgumentParser(description="Generating a C file from a template",prog="cgen",usage="%(prog)s [options]")
parser.add_argument('Path', nargs=1, default='', help='Path for generated C file (Defaults to executing folder')
parser.add_argument('--MN', help='Specify the modelname (Defaults to testing)', type=str, default='testing')
parser.add_argument('--TFP', help='Specify full path to template file (Defaults to ./ctemplate.c)', default='__file__')
parser.add_argument('--FN', help='Specify name for generated C file (Defaults to main.c)', default='main.c')

args = parser.parse_args()

def createfile(templatePath, filePath, fileName):
    if templatePath=="__file__":
        print(path.abspath(__file__))
        templatePath = path.join(path.dirname(path.abspath(__file__)), "ctemplate.c")
        print(templatePath)
        if not path.isfile(templatePath):
            raise IOError("Template file doesn't exist!")
    elif not path.isfile(templatePath):
        raise IOError("Template file specified doesn't exist!")

    if filePath is not "" and not path.isdir(filePath):
        #If user specified filePath but filePath is not directory, Throw error
        raise IOError("Specified path for generated C file is not directory")
    elif filePath is not "":
        #If user specified filePath and filePath is directory, join filePath and fileName
        #We dont need to check for file at this path, because we will generate it.
        chdir(filePath)
        
    with open(templatePath, "r") as tempf:
        with open(fileName, "w") as f:
            for line in tempf:
                try:
                    f.write(line.format(**templateReplace))
                except ValueError:
                    f.write(line)

""" def isdir(dirPath):
    if path.isdir(dirPath):
        return true:
    elif:

    return false """




def main():
    templateReplace['classname'] = args.MN
    createfile(args.TFP, args.Path[0], args.FN)


if __name__ == "__main__":
    main()



