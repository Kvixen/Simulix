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
from shutil import copy2, copytree, copyfile
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

GENERATED_CODE_LICENSE = "Simulix_cg_license.txt"

# General functions

def generate_template_file(src, dst, template_file_name, file_name, license=False):
    """
    Generates a template file using the .format function with the global variable TEMPLATE_REPLACE.
    string.Formatter.format (https://docs.python.org/3.4/library/string.html#string.Formatter.format).
    


    Args:
        src                 (str) : Source path of template_file_name
        dst                 (str) : Destination path of generated template file
       template_file_name   (str) : File name for template
       file_name            (str) : File name for generated file
       license              (bool): Boolean to include license (templates/Simulix_cg_license.txt). Defaults to False.

    Returns:
        None
    """
    with open(path.join(dst, file_name), 'w') as generated_file:
        if license:
            with open(path.join(src, GENERATED_CODE_LICENSE), 'r') as license_file:
                generated_file.write(license_file.read())
        with open(path.join(src, template_file_name), 'r') as template_file:
            generated_file.write(template_file.read().format(**TEMPLATE_REPLACE))

def find_file(dst, file_name):
    """
    Walks in directory and all sub-directories in dst to find file_name.

    Args:
        dst         (str): Path to recursive search
        file_name   (str): File name to look for

    Returns:
        Path for file if found else None
    """
    for root, dirs, files in walk(dst):
        if file_name in files:
            return path.join(root, file_name)
    return None

def add_definitions(defines_path, cmakelist_path):
    """
    Add definitions exported by Simulink which contains inside file defines.txt and adds it to the cmake script.
    Also adds definition SIMULIX for other functionallity inside the script (This was added for the planned repack FMU function).

    Args:
        defines_path    (str): Path where defines.txt exist
        cmake_list_path (str): Path where the CMakeLists exist

    Returns:
        None
    """
    path_to_define = find_file(defines_path, "defines.txt")
    if path_to_define:
        with open(path.join(cmakelist_path, "CMakeLists.txt"), 'r+') as cmake_file:
            content = cmake_file.read()
            cmake_file.seek(0,0)
            cmake_file.write('add_definitions(-DSIMULIX ')
            with open(path_to_define, 'r+') as defines_file:
                defines_file.write("SIMULIX")
                for line in defines_file:
                    cmake_file.write("-D{0} ".format(line.rstrip('\r\n')))
            cmake_file.write(')\n' + content)

def copy_directory(src, dst):
    """
    Copies directory src and all files within and puts it in dst.

    Args:
        src      (str): Directory to copy
        dst      (str): Destination path
        
    Returns:
        None
    """
    if not path.exists(dst):
        makedirs(dst)
    for item in listdir(src):
        s = path.join(src, item)
        d = path.join(dst, item)
        if path.isdir(s):
            copytree(s, d, False, None)
        else:
            copy2(s, d)

# Zip Functions
def handle_zip(dst, zip_path):
    """
    Unpacks a zip to destination and finds information about Matlab version and model name.

    Args:
        dst      (str): Destination path
        zip_path (str): A temporary destionation (If left None will be same as dst)

    Returns:
        None
    """
    zip_ref = zipfile.ZipFile(zip_path, 'r')
    zip_ref.extractall(dst)
    root_dirs = []
    for f in zip_ref.namelist():
        r_dir = f.split('/')[0]
        if r_dir not in root_dirs:
            root_dirs.append(r_dir)
    zip_ref.close()
    # Loop through the 2-3 folders inside the zip
    # The zip can contain the following folders where the first two are essential to Simulix
    # 20xxb or 20xxb where x is a number representing the current year (2018b)
    # model_name_grt_rtw (model_name is gonna be replaced with the name of the Simulink model)
    # otherFiles
    for line in root_dirs:
        if MAT_VERSION_PATTERN.match(line):
            TEMPLATE_REPLACE['matlabVersion'] = line
        elif line != "otherFiles":
            TEMPLATE_REPLACE['folderName'] = line
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
    """
    Generates template files C-api utilities and exemain to the temporary folder and a CMakeLists to the dst folder.

    Args:
        src      (str): Source path for the templates folder
        dst      (str): Destination path
        temp_dst (str): A temporary destionation (If left None will be same as dst)

    Returns:
        None
    """
    # All template files are inside templates directory
    src = path.join(src, "templates")
    if not temp_dst:
        temp_dst = dst
    generate_template_file(src, temp_dst, "Simulix_capi_utils_template.c", "includes/Simulix_capi_utils.c", license=True)
    generate_template_file(src, temp_dst, "Simulix_exemain_template.c", "Simulix_exemain.c", license=True)
    generate_template_file(src, dst, "CMakeLists.txt", "CMakeLists.txt")

def copy_directories(src, dst):
    """
    Copies directories includes, libraryincludes and licenses from source to destination

    Args:
        src (str): Source path for the above mentioned folders
        dst (str): Destination path

    Returns:
        None
    """
    copy_directory(path.join(src, 'includes'), path.join(dst, 'includes'))
    copy_directory(path.join(src, 'libraryincludes'), path.join(dst, 'libraryincludes'))
    copy_directory(path.join(src, 'licenses'), path.join(dst, 'licenses'))

def generate_files(src, dst, zip_path, zip_name, temp_path):
    """
    Extracts content from zip in zip_path
    Generates and copies neccesary files
    """
    environ['SIMX_EXE'] = "1"
    TEMPLATE_REPLACE['path'] = path.dirname(path.realpath(__file__)).replace('\\', '/')

    if zip_name.split('.')[-1] == "zip":
        zip_path = path.join(zip_path, zip_name)
    else:
        zip_path = path.join(zip_path, zip_name + ".zip")
    if not path.isfile(zip_path):
        exit("Couldn't find the specified ZIP file")
    handle_zip(temp_path, zip_path)
    copy_directories(src, temp_path)
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
