import json

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



def main():
    data = read_json_file("ModelOutputs.json")
    templateReplace['numReal'] = int(data['numReal']) + 1 #Add one because we dont count the real current_time until now
    templateReplace['numInt'] = int(data['numInt'])
    templateReplace['numIntP'] = int(data['numInt']) + 1 #Add one because we add a int in the new main file not existing in our model
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
            booleanString+= "{{B, (void *)&v{0}_{1}.{2}}},\n".format(modelName, causalityDict[item['causality']], item['name'])
            
        
    templateReplace['realString'] = realString
    templateReplace['intString'] = intString
    templateReplace['booleanString'] = booleanString
    with open("dllmain.c", "w") as dllmain:
        with open("C:/work/FGEN/templates/dllmain.c") as dllmainTemplate:
            dllmain.write(dllmainTemplate.read().format(**templateReplace))


            



if __name__ == '__main__':
    main()