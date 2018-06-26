<<<<<<< HEAD
#include <stdlib.h>
#include <stdio.h>
#include "bil.h"
#include "bil_capi.h"
#define NAME_LENGTH_MAX         63 //The currently longest variable name according to Mathworks (ref/namelengthmax)


//rtw-C-API nonsense defines
//Input
#define GetNumInputs            rtwCAPI_GetNumRootInputsFromStaticMap(bil_GetCAPIStaticMap())
#define GetInputs               rtwCAPI_GetRootInputsFromStaticMap(bil_GetCAPIStaticMap())
//Parameter
#define GetNumParameters        rtwCAPI_GetNumModelParametersFromStaticMap(bil_GetCAPIStaticMap())
#define GetParameters           rtwCAPI_GetModelParametersFromStaticMap(bil_GetCAPIStaticMap())
//Output
#define GetNumOutputs           rtwCAPI_GetNumRootOutputsFromStaticMap(bil_GetCAPIStaticMap())
#define GetOutputs              rtwCAPI_GetRootOutputsFromStaticMap(bil_GetCAPIStaticMap())





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
=======
#include <stdlib.h>
#include <stdio.h>
#include "bil.h"
#include "bil_capi.h"
#define NAME_LENGTH_MAX         63 //The currently longest variable name according to Mathworks (ref/namelengthmax)


//rtw-C-API nonsense defines
//Input
#define GetNumInputs            rtwCAPI_GetNumRootInputsFromStaticMap(bil_GetCAPIStaticMap())
#define GetInputs               rtwCAPI_GetRootInputsFromStaticMap(bil_GetCAPIStaticMap())
//Parameter
#define GetNumParameters        rtwCAPI_GetNumModelParametersFromStaticMap(bil_GetCAPIStaticMap())
#define GetParameters           rtwCAPI_GetModelParametersFromStaticMap(bil_GetCAPIStaticMap())
//Output
#define GetNumOutputs           rtwCAPI_GetNumRootOutputsFromStaticMap(bil_GetCAPIStaticMap())
#define GetOutputs              rtwCAPI_GetRootOutputsFromStaticMap(bil_GetCAPIStaticMap())





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
>>>>>>> d6ee857bb1e58a317cbd04839cc777aa3ad883eb
