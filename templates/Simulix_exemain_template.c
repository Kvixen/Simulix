#include "{modelName}.h"
#include "{modelName}_capi.h"
#define MODEL_NAME "{modelName}"
#define REAL_TIME_MODEL {modelNameS}_M
#define INIT_MODEL {modelName}_initialize()
#define STEP_MODEL {modelName}_step()
#define TERMINATE_MODEL {modelName}_terminate()
#include <stdlib.h>
#include <stdio.h>
#include "Simulix_capi_utils.h"
#include "cJSON.h"
#define NAME_LENGTH_MAX             63 //The currently longest variable name according to Mathworks (ref/namelengthmax)

unsigned int numReal = 0;
unsigned int numInt = 0;
unsigned int numBoolean = 0;

#define GetNumInputs(mmi)            rtwCAPI_GetNumRootInputs(mmi)
#define GetInputs(mmi)               rtwCAPI_GetRootInputs(mmi)

#define GetNumModelParameters(mmi)   rtwCAPI_GetNumModelParameters(mmi)
#define GetModelParameters(mmi)      rtwCAPI_GetModelParameters(mmi)

#define GetNumBlockParameters(mmi)   rtwCAPI_GetNumBlockParameters(mmi)
#define GetBlockParameters(mmi)      rtwCAPI_GetBlockParameters(mmi)

#define GetNumOutputs(mmi)           rtwCAPI_GetNumRootOutputs(mmi)
#define GetOutputs(mmi)              rtwCAPI_GetRootOutputs(mmi)

static char *_getDataType(uint8_T index){{
    switch(index){{
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
    }}
    return "";
}}

#define GetDataType(num)              _getDataType(num)

static addObject(int flag, struct ScalarVariable* sVariable, char* causality, char* variability, char* initial, cJSON *ScalarVariables, cJSON *Unknown) {{
    char valueReference[15];
    char index[15];
    char name[512];
    cJSON *startArray;
    cJSON* jsonSV = cJSON_CreateObject();
    if (sVariable->nDims == 0) {{
        sprintf(name, "%s", sVariable->name);
    }}
    else if (sVariable->nDims == 1) {{
        sprintf(name, "%s[%u]", sVariable->name, sVariable->dims[0]);
    }}
    else if (sVariable->nDims == 2) {{
        sprintf(name, "%s[%u,%u]", sVariable->name, sVariable->dims[0], sVariable->dims[1]);
    }}
    cJSON_AddStringToObject(jsonSV, "name", name);
    cJSON_AddStringToObject(jsonSV, "causality", causality);
    cJSON_AddStringToObject(jsonSV, "description", sVariable->name);
    cJSON_AddStringToObject(jsonSV, "variability", variability);
    sprintf(index, "%u", sVariable->addrIdx);
    cJSON_AddStringToObject(jsonSV, "index", index);
    sprintf(index, "%u", sVariable->arrayIdx);
    cJSON_AddStringToObject(jsonSV, "offset", index);

    if (strcmp(GetDataType(sVariable->DataID), "Real") == 0) {{
        sprintf(valueReference, "%u", numReal++);
    }} else if (strcmp(GetDataType(sVariable->DataID), "Integer") == 0) {{
        sprintf(valueReference, "%u", numInt++);
    }} else {{
        sprintf(valueReference, "%u", numBoolean++);
    }}
    cJSON_AddStringToObject(jsonSV, "valueReference", valueReference);

    startArray = cJSON_AddArrayToObject(jsonSV, GetDataType(sVariable->DataID));

    if (flag != ROOT_OUTPUT_FLAG) {{
        cJSON *startObject;
        cJSON_AddStringToObject(jsonSV, "initial", initial);
        startObject = cJSON_CreateObject();
        cJSON_AddStringToObject(startObject, "start", sVariable->value);
        cJSON_AddItemToArray(startArray, startObject);
    }} else {{
        cJSON *outputObject;
        char outputIndex[15];
        if (strcmp(GetDataType(sVariable->DataID), "Real") == 0) {{
            sprintf(outputIndex, "%u", numReal);
        }} else if (strcmp(GetDataType(sVariable->DataID), "Integer") == 0) {{
            sprintf(outputIndex, "%u", numInt);
        }} else {{
            sprintf(outputIndex, "%u", numBoolean);
        }}
        outputObject = cJSON_CreateObject();
        cJSON_AddStringToObject(outputObject, "index", outputIndex);
        cJSON_AddItemToArray(Unknown, outputObject);
    }}
    cJSON_AddItemToArray(ScalarVariables, jsonSV);
}}

