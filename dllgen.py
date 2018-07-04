import json

templateReplace = {

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
    templateReplace['GUID'] = data['GUID']
    for item in data['ModelVariables'][0]['ScalarVariable'][0]:
        for key, value in item:



if __name__ == '__main__':
    main()