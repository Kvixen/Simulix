from pathlib import Path
from os import listdir, getcwd, path, chdir, makedirs, walk
import sys
import argparse
from shutil import copy2, copytree
import zipfile
from glob import glob
import re

templateReplace = {
}


pattern = re.compile(r"^(R20)\d\d[a-b]")
#Includes for capi_utils.c. Only one line made it unnecessary for an entire file. If this gets appended, please make a template file for it instead
CONST_CAPI_UTILS_INCLUDE = '#include "{modelName}.h"\n'




def _gen_capi_utils(src, dst):
    with open(path.join(dst,"capi_utils.c"), 'w') as capiFile:
        with open(path.join(src,'LicenseTemplate'),'r') as licenseFile:
            capiFile.write(licenseFile.read())
        capiFile.write(CONST_CAPI_UTILS_INCLUDE.format(**templateReplace))
        with open(path.join(src,'capi_utils_template.c'), 'r') as capiTemplate:
            capiFile.write(capiTemplate.read())

def _gen_main(src, dst):
    with open(path.join(dst, 'main.c'),'w') as mainFile:
        with open(path.join(src,'LicenseTemplate'),'r') as licenseFile:
            mainFile.write(licenseFile.read())
        with open(path.join(src,'mainIncludes'),'r') as mainIncludes:
            mainFile.write(mainIncludes.read().format(**templateReplace))
        with open(path.join(src,'main_template.c'),'r') as mainTemplate:
            mainFile.write(mainTemplate.read())

def _copy_files(src, dst):

    for item in listdir(src):
        s = path.join(src, item)
        d = path.join(dst, item)
        if path.isdir(s):
            copytree(s, d, False, None)
        else:
            copy2(s,d)
    
def _gen_cmakelists(src, dst):
    with open(path.join(dst, 'CMakeLists.txt'),'w') as CMakeFile:
        with open(path.join(src,'CMakeListsTemplate.txt'),'r') as templateFile:
            CMakeFile.write(templateFile.read().format(**templateReplace))

def _handle_zip(dst, zipPath):
    zip_ref = zipfile.ZipFile(zipPath, 'r')
    zip_ref.extractall(dst)
    root_dirs = []
    for f in zip_ref.namelist():
        r_dir = f.split('/')
        r_dir = r_dir[0]
        if r_dir not in root_dirs:
            root_dirs.append(r_dir)
    zip_ref.close()
    for line in root_dirs:
        if pattern.match(line):
            templateReplace['matlabVersion'] = line
        elif line != "otherFiles":
            templateReplace['folderName'] = line
            tempList = listdir(line)[0].split('_')
            templateReplace['modelName'] = '_'.join(tempList[:-2])
            if len(templateReplace['modelName']) > 28:
                templateReplace['modelNameS'] = templateReplace['modelName'][:28]
            else:
                templateReplace['modelNameS'] = templateReplace['modelName']
        




def generate_files(src, dst, zipPath):
    """
    Extracts content from zip in zipPath
    Generates and copies neccesary files
    

    Parameters:
        src:
            Source of template files, should be 
            {path_to_cgen.py}/includes unless specified otherwise.

        dst:
            Destination of generated file(s).

        zipPath:
            Path to generated Zip.
    """
    _handle_zip(dst, zipPath)
    templatePath = path.join(src, 'templates')
    includeDst = path.join(dst, 'includes')
    includeSrc = path.join(src, 'includes')
    
    if not path.exists(includeDst):
        makedirs(includeDst)
    _copy_files(includeSrc, includeDst)
    _gen_capi_utils(templatePath, includeDst)
    _gen_main(templatePath, dst)
    _gen_cmakelists(templatePath, dst)


def main():
    generate_files(args.TP, args.Path, path.join(args.ZP, args.ZN))
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generating a C file from a template",prog="cgen",usage="%(prog)s [options]")
    parser.add_argument('Path', nargs='?', default=getcwd(), help='Path for generated C file')
    parser.add_argument('--TP', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('--ZN', help='Name of zipfile generated from matlab', default='default.zip')
    parser.add_argument('--ZP', help='Path to zipfile, if not executing folder', default=getcwd())
    args = parser.parse_args()
    main()

