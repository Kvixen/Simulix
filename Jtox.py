import xml.etree.cElementTree as ET
import json
import uuid
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
					k1_ele.set(k2, v2)
	return root
			
def build_XML_tree(data):

    step_size = data["StepSize"]
    name = data["Model"]
    guid = str(uuid.uuid4())

    fmiMD = {"fmiVersion":"2.0", "modelName":name, "guid":guid, "numberOfEventIndicators":"0"}
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

    return root


def main():
    data = read_json_file("ModelOutputs.json")

    XML_tree = build_XML_tree(data)

    with open('modelDescription.xml', 'w') as output:
        output.write(prettify(XML_tree))