static void objectCreator(int flag, cJSON *root, cJSON *ScalarVariables, rtwCAPI_ModelMappingInfo* capiMap) {{
    unsigned int numVars, varIdx;
    struct ScalarVariable sVariable;
    cJSON *ModelStructure = NULL;
    cJSON *outputChildObject = NULL;
    cJSON *Outputs = NULL;
    cJSON *Unknown = NULL;
    cJSON *UnknownChildObject = NULL;
    char variability[10];
    char initial[10];
    char causality[15];

    switch(flag) {{
        case ROOT_INPUT_FLAG:
            numVars = GetNumInputs(capiMap);
            strcpy(variability, "discrete");
            strcpy(initial, "approx");
            strcpy(causality, "input");
            break;
        case MODEL_PARAMETER_FLAG:
            numVars = GetNumModelParameters(capiMap);
            strcpy(variability, "tunable");
            strcpy(initial, "exact");
            strcpy(causality, "parameter");
            break;
        case BLOCK_PARAMETER_FLAG:
            numVars = GetNumBlockParameters(capiMap);
            strcpy(variability, "tunable");
            strcpy(initial, "exact");
            strcpy(causality, "parameter");
            break;
        case ROOT_OUTPUT_FLAG:
            numVars = GetNumOutputs(capiMap);
            strcpy(variability, "discrete");
            strcpy(causality, "output");
            ModelStructure = cJSON_AddArrayToObject(root, "ModelStructure");
            outputChildObject = cJSON_CreateObject();
            Outputs = cJSON_AddArrayToObject(outputChildObject, "Outputs");
            UnknownChildObject = cJSON_CreateObject();
            Unknown = cJSON_AddArrayToObject(UnknownChildObject, "Unknown");
            break;
    }}

    #ifdef FDEBUG
    printf("Debug in Simulix_exemain.c, function objectCreator:\n\
    Number of %s    = %u,\n", flag == ROOT_INPUT_FLAG ? "inputs" : flag == ROOT_OUTPUT_FLAG ? "outputs" : "parameters", numVars);
    #endif

    for (varIdx = 0; varIdx < numVars; varIdx++) {{
        unsigned int arrayIdx = 0;
        do {{
            sVariable = GetVariable(capiMap, varIdx, flag, arrayIdx++);
            if (sVariable.success) {{
               addObject(flag, &sVariable, causality, variability, initial, ScalarVariables, Unknown);
            }}
        }} while (sVariable.success && sVariable.nDims);
    }}
    if (flag == ROOT_OUTPUT_FLAG) {{
        cJSON_AddItemToArray(Outputs, UnknownChildObject);
        cJSON_AddItemToArray(ModelStructure, outputChildObject);
    }}
}}

int main(int argc, const char *argv[]) {{

    INIT_MODEL;
    STEP_MODEL;

    {{
        rtwCAPI_ModelMappingInfo* capiMap = &(rtmGetDataMapInfo(REAL_TIME_MODEL).mmi);

        char name[NAME_LENGTH_MAX] = "";
        char *string = NULL;
        cJSON *ScalarVariables = NULL;
        char stepSize[15];

        cJSON *root = cJSON_CreateObject();
        sprintf(stepSize, "%f", (double) REAL_TIME_MODEL->Timing.stepSize0);

        {{
            cJSON *ModelVariablesObject = cJSON_CreateObject();
            ScalarVariables = cJSON_AddArrayToObject(ModelVariablesObject, "ScalarVariable");

            objectCreator(ROOT_INPUT_FLAG, root, ScalarVariables, capiMap);
            objectCreator(MODEL_PARAMETER_FLAG, root, ScalarVariables, capiMap);
            objectCreator(BLOCK_PARAMETER_FLAG, root, ScalarVariables, capiMap);
            objectCreator(ROOT_OUTPUT_FLAG, root, ScalarVariables, capiMap);
            {{
                cJSON *ModelVariables = cJSON_AddArrayToObject(root, "ModelVariables");
                cJSON_AddItemToArray(ModelVariables, ModelVariablesObject);
            }}
        }}

        //Information used in various python scripts
        cJSON_AddStringToObject(root, "Model", MODEL_NAME);
        cJSON_AddStringToObject(root, "StepSize", stepSize);
        cJSON_AddNumberToObject(root, "numReal", numReal);
        cJSON_AddNumberToObject(root, "numInt", numInt);
        cJSON_AddNumberToObject(root, "numBoolean", numBoolean);

        string = cJSON_Print(root);
        if (string == NULL) {{
            fprintf(stderr, "Failed to print cJSON.\n");
            exit(12);
        }}
        cJSON_Delete(root);

        {{
            FILE *f = fopen("ModelOutputs.json", "w");
            if(!f){{
                printf("Error opening file");
                exit(13);
            }}
            fprintf(f, "%s",string);
            fclose(f);
        }}
    }}

    TERMINATE_MODEL;

    return 0;
}}
