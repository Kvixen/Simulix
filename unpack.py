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
from os import listdir, getcwd, path, makedirs, walk, rename, environ
import xml.etree.ElementTree
import sys
import argparse
from shutil import copy2, copytree, copyfile, rmtree
import zipfile
import re
import tempfile

TEMPLATE_REPLACE = {
}

BAD_SOURCES = {
    "rt_main.c",
    "Simulix_exemain.c",
    "cJSON.c",
    "Simulix_capi_utils.c",
    "cJSON.h",
    "Simulix_capi_utils.h"
}

LIST_OF_SOURCES = []

#Match R2017b R2016a ect...
MAT_VERSION_PATTERN = re.compile(r"^(R20)\d\d(a|b)$")
#Match path/to/file/index.c path/to/other/file/func.h
SOURCE_OR_HEADER_PATTERN = re.compile(r"^(.)+(\.h|\.c)$")

GENERATED_CODE_LICENSE = "templates/Simulix_cg_license.txt"

# General functions

def generate_template_file(src, dst, template_file_name, file_name, license=False):
    with open(path.join(dst, file_name), 'w') as generated_file:
        if license:
            with open(path.join(src, GENERATED_CODE_LICENSE), 'r') as license_file:
                generated_file.write(license_file.read())
        with open(path.join(src, template_file_name), 'r') as template_file:
            generated_file.write(template_file.read().format(**TEMPLATE_REPLACE))

def find_file(dst, file_name):
    for root, dirs, files in walk(dst):
        if file_name in files:
            return path.join(root, file_name)
    return None

def add_definitions(defines_path, cmakelist_path):
    path_to_define = find_file(defines_path, "defines.txt")
    with open(path.join(cmakelist_path, "CMakeLists.txt"), 'r+') as cmake_file:
        content = cmake_file.read()
        cmake_file.seek(0,0)
        cmake_file.write('add_definitions(-DSIMULIX ')
        if path_to_define:
            with open(path_to_define, 'r+') as defines_file:
                defines_file.write("SIMULIX")
                for line in defines_file:
                    cmake_file.write("-D{0} ".format(line.rstrip('\r\n')))
        cmake_file.write(')\n' + content)

def copy_directory(src, dst):
    if not path.exists(dst):
        makedirs(dst)
    for item in listdir(src):
        s = path.join(src, item)
        d = path.join(dst, item)
        if path.isdir(s):
            copytree(s, d, False, None)
        else:
            copy2(s, d)

def make_directory(dst, dir_name):
    if not path.isdir(path.join(dst, dir_name)):
        makedirs(path.join(dst, dir_name))

# Zip Functions
def handle_zip(dst, zip_path):
    zip_ref = zipfile.ZipFile(zip_path, 'r')
    zip_ref.extractall(dst)
    root_dirs = []
    for f in zip_ref.namelist():
        r_dir = f.split('/')[0]
        if r_dir not in root_dirs:
            root_dirs.append(r_dir)
    zip_ref.close()
    for line in root_dirs:
        if MAT_VERSION_PATTERN.match(line):
            #environ['SIMX_MATLAB_VERSION']
            TEMPLATE_REPLACE['matlabVersion'] = line
        elif line != "otherFiles":
            #environ['SIMX_GEN_FOLDER_NAME'] = line
            TEMPLATE_REPLACE['folderName'] = line
            # The model name in a generated grt code is inside a folder called <model_name>_grt_rtw
            # Inside this zip there's only 2-3 folders. One of them is neither matching the pattern regex nor called "otherFiles"
            # This folder contains the mentioned folder above. When we find a directory following the rules above we do the following:
            # Look for directories in the above mentioned folder:
            # listdir(path.join(dst, line))[0]
            # This will produce a list with only one element which we will access. 
            # Next, we split it with '_' in order to get rid of _grt_rtw
            # And we join together all elements except the last two so we support model names with underscores.
            # '_'.join(<code-in-here>.split('_')[:-2])
            for folder_path in listdir(path.join(dst, line)):
                if path.isdir(path.join(dst, line, folder_path)):
                    modelName = '_'.join(folder_path.split('_')[:-2])
                    environ['SIMX_MODEL_NAME'] = modelName
                    TEMPLATE_REPLACE['modelName'] = modelName
                    break
            if TEMPLATE_REPLACE['folderName'] == TEMPLATE_REPLACE['modelName']:
                rename(path.join(dst, TEMPLATE_REPLACE['folderName']), path.join(dst, "old_" + TEMPLATE_REPLACE['folderName']))
                TEMPLATE_REPLACE['folderName'] = "old_" + TEMPLATE_REPLACE['folderName']
            TEMPLATE_REPLACE['modelNameS'] = TEMPLATE_REPLACE['modelName'][:28]

def generate_template_files(src, dst, temp_dst=None):
    if not temp_dst:
        temp_dst = dst
    generate_template_file(src, temp_dst, "templates/Simulix_capi_utils_template.c", "includes/Simulix_capi_utils.c", license=True)
    generate_template_file(src, temp_dst, "templates/Simulix_exemain_template.c", "Simulix_exemain.c", license=True)
    generate_template_file(src, dst, "templates/CMakeLists.txt", "CMakeLists.txt")

