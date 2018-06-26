from pathlib import Path
from string import Template
from os import listdir, getcwd, path, chdir
import sys
import argparse

templateReplace = {
}


#region Parser
parser = argparse.ArgumentParser(description="Generating a C file from a template",prog="cgen",usage="%(prog)s [options]")
parser.add_argument('Path', nargs=1, default='', help='Path for generated C file (Defaults to executing folder')
parser.add_argument('--MN', help='Specify the modelname (Defaults to DEFAULT)', type=str, default='DEFAULT')
parser.add_argument('--TFP', help='Specify full path to template file (Defaults to ./ctemplate.c)', default='__file__')
parser.add_argument('--FN', help='Specify name for generated C file (Defaults to main.c)', default='main.c')
args = parser.parse_args()
#endregion


def generatefile(templatePath, filePath, fileName):
    '''

    '''
    if templatePath=="__file__":
        templatePath = path.join(path.dirname(path.abspath(__file__)), "ctemplate.c")
        if not path.isfile(templatePath):
            raise IOError("Template file doesn't exist!")
    elif not path.isfile(templatePath):
        raise IOError("Template file specified doesn't exist!")

    if filePath is not "" and not path.isdir(filePath):
        raise IOError("Specified path for generated C file is not directory")
    elif filePath is not "":
        chdir(filePath)
        
    with open(templatePath, "r") as tempf:
        with open(fileName, "w") as f:
            for line in tempf:
                #region Trycatch explanation (big)
                #This try catch is because sometimes format gets a ValueError because some syntax in C, like {} exists but make no sense
                #This is why we have a little 'hack' where we basiclly just wait until we crash, and If we do, we dont format that line.
                #endregion 
                try:
                    f.write(line.format(**templateReplace))
                except ValueError:
                    f.write(line)





def main():
    templateReplace['classname'] = args.MN
    generatefile(args.TFP, args.Path[0], args.FN)


if __name__ == "__main__":
    main()

