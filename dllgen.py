import json
import argparse
from os import getcwd, path

templateReplace = {

}

causalityDict = {
    'input':'U',
    'output':'Y',
    'parameter':'P'
}

def read_json_file(src):
    with open((src),'r') as json_file:
        data = json.load(json_file)
    return data



def main(dst, src):
    data = read_json_file("ModelOutputs.json")
    templateReplace['numReal'] = int(data['numReal']) + 1 #Add one because we dont count the real current_time until now
    templateReplace['numInt'] = int(data['numInt']) + 1
    templateReplace['numBoolean'] = int(data['numBoolean'])
    templateReplace['stepSize'] = data['StepSize']
    templateReplace['modelName'] = data['Model']
    modelName = data['Model']
    templateReplace['GUID'] = data['GUID']
    realString = ""
    intString = ""
    booleanString = ""
    for item in data['ModelVariables'][0]['ScalarVariable']:
        if 'Real' in item.keys():
            realString+= "{{F64, (void *)&{0}_{1}.{2}}},\n".format(modelName, causalityDict[item['causality']], item['name'])
        
        elif 'Integer' in item.keys():
            intString+= "{{S32, (void *)&{0}_{1}.{2}}},\n".format(modelName, causalityDict[item['causality']], item['name'])
        else:
            booleanString+= "{{B, (void *)&{0}_{1}.{2}}},\n".format(modelName, causalityDict[item['causality']], item['name'])
            
        
    templateReplace['realString'] = realString
    templateReplace['intString'] = intString
    templateReplace['booleanString'] = booleanString
    with open(path.join(dst, 'dllmain.c'), 'w') as dllmain:
        with open(src, 'r') as dllmainTemplate:
            dllmain.write(dllmainTemplate.read().format(**templateReplace))


            



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate a C file for a FMU",prog="dllgen",usage="%(prog)s [options]")
    parser.add_argument('-o', help='Path to dllmain.c output', default=getcwd())
    parser.add_argument('-t', help='Path to template file', default=path.join(path.dirname(path.realpath(__file__)), 'templates/dllmain.c'))
    args = parser.parse_args()
    main(args.o, args.t)

    