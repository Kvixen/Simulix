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

struct ScalarVariable{
    char name[63];
    float value;
    uint8_T DataID;
    char type[50];
};
static const struct ScalarVariable EmptyStruct;
//signalTypeFlags
static const int MODEL_PARAMETER_FLAG = 0;
static const int ROOT_INPUT_FLAG = 1;
static const int ROOT_OUTPUT_FLAG = 2;

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
