#include <stdlib.h>
#include <stdio.h>
#include "{classname}.h"
#include "{classname}_capi.h"
#define NAME_LENGTH_MAX         63 //The currently longest variable name according to Mathworks (ref/namelengthmax)


//rtw-C-API nonsense defines
//Input
#define GetNumInputs            rtwCAPI_GetNumRootInputsFromStaticMap({classname}_GetCAPIStaticMap())
#define GetInputs               rtwCAPI_GetRootInputsFromStaticMap({classname}_GetCAPIStaticMap())
//Parameter
#define GetNumParameters        rtwCAPI_GetNumModelParametersFromStaticMap({classname}_GetCAPIStaticMap())
#define GetParameters           rtwCAPI_GetModelParametersFromStaticMap({classname}_GetCAPIStaticMap())
//Output
#define GetNumOutputs           rtwCAPI_GetNumRootOutputsFromStaticMap({classname}_GetCAPIStaticMap())
#define GetOutputs              rtwCAPI_GetRootOutputsFromStaticMap({classname}_GetCAPIStaticMap())





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
    int i; //Will be used for loops
  

    fprintf(f, "Inputs");
    for(i = 0; i < numRootInputs; i++){
        strcpy(name, GetInputs[i].blockPath);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }
    }
    fprintf(f, "\n");

    fprintf(f, "Parameters");
    for(i = 0; i < numModelParameters; i++){
        strcpy(name, GetParameters[i].varName);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }

    }
    fprintf(f, "\n");

    fprintf(f,"Outputs");
    for(i = 0; i < numRootOutputs; i++){
        strcpy(name, GetOutputs[i].blockPath);
        if(name && strcmp("", name) != 0){
            fprintf(f, ",%s",name);
        }
    }
    fprintf(f, "\n");


    fclose(f);
    return 0;
}
>>>>>>> 42b4b35e8a69c8e6aff9f40454ad8df1d6652564
