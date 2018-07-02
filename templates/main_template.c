#include <stdlib.h>
#include <stdio.h>
#include "capi_utils.h"
#include "cJSON.h"
#define NAME_LENGTH_MAX             63 //The currently longest variable name according to Mathworks (ref/namelengthmax)

int numReal = 0;
int numInt = 0;
int numBoolean = 0;

#define GetNumInputs(mmi)            rtwCAPI_GetNumRootInputs(mmi)
#define GetInputs(mmi)               rtwCAPI_GetRootInputs(mmi)

#define GetNumParameters(mmi)        rtwCAPI_GetNumModelParameters(mmi)
#define GetParameters(mmi)           rtwCAPI_GetModelParameters(mmi)

#define GetNumOutputs(mmi)           rtwCAPI_GetNumRootOutputs(mmi)
#define GetOutputs(mmi)              rtwCAPI_GetRootOutputs(mmi)

char *_getDataType(uint8_T index){
    switch(index){
        case 0:
        case 1:
            return "Real";
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            return "Integer";
        case 8:
            return "Boolean";
    }
}

#define GetDataType(num)              _getDataType(num)

void objectCreator(int FLAG, cJSON *root, cJSON *ScalarVariables, rtwCAPI_ModelMappingInfo* capiMap) {
    
    int number, i;
    struct ScalarVariable sVariable;
    cJSON *jsonSV = NULL;
    cJSON *startArray = NULL;
    cJSON *startObject = NULL;
    cJSON *outputArray = NULL;
    cJSON *outputObject = NULL;
    char outputIndex[5];
    char valueReference[5];
    char variability[10];
    char initial[10];
    char causality[15];

    switch(FLAG) {
        case ROOT_INPUT_FLAG:
            number = GetNumInputs(capiMap);
            strcpy(variability, "discrete");
            strcpy(initial, "approx");
            strcpy(causality, "input");
            break;
        case MODEL_PARAMETER_FLAG:
            number = GetNumParameters(capiMap);
            strcpy(variability, "tunable");
            strcpy(initial, "exact");
            strcpy(causality, "parameter");
            break;
        case ROOT_OUTPUT_FLAG:
            number = GetNumOutputs(capiMap);
            strcpy(variability, "discrete");
            strcpy(causality, "output");
            outputarray = cJSON_AddArrayToObject(cJSON_AddArrayToObject(root, "ModelStructure"), "Outputs");
            break;		
    }

    for (i=0; i < number; i++) {
        sVariable = GetVariable(capiMap, i, FLAG);
        jsonSV = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonSV, "name", sVariable.name);
        cJSON_AddStringToObject(jsonSV, "causality", causality);
        cJSON_AddStringToObject(jsonSV, "description", sVariable.name);
        cJSON_AddStringToObject(jsonSV, "variability", variability);
        startArray = cJSON_AddArrayToObject(jsonSV, GetDataType(sVariable.DataID));
        if(strcmp(GetDataType(sVariable.DataID), "Real") == 0){
            sprintf(valueReference, "%i", numReal++);
        } else if (strcmp(GetDataType(sVariable.DataID), "Integer") == 0) {
            sprintf(valueReference, "%i", numInt++);
        } else {
            sprintf(valueReference, "%i", numBoolean++);
        }
        cJSON_AddStringToObject(jsonSV, "valueReference", valueReference);
        
        if(FLAG != ROOT_OUTPUT_FLAG){
            cJSON_AddStringToObject(jsonSV, "initial", initial);
            startObject = cJSON_CreateObject();
            cJSON_AddStringToObject(startObject, "start", sVariable.value);
            cJSON_AddItemToArray(startArray, startObject);
        } else {
            if(strcmp(GetDataType(sVariable.DataID), "Real") == 0){
                sprintf(outputIndex, "%i", numReal++);
            } else if (strcmp(GetDataType(sVariable.DataID), "Integer") == 0) {
                sprintf(outputIndex, "%i", numInt++);
            } else {
                sprintf(outputIndex, "%i", numBoolean++);
            }
            outputObject = cJSON_CreateObject();
            cJSON_AddStringToObject(outputObject, "Unknown", outputIndex);
            cJSON_AddItemToArray(outputArray, outputObject);
        }
        cJSON_AddItemToArray(ScalarVariables, jsonSV); 
        if(FLAG = ROOT_OUTPUT_FLAG){
            cJSON_AddItemToArray(root, outputArray);
        }
    }
}


int main(int argc, const char *argv[]) {

    INIT_MODEL;
    STEP_MODEL;
    rtwCAPI_ModelMappingInfo* capiMap = &(rtmGetDataMapInfo(REAL_TIME_MODEL).mmi);
    


    FILE *f = fopen("ModelOutputs.json", "w");
    if(!f){
        printf("Error opening file");
        exit(13);
    }
    
    char name[NAME_LENGTH_MAX] = "";
    int i;
    char *string = NULL;
    cJSON *ScalarVariables = NULL;

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, MODEL_NAME);
    ScalarVariables = cJSON_AddArrayToObject(root, "ScalarVariable");

    objectCreator(ROOT_INPUT_FLAG, root, ScalarVariables, capiMap);
    objectCreator(ROOT_OUTPUT_FLAG, root, ScalarVariables, capiMap);
    objectCreator(MODEL_PARAMETER_FLAG, root, ScalarVariables, capiMap);

    string = cJSON_Print(root);
    if (string == NULL) {
        fprintf(stderr, "Failed to print cJSON.\n");
    }

    cJSON_Delete(root);
    fprintf(f, "%s",string);
    

    TERMINATE_MODEL;

    fclose(f);
    return 0;
}
