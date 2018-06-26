from pathlib import Path
from string import Template
from os import listdir, getcwd, path, chdir
import sys
import argparse

templateReplace = {
}

CONST_CAPI_UTILS_INCLUDE = '#include "{modelName}.h"\n'


#region Parser
parser = argparse.ArgumentParser(description="Generating a C file from a template",prog="cgen",usage="%(prog)s [options]")
parser.add_argument('Path', nargs=1, default='', help='Path for generated C file (Defaults to executing folder')
parser.add_argument('--MN', help='Specify the modelname (Defaults to DEFAULT)', type=str, default='DEFAULT')
parser.add_argument('--TI', help='Path to templates and includes folders', default='__file__')
parser.add_argument('--FN', help='Specify name for generated C file (Defaults to main.c)', default='main.c')
args = parser.parse_args()
#endregion

def genCAPI_utils():
    with open('test/capi_utils.c', 'w') as capiFile:
        with open('templates/LicenseTemplate','r') as licenseFile:
            capiFile.write(licenseFile.read())
        capiFile.write(CONST_CAPI_UTILS_INCLUDE.format(**templateReplace))
        with open('includes/capi_utils.c', 'r') as capiTemplate:
            capiFile.write(capiTemplate.read())

def genMain():
    with open('test/main.c','w') as mainFile:
        with open('templates/LicenseTemplate','r') as licenseFile:
            mainFile.write(licenseFile.read())
        with open('includes/mainIncludes','r') as mainIncludes:
            mainFile.write(mainIncludes.read().format(**templateReplace))
        with open('templates/ctemplate.c','r') as mainTemplate:
            mainFile.write(mainTemplate.read())

def copyFiles():
    pass

def main():
    templateReplace['modelName'] = args.MN
    #generatefile(args.TFP, args.Path[0], args.FN)
    genCAPI_utils()
    genMain()



if __name__ == "__main__":
    main()

