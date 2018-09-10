/*

Simulix generates an FMU from a simulink model source code.
 
Copyright (C) 2018 Scania CV AB and Simulix contributors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
As a special exception, you may create a larger work that contains
some or all of the template files included in Simulix and distribute
that work under terms of your choice, so long as that work isn't itself 
a FMU compiler using the template files.

This exception was added in version 1.0 of Simulix.

This special exception applies to files containing this special exception text.
The terms of the exception exists inside the text.

*/
#ifndef _CAPI_UTILS_H_
# define _CAPI_UTILS_H_

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "rtwtypes.h"
#include "rtw_modelmap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROOT_INPUT_FLAG 0
#define MODEL_PARAMETER_FLAG 1
#define ROOT_OUTPUT_FLAG 2

struct ScalarVariable{
    char name[63];
    char value[50];
    uint8_T DataID;
    int success;
};
static const struct ScalarVariable EmptyStruct;

extern void GetName(char* name);

extern void RemoveSpaces(char* source);

extern void GetValueFromAdress( char*           paramName,
                                void*          paramAddress,
                                uint8_T        slDataId,
                                unsigned short isComplex,
                                uint_T*        actDims,
                                uint_T         numDims,
                                real_T         slope,
                                real_T         bias);


extern void GetModelParameter(rtwCAPI_ModelMappingInfo* capiMap,  
                                      uint_T                    paramIdx);

extern void GetSignal(rtwCAPI_ModelMappingInfo* capiMap,  
                              uint_T                    signalIdx,
                              uint_T                    signalTypeFlag);

extern struct ScalarVariable GetVariable(rtwCAPI_ModelMappingInfo* capiMap,
                                   uint_T                    index,
                                   uint_T                    flag);


#ifdef __cplusplus
}
#endif

#endif
