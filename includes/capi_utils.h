/*
Copyright (C) 2018 Scania

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

#define ROOT_INPUT_FLAG 1
#define MODEL_PARAMETER_FLAG 0
#define ROOT_OUTPUT_FLAG 2

struct ScalarVariable{
    char name[63];
    float value;
    uint8_T DataID;
    char type[50];
};
static const struct ScalarVariable EmptyStruct;


extern void GetValueFromAdress(const char_T*  paramName,
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
