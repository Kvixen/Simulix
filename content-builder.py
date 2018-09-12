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
import lxml.etree as ET
import json
import uuid
import argparse
from os import getcwd, path, environ
import collections
import re

SOURCE_REXEG = re.compile(r"(\w+.c)$")

BAD_DLL_SOURCES = [
    "fmuTemplate.c"
]

def read_json_file(src):
    with open((src),'r') as json_file:
        data = json.load(json_file, object_pairs_hook=collections.OrderedDict)
    return data

def create_xml_subtree(root, name, dict_tree):
    if isinstance(dict_tree, list): #type(dict_tree) == list:
        if not dict_tree:
            k1_ele = ET.SubElement(root, name)
            return root
        for i in dict_tree:
            k1_ele = ET.SubElement(root, name)
            for k2, v2 in i.items():
                if isinstance(v2, list): #type(v2) == list:
                    create_xml_subtree(k1_ele, k2, v2)
                else:
                    if k2 == 'start':
                        try:
                            v2 = str(float(v2))
                        except ValueError:
                            continue
                    k1_ele.set(k2, v2)
    return root

def xmlgen(data):

    step_size = str(float(data["StepSize"]))
    name = data["Model"]

    fmiMD = {"fmiVersion":"2.0", "modelName":name, "guid":data["GUID"], "numberOfEventIndicators":"0"}
    co_simulation_dict = {"providesDirectionalDerivative":"false", "modelIdentifier":name, "canHandleVariableCommunicationStepSize":"false"}
    category_dictlist = [{"name":"logAll", "description":"-"}, {"name":"logError", "description":"-"}, {"name":"logFmiCall", "description":"-"}, {"name":"logEvent", "description":"-"}]
    step_size_dict = {"stepSize":step_size}

    root = ET.Element("fmiModelDescription", fmiMD)

    source_files_subelement = ET.SubElement(ET.SubElement(root,"CoSimulation",co_simulation_dict), "SourceFiles")
    # $ENV{SIMX_SOURCE_LIST} is set in unpack.py
    source_list = environ['SIMX_SOURCES_LIST'].split(';')
    source_list.append("Simulix_dllmain.c")
    for source in source_list:
        match = SOURCE_REXEG.search(source)
        if match and match.group(1) not in BAD_DLL_SOURCES:
            ET.SubElement(source_files_subelement, "File").set("name", match.group(1))

    LogCategories = ET.SubElement(root,"LogCategories")
    for i in range(len(category_dictlist)):
        ET.SubElement(LogCategories,"Category",category_dictlist[i])

    ET.SubElement(root, "DefaultExperiment", step_size_dict)
    create_xml_subtree(root, "ModelVariables", data['ModelVariables'])
    create_xml_subtree(root, "ModelStructure", data['ModelStructure'])

    ET.ElementTree(root).write(path.join('modelDescription.xml'),
        encoding='utf-8', xml_declaration=True, method='xml', pretty_print=True)

def dllgen(dst, src, data):

    template_replace = {

    }

    causality_dict = {
        'input':'U',
        'output':'Y',
        'parameter':'P'
    }

    template_replace['numReal'] = int(data['numReal'])
    template_replace['numInt'] = int(data['numInt'])
    template_replace['numBoolean'] = int(data['numBoolean'])
    template_replace['stepSize'] = data['StepSize']
    template_replace['modelName'] = data['Model']
    template_replace['GUID'] = data['GUID']
    model_name = data['Model']
    model_nameS = model_name[:29]

    real_string = ""
    int_string = ""
    boolean_string = ""

    for item in data['ModelVariables'][0]['ScalarVariable']:
        if 'Real' in item.keys():
            real_string+= "{{F64, {0}}},\n    ".format(item['index'])
        elif 'Integer' in item.keys():
            int_string+= "{{S32, {0}}},\n    ".format(item['index'])
        else:
            boolean_string+= "{{B, {0}}},\n    ".format(item['index'])

    template_replace['realString'] = real_string
    template_replace['intString'] = int_string
    template_replace['booleanString'] = boolean_string
    with open(path.join(dst, 'Simulix_dllmain.c'), 'w') as dllmain:
        with open(src, 'r') as dllmainTemplate:
            dllmain.write(dllmainTemplate.read().format(**template_replace))

def main(dst, src):
    data = read_json_file("ModelOutputs.json")
    data["GUID"] = str(uuid.uuid4())

    dllgen(dst, src, data)
    # remove index key that must not be known to xmlgen
    index_list = list(map(lambda d: d.pop('index'), data['ModelVariables'][0]['ScalarVariable']))
    xmlgen(data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate a C file for a FMU",prog="dllgen",usage="%(prog)s [options]")
    parser.add_argument('-o', help='Path to Simulix_dllmain.c output', default=getcwd())
    parser.add_argument('-t', help='Path to template file', default=path.join(path.dirname(path.realpath(__file__)), 'templates/Simulix_dllmain_template.c'))
    args = parser.parse_args()
    main(args.o, args.t)