def handle_extension(extension_path, src, dst):
    if path.isfile(path.join(extension_path, "extension.py")):
        sys.path.append(extension_path)
        import extension
        try:
            TEMPLATE_REPLACE.update(extension.get_template_info(TEMPLATE_REPLACE))
        except AttributeError:
            pass
        if path.isdir(path.join(extension_path, "templates")):
            if path.isfile(path.join(extension_path, "templates/Simulix_exemain_template.c")):
                generate_template_file(extension_path, dst, "templates/Simulix_exemain_template.c", "Simulix_exemain.c")
            else:
                generate_template_file(src, dst,"templates/Simulix_exemain_template.c", "Simulix_exemain.c")

            if path.isfile(path.join(extension_path, "templates/CMakeLists.txt")):
                generate_template_file(extension_path, dst, "templates/CMakeLists.txt", "CMakeLists.txt")
            else:
                generate_template_file(src, dst, "templates/CMakeLists.txt", "CMakeLists.txt")

            if path.isfile(path.join(extension_path, "templates/Simulix_capi_utils_template.c")):
                generate_template_file(extension_path, dst, "templates/Simulix_capi_utils_template.c", "includes/Simulix_capi_utils.c")
            else:
                generate_template_file(src, dst, "templates/Simulix_capi_utils_template.c", "includes/Simulix_capi_utils.c")
        else:
            generate_template_files(src, dst)   
        return True
    else:
        return False
    return False

def copy_directories(src, dst):
    copy_directory(path.join(src, 'includes'), path.join(dst, 'includes'))
    copy_directory(path.join(src, 'libraryincludes'), path.join(dst, 'libraryincludes'))
    copy_directory(path.join(src, 'licenses'), path.join(dst, 'licenses'))

def generate_files(src, dst, zip_path, zip_name, extension_path, temp_path):
    """
    Extracts content from zip in zip_path
    Generates and copies neccesary files
    """
    environ['SIMX_EXE'] = "1"
    TEMPLATE_REPLACE['path'] = path.dirname(path.realpath(__file__)).replace('\\', '/')

    print("Generating files")
    handle_zip(temp_path, zip_path)
    copy_directories(src, temp_path)
    # Extensions will overwrite whatever handle_zip has put in the TEMPLATE_REPLACE
    # This can be useful and harmful so use extensions carefully
    extension = False
    if extension_path:
        extension = handle_extension(extension_path, src, dst) 
    if not extension or not extension_path:
        generate_template_files(src, dst, temp_path)
    add_definitions(temp_path, dst)
    extract_file_names(temp_path)
    # This is used by content-builder to add a list of sources to modelDescription.xml
    environ['SIMX_SOURCES_LIST'] = ";".join(LIST_OF_SOURCES)

def extract_file_names(dst):
    for root, dirs, files in walk(dst):
        for file in files:
            if not file in BAD_SOURCES and SOURCE_OR_HEADER_PATTERN.search(file):
                LIST_OF_SOURCES.append(path.join(root, file))
                
# Fmu Functions
def handle_fmu(dst, fmu_path):
    file_name, file_ext = path.splitext(fmu_path)
    zip_path = file_name + ".zip"
    print(zip_path)
    model_name = file_name.split('\\')[-1]
    TEMPLATE_REPLACE['modelName'] = model_name
    copyfile(fmu_path, zip_path)
    #rename(fmu_path, path.join("\\".join(fmu_path.split('\\')[:-1]), "old_" + fmu_path.split('\\')[-1]))
    zip_ref = zipfile.ZipFile(zip_path, 'r')
    zip_ref.extractall(path.join(dst, model_name))
    get_modeldescription_sources(dst)

def get_modeldescription_sources(dst):
    md_path = find_file(dst, 'modelDescription.xml')
    xml_root = xml.etree.ElementTree.parse(md_path).getroot()
    for atype in xml_root.findall('SourceFiles'):
        print(atype.get('File'))
    
def generate_files_fmu(src, dst, fmu_path, fmu_name, temp_path):
    TEMPLATE_REPLACE['path'] = path.dirname(path.realpath(__file__)).replace('\\', '/')
    if fmu_name.split('.')[-1] == "fmu":
        fmu_path = path.join(fmu_path, fmu_name)
    else:
        fmu_path = path.join(fmu_path, fmu_name + ".fmu")
    if not path.isfile(fmu_path):
        exit("Couldn't find the specified FMU file")
    handle_fmu(dst, fmu_path)
    generate_template_file(src, dst, 'templates/CMakeLists.txt', 'CMakeLists.txt')

# Main

def main():
    with tempfile.TemporaryDirectory() as dirpath:
        generate_files(args.t, args.p, args.zp, args.ZN, args.e, dirpath)  

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generating necessary files for an FMU",prog="unpack",usage="%(prog)s [options]")
    parser.add_argument('-p', default=getcwd(), help='Path for generated files')
    parser.add_argument('-t', help='Path to templates and includes folders', default=path.abspath(path.dirname(sys.argv[0])))
    parser.add_argument('ZN', nargs='?', help='Name of zipfile generated from matlab', default='default')
    parser.add_argument('-zp', help='Path to zipfile, if not executing folder', default=getcwd())
    parser.add_argument('-e', help='Path to extension')
    args = parser.parse_args()
    main()
