import xml.etree.cElementTree as ET
import json
import uuid
import argparse
from os import getcwd, path
from pprint import pprint
from xml.dom import minidom

def read_json_file(src):
	with open((src),'r') as json_file:
		data = json.load(json_file)
	return data

def prettify(elem):
	rough_string = ET.tostring(elem, encoding='utf-8')
	reparsed = minidom.parseString(rough_string)
	return reparsed.toprettyxml(indent="".join([' '] * 4))

def create_xml_subtree(root, name, dict_tree):
    if type(dict_tree) == list and not dict_tree:
        k1_ele = ET.SubElement(root, name)
    elif type(dict_tree) == list:
        for i in dict_tree:
            k1_ele = ET.SubElement(root, name)
            for k2, v2 in i.items():
                if type(v2) == list:
                    create_xml_subtree(k1_ele, k2, v2)
                else:
                    if k2 == 'start':
                        try:
                            v2 = str(float(v2))
                        except:
                            break
                    k1_ele.set(k2, v2)
    return root

def xmlgen(data):

    step_size = str(float(data["StepSize"]))
    name = data["Model"]

    fmiMD = {"fmiVersion":"2.0", "modelName":name, "guid":data["GUID"], "numberOfEventIndicators":"0"}
    CoSimulation_dict = {"providesDirectionalDerivative":"false", "modelIdentifier":name, "canHandleVariableCommunicationStepSize":"true"}
    category_dictlist = [{"name":"logAll", "description":"-"}, {"name":"logError", "description":"-"}, {"name":"logFmiCall", "description":"-"}, {"name":"logEvent", "description":"-"}]
    step_size_dict = {"stepSize":step_size}

    root = ET.Element("fmiModelDescription", fmiMD)
    ET.SubElement(root,"CoSimulation",CoSimulation_dict)
    
    LogCategories = ET.SubElement(root,"LogCategories")
    for i in range(len(category_dictlist)):
        ET.SubElement(LogCategories,"Category",category_dictlist[i])

    ET.SubElement(root, "DefaultExperiment", step_size_dict)
    create_xml_subtree(root, "ModelVariables", data['ModelVariables'])
    create_xml_subtree(root, "ModelStructure", data['ModelStructure'])

    with open(path.join('modelDescription.xml'), 'w') as output:
        output.write(prettify(root))


def dllgen(dst, src, data):

    templateReplace = {

    }

    causalityDict = {
        'input':'U',
        'output':'Y',
        'parameter':'P'
    }

    templateReplace['numReal'] = int(data['numReal']) + 1 #Add one because we dont count the real current_time until now
    templateReplace['numInt'] = int(data['numInt']) + 1
    templateReplace['numBoolean'] = int(data['numBoolean'])
    templateReplace['stepSize'] = data['StepSize']
    templateReplace['modelName'] = data['Model']
    templateReplace['GUID'] = data['GUID']
    modelName = data['Model']
    
    realString = ""
    intString = ""
    booleanString = ""

    for item in data['ModelVariables'][0]['ScalarVariable']:
        if 'Real' in item.keys():
            realString+= "{{F64, (void *)&{0}_{1}.{2}}},\n    ".format(modelName, causalityDict[item['causality']], item['name'])
        
        elif 'Integer' in item.keys():
            intString+= "{{S32, (void *)&{0}_{1}.{2}}},\n    ".format(modelName, causalityDict[item['causality']], item['name'])
        else:
            booleanString+= "{{B, (void *)&{0}_{1}.{2}}},\n    ".format(modelName, causalityDict[item['causality']], item['name'])
            
        
    templateReplace['realString'] = realString
    templateReplace['intString'] = intString
    templateReplace['booleanString'] = booleanString
    with open(path.join(dst, 'dllmain.c'), 'w') as dllmain:
        with open(src, 'r') as dllmainTemplate:
            dllmain.write(dllmainTemplate.read().format(**templateReplace))


def main(dst, src):
    data = read_json_file("ModelOutputs.json")
    data["GUID"] = str(uuid.uuid4())
    
    xmlgen(data)
    dllgen(dst, src, data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate a C file for a FMU",prog="dllgen",usage="%(prog)s [options]")
    parser.add_argument('-o', help='Path to dllmain.c output', default=getcwd())
    parser.add_argument('-t', help='Path to template file', default=path.join(path.dirname(path.realpath(__file__)), 'templates/dllmain_template.c'))
    args = parser.parse_args()
    main(args.o, args.t)
