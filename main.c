#include <stdlib.h>
#include <stdio.h>
#include "testing.h"
#include "testing_capi.h"
#define NAME_LENGTH_MAX         63 //The currently longest variable name according to Mathworks (ref/namelengthmax)


//rtw-C-API nonsense defines
//Input
#define GetNumInputs            rtwCAPI_GetNumRootInputsFromStaticMap(testing_GetCAPIStaticMap())
#define GetInputs               rtwCAPI_GetRootInputsFromStaticMap(testing_GetCAPIStaticMap())
//Parameter
#define GetNumParameters        rtwCAPI_GetNumModelParametersFromStaticMap(testing_GetCAPIStaticMap())
#define GetParameters           rtwCAPI_GetModelParametersFromStaticMap(testing_GetCAPIStaticMap())
//Output
#define GetNumOutputs           rtwCAPI_GetNumRootOutputsFromStaticMap(testing_GetCAPIStaticMap())
#define GetOutputs              rtwCAPI_GetRootOutputsFromStaticMap(testing_GetCAPIStaticMap())





int main(int argc, const char *argv[]) {

    FILE *f = fopen("ModelOutputs.txt", "w");
    if(!f){
        printf("Error opening file");
        exit(13);
    }
    
    const int numRootInputs = GetNumInputs;
    const int numModelParameters = GetNumParameters;
    const int numRootOutputs = GetNumOutputs;
    char name[NAME_LENGTH_MAX] = "";
  

    fprintf(f, "Inputs");
    for(int i = 0; i < numRootInputs; i++){
        strcpy(name, GetInputs[i].blockPath);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }
    }
    fprintf(f, "\n");

    fprintf(f, "Parameters");
    for(int i = 0; i < numModelParameters; i++){
        strcpy(name, GetParameters[i].varName);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }

    }
    fprintf(f, "\n");

    fprintf(f,"Outputs");
    for(int i = 0; i < numRootOutputs; i++){
        strcpy(name, GetOutputs[i].blockPath);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }
    }
    fprintf(f, "\n");


    fclose(f);
    return 0;
}
