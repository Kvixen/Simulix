#include <stdlib.h>
#include <stdio.h>
#include "capi_utils.h"
#include "cJSON.h"
#define NAME_LENGTH_MAX             63 //The currently longest variable name according to Mathworks (ref/namelengthmax)



#define GetNumInputs(mmi)            rtwCAPI_GetNumRootInputs(mmi)
#define GetInputs(mmi)               rtwCAPI_GetRootInputs(mmi)

#define GetNumParameters(mmi)        rtwCAPI_GetNumModelParameters(mmi)
#define GetParameters(mmi)           rtwCAPI_GetModelParameters(mmi)

#define GetNumOutputs(mmi)           rtwCAPI_GetNumRootOutputs(mmi)
#define GetOutputs(mmi)              rtwCAPI_GetRootOutputs(mmi)

static const char *strings[] = { "SS_DOUBLE",
                                 "SS_SINGLE", 
                                 "SS_INT8", 
                                 "SS_UINT8",
                                 "SS_INT16",
                                 "SS_UINT16",
                                 "SS_INT32",
                                 "SS_UINT32",
                                 "SS_BOOLEAN"};

#define GetDataType(num)              strings[num]

void objectCreator(int FLAG, cJSON *ScalarVariables, rtwCAPI_ModelMappingInfo* capiMap) {
    
    int number, i;
    struct ScalarVariable sVariable;
    
    switch(FLAG) {
        case ROOT_INPUT_FLAG:
            number = GetNumInputs(capiMap);
            break;
        case MODEL_PARAMETER_FLAG:
            number = GetNumParameters(capiMap);
            break;
        case ROOT_OUTPUT_FLAG:
            number = GetNumOutputs(capiMap);
            break;		
    }
    
    for (i=0; i < number; i++) {
        sVariable = GetVariable(capiMap, i, FLAG);
        cJSON *ScalarVariable = cJSON_CreateObject();

        cJSON_AddStringToObject(ScalarVariable, "name", sVariable.name);
        cJSON_AddNumberToObject(ScalarVariable, "start", sVariable.value);
        cJSON_AddStringToObject(ScalarVariable, "type", GetDataType(sVariable.DataID));
        cJSON_AddStringToObject(ScalarVariable, "causality", sVariable.type);
        cJSON_AddItemToArray(ScalarVariables, ScalarVariable); 
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
    ScalarVariables = cJSON_AddArrayToObject(root, "ScalarVariable");

    objectCreator(ROOT_INPUT_FLAG, ScalarVariables, capiMap);
    objectCreator(ROOT_OUTPUT_FLAG, ScalarVariables, capiMap);
    objectCreator(MODEL_PARAMETER_FLAG, ScalarVariables, capiMap);

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
