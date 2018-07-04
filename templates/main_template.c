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
    cJSON *ModelStructure = NULL;
    cJSON *outputChildObject = NULL;
    cJSON *Outputs = NULL;
    cJSON *Unknown = NULL;
    cJSON *UnknownChildObject = NULL;
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
            ModelStructure = cJSON_AddArrayToObject(root, "ModelStructure");
            outputChildObject = cJSON_CreateObject();
            Outputs = cJSON_AddArrayToObject(outputChildObject, "Outputs");
            UnknownChildObject = cJSON_CreateObject();
            Unknown = cJSON_AddArrayToObject(UnknownChildObject, "Unknown");
            break;		
    }

    #ifdef FDEBUG
    printf("Debug in MAIN.c, function objectCreator:\n\
    Number of %s    = %i,\n", FLAG == ROOT_INPUT_FLAG ? "inputs" : FLAG == ROOT_OUTPUT_FLAG ? "outputs" : "parameters", number);
    #endif
    for (i=0; i < number; i++) {
        sVariable = GetVariable(capiMap, i, FLAG);
        jsonSV = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonSV, "name", sVariable.name);
        cJSON_AddStringToObject(jsonSV, "causality", causality);
        cJSON_AddStringToObject(jsonSV, "description", sVariable.name);
        cJSON_AddStringToObject(jsonSV, "variability", variability);

        if(strcmp(GetDataType(sVariable.DataID), "Real") == 0){
            sprintf(valueReference, "%i", numReal++);
        } else if (strcmp(GetDataType(sVariable.DataID), "Integer") == 0) {
            sprintf(valueReference, "%i", numInt++);
        } else {
            sprintf(valueReference, "%i", numBoolean++);
        }
        cJSON_AddStringToObject(jsonSV, "valueReference", valueReference);
        
        startArray = cJSON_AddArrayToObject(jsonSV, GetDataType(sVariable.DataID));

        if(FLAG != ROOT_OUTPUT_FLAG){
            cJSON_AddStringToObject(jsonSV, "initial", initial);
            startObject = cJSON_CreateObject();
            cJSON_AddStringToObject(startObject, "start", sVariable.value);
            cJSON_AddItemToArray(startArray, startObject);
        } else {
            if(strcmp(GetDataType(sVariable.DataID), "Real") == 0){
                sprintf(outputIndex, "%i", numReal);
            } else if (strcmp(GetDataType(sVariable.DataID), "Integer") == 0) {
                sprintf(outputIndex, "%i", numInt);
            } else {
                sprintf(outputIndex, "%i", numBoolean);
            }
            outputObject = cJSON_CreateObject();
            cJSON_AddStringToObject(outputObject, "index", outputIndex);
            cJSON_AddItemToArray(Unknown, outputObject);
        }
        cJSON_AddItemToArray(ScalarVariables, jsonSV); 
        if(FLAG == ROOT_OUTPUT_FLAG){
            cJSON_AddItemToArray(Outputs, UnknownChildObject);
            cJSON_AddArrayToObject(outputChildObject, "Derivatives");
            cJSON_AddArrayToObject(outputChildObject, "InitialUnknowns");
            cJSON_AddItemToArray(ModelStructure, outputChildObject);
        }
    }
}


int main(int argc, const char *argv[]) {

    INIT_MODEL;
    STEP_MODEL;
    rtwCAPI_ModelMappingInfo* capiMap = &(rtmGetDataMapInfo(REAL_TIME_MODEL).mmi);
    
    char name[NAME_LENGTH_MAX] = "";
    int i;
    char *string = NULL;
    cJSON *ScalarVariables = NULL;
    char stepSize[15];

    cJSON *root = cJSON_CreateObject();
    sprintf(stepSize, "%f", (double) REAL_TIME_MODEL->Timing.stepSize0);


    cJSON *ModelVariablesObject = cJSON_CreateObject();
    ScalarVariables = cJSON_AddArrayToObject(ModelVariablesObject, "ScalarVariable");

    objectCreator(ROOT_INPUT_FLAG, root, ScalarVariables, capiMap);
    objectCreator(MODEL_PARAMETER_FLAG, root, ScalarVariables, capiMap);
    objectCreator(ROOT_OUTPUT_FLAG, root, ScalarVariables, capiMap);
    cJSON *ModelVariables = cJSON_AddArrayToObject(root, "ModelVariables");
    cJSON_AddItemToArray(ModelVariables, ModelVariablesObject);

    //Information used in various python scripts
    cJSON_AddStringToObject(root, "Model", MODEL_NAME);
    cJSON_AddStringToObject(root, "StepSize", stepSize);
    cJSON_AddNumberToObject(root, "numReal", numReal);
    cJSON_AddNumberToObject(root, "numInt", numInt);
    cJSON_AddNumberToObject(root, "numBoolean", numBoolean);


    string = cJSON_Print(root);
    if (string == NULL) {
        fprintf(stderr, "Failed to print cJSON.\n");
    }
    cJSON_Delete(root);

    FILE *f = fopen("ModelOutputs.json", "w");
    if(!f){
        printf("Error opening file");
        exit(13);
    }
    fprintf(f, "%s",string);
    fclose(f);
    

    TERMINATE_MODEL;

    return 0;
}
