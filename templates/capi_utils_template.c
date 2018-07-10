/*
FGen generates an FMU from a simulink model source code.
 
Copyright (C) 2018 Scania and FGen contributors

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
#include "{modelName}.h"
#include <stddef.h>
#include <assert.h>
#include "capi_utils.h"
#include <string.h>


struct ScalarVariable sVariable;

void GetName(char *name){{
    const char s[2] = "/";
    char *token;
   
    token = strtok(name, s);
   
   while( token != NULL ) {{
      strcpy(name, token);
      token = strtok(NULL, s);
   }}
}}

void RemoveSpaces(char* source){{
    char* i = source;
    char* j = source;
    while(*j != 0)
    {{
      *i = *j++;
      if(*i != ' ')
        i++;
    }}
    *i = 0;
}}

void GetValueFromAdress( char           *paramName,
                         void*          paramAddress,
                         uint8_T        slDataID,
                         unsigned short isComplex,
                         uint_T*        actualDims,
                         uint_T         numDims,
                         real_T         slope,
                         real_T         bias) {{
    
    strcpy(sVariable.name, paramName);
    sVariable.DataID = slDataID;

    uint_T numRows  = actualDims[0];
    uint_T numCols  = actualDims[1];
    uint_T numPages = 0;
    int rowIdx;
    int colIdx;
    int pageIdx;
    bool _set = false;
    if (numDims == 3) numPages = actualDims[2];
    #ifdef FDEBUG
    printf("Debug in CAPI_UTILS.c, function GetValueFromAdress:\n\
    Parameter Name = %s,\n\
    slDataID       = %i,\n\
    isComplex      = %s,\n\
    numRows        = %i,\n\
    numCols        = %i,\n\
    numPages       = %i,\n\
    slope          = %f,\n\
    bias           = %f\n", paramName, slDataID, isComplex ? "true" : "false" ,numRows,numCols, numPages, slope, bias);
    #endif

    switch(slDataID) {{
        case SS_DOUBLE :
        {{
            real_T* paramVal = (real_T *) paramAddress;
            sprintf(sVariable.value, "%f", paramVal[0]);
            break;
        }}
        case SS_SINGLE :
        {{
            real32_T* paramVal = (real32_T *) paramAddress;
            sprintf(sVariable.value, "%f", paramVal[0]);
            break;
        }}
        case SS_INT8:
        {{
            int8_T* paramVal = (int8_T *) paramAddress;
            int8_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}
        case SS_UINT8:
        {{
            uint8_T* paramVal = (uint8_T *) paramAddress;
            uint8_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}
        case SS_INT16:
        {{
            int16_T* paramVal = (int16_T *) paramAddress;
            int16_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}
        case SS_UINT16:
        {{
            uint16_T* paramVal = (uint16_T *) paramAddress;
            uint16_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}
        case SS_INT32:
        {{
            int32_T* paramVal = (int32_T *) paramAddress;
            int32_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}

        case SS_UINT32:
        {{
            uint32_T* paramVal = (uint32_T *) paramAddress;
            uint32_T intVal  = paramVal[0];
            real_T  realVal = slope*(intVal) + bias;
            sprintf(sVariable.value, "%d", realVal);
            break;
        }}
        case SS_BOOLEAN:
        {{
            boolean_T* paramVal = (boolean_T *) paramAddress;
            sprintf(sVariable.value, "%s", paramVal[0] ? "True" : "False");
            break;
        }}
        default:
            strcpy(sVariable.name, "Type not handled, CAPI_UTILS ERROR 2");
    }}
    return;
}}

void GetModelParameter(rtwCAPI_ModelMappingInfo* capiMap,
                        uint_T                    paramIdx) {{
    
    const rtwCAPI_ModelParameters* modelParams;
    const rtwCAPI_DataTypeMap*     dataTypeMap;
    const rtwCAPI_DimensionMap*    dimMap;
    const rtwCAPI_FixPtMap*        fxpMap;
    const uint_T*                  dimArray;
    void**                         dataAddrMap;

    char_T* paramName;
    uint_T        addrIdx;
    uint16_T      dataTypeIdx;
    uint16_T      dimIndex;
    uint16_T      fxpMapIdx;
    uint8_T       slDataID;

    void*                paramAddress;
    const char_T*        cDataName;
    unsigned short       isComplex;
    uint8_T              numDims;
    uint_T               dimArrayIdx;
    rtwCAPI_Orientation  orientation;
    uint_T*              actualDimensions;

    int    idx;
    real_T slope = 1.0;
    real_T bias  = 0.0;
    unsigned short modParamFlag = 0; 

    /* Assert the parameter index is less than total number of parameters */
    assert(paramIdx < rtwCAPI_GetNumModelParameters(capiMap));

    /* Get modelParams, an array of rtwCAPI_ModelParameters structure  */
    modelParams = rtwCAPI_GetModelParameters(capiMap);
    if (modelParams == NULL) return;
    
    /* Get Parameter Name */
    paramName   = rtwCAPI_GetModelParameterName(modelParams, paramIdx); 

    /* Get Data Type attributes of the Model Parameter                       */
    /* Accessing the data type information from capiMap is a 3 step process  *
     * 1) Get the dataTypeMap Array from capiMap.                            *
     * 2) Get the index into the above array, dataTypeIdx, from ModelParameter*
     *    structure. The index will point to a rtwCAPI_DataTypeMap structure.*
     * 3) From the structure, get the member values of the structure, namely *
     *       o Simulink Data ID, can be one of the enumerated value          *
     *           SS_DOUBLE, SS_SINGLE, SS_INT8, SS_UINT8, SS_INT16, SS_UINT16*
     *            , SS_INT32, SS_UINT32, SS_BOOLEAN                          *
     *       o Complexity                                                    *
     *       o Data Size                                                     *
     *    For complete structure see matlabroot/rtw/c/src/rtw_capi.h         */

    dataTypeMap = rtwCAPI_GetDataTypeMap(capiMap);
    if (dataTypeMap == NULL) return;
    
    dataTypeIdx = rtwCAPI_GetModelParameterDataTypeIdx(modelParams, paramIdx);
    slDataID    = rtwCAPI_GetDataTypeSLId(dataTypeMap, dataTypeIdx);
    cDataName   = rtwCAPI_GetDataTypeCName(dataTypeMap,dataTypeIdx);
    isComplex   = rtwCAPI_GetDataIsComplex(dataTypeMap,dataTypeIdx);

    /* Get Dimensions of the Model Parameter                                 */
    /* Accessing the dimension from capiMap is a 4 step process              *
     * 1) Get the dimMap array, an array of CAPI_DimensionMap struct         *
     * 2) Get the dimArray array, an array of all the dimensions in the Map  *
     * 3) Get the index into the dimMap array (dimIdx) from ModelParameter   *
     *    structure. the index will point to rtwCAPI_Dimension structure     *
     * 4) From the rtwCAPI_Dimension structure, get the following information*
     *       o orientation (scalar | vector | matrix)                        *
     *       o Number of dimensions (numDims)                                *
     *       o index into the dimArray (dimArrayIdx)                         *
     * 5) Using numDims and the dimArrayIdx, get the actual dimensions from  *
     *    the dimArray                                                       *
     *       uint_T ActualDims[numDims] = {{dimArray[dimArrayIdx],            *
     *                                     dimArray[dimArrayIdx+1],          *
     *				       ...                                   *
     *				       dimArray[dimArrayIdx+(numDims-1)]}}    *
     *    For e.g, scalar and 2-D parameters will have numDims = 2, So       *
     *       uint_T ActualDims[2] = {{dimArray[dimArrayIdx],                  *
     *                               dimArray[dimArrayIdx +1]}}               */
    
    dimMap   = rtwCAPI_GetDimensionMap(capiMap);
    dimArray = rtwCAPI_GetDimensionArray(capiMap);

    if ((dimMap == NULL) || (dimArray == NULL)) return;

    dimIndex    = rtwCAPI_GetModelParameterDimensionIdx(modelParams, paramIdx);
    numDims     = rtwCAPI_GetNumDims(dimMap, dimIndex); 
    dimArrayIdx = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    orientation = rtwCAPI_GetOrientation(dimMap, dimIndex);

    actualDimensions = (uint_T *) malloc(numDims*sizeof(uint_T));
    for(idx=0; idx < numDims; idx++) {{
        actualDimensions[idx] = dimArray[dimArrayIdx + idx];
    }}
    
    /* Get fixed-point information of the parameter */
    fxpMap = rtwCAPI_GetFixPtMap(capiMap);
    if (fxpMap == NULL) return;
    
    fxpMapIdx = rtwCAPI_GetModelParameterFixPtIdx(modelParams, paramIdx);
    if(fxpMapIdx > 0) {{
        /* Only Fixed-point parameters have fxpMapIdx > 0 */
        real_T fracslope = rtwCAPI_GetFxpFracSlope(fxpMap,fxpMapIdx);
        int8_T expt      = rtwCAPI_GetFxpExponent(fxpMap,fxpMapIdx);
        
        /* slope = fractional slope * 2^exponent 
         * fractional slope is also called Slope Adjustment Factor. 
         * Type "help fixdt" in MATLAB command window for more information
         * on fixed point data types  
         */
        
        slope = fracslope*pow(2.0,expt);
        bias  = rtwCAPI_GetFxpBias(fxpMap,fxpMapIdx);
    }}

    /* Get the address to this parameter                                     */
    dataAddrMap = rtwCAPI_GetDataAddressMap(capiMap);
    addrIdx     = rtwCAPI_GetModelParameterAddrIdx(modelParams,paramIdx);
    paramAddress= (void *) rtwCAPI_GetDataAddress(dataAddrMap,addrIdx); 

    if (paramAddress == NULL) return;
    
    GetValueFromAdress(paramName, paramAddress, slDataID, isComplex,
                        actualDimensions, numDims, slope, bias);

    free(actualDimensions);
    return;
}}

void GetSignal(rtwCAPI_ModelMappingInfo* capiMap,
                uint_T                    signalIdx,
                uint_T                    signalTypeFlag) {{
    
    const rtwCAPI_Signals*         signals;
    const rtwCAPI_DataTypeMap*     dataTypeMap;
    const rtwCAPI_DimensionMap*    dimMap;
    const rtwCAPI_FixPtMap*        fxpMap;
    const uint_T*                  dimArray;
    void**                         dataAddrMap;

    char *signalName = malloc(256);
    uint_T        addrIdx;
    uint16_T      dataTypeIdx;
    uint16_T      dimIndex;
    uint16_T      fxpMapIdx;
    uint8_T       slDataID;

    void*                paramAddress;
    const char_T*        cDataName;
    unsigned short       isComplex;
    uint8_T              numDims;
    uint_T               dimArrayIdx;
    rtwCAPI_Orientation  orientation;
    uint_T*              actualDimensions;

    int    idx;
    real_T slope = 1.0;
    real_T bias  = 0.0;
    unsigned short modParamFlag = 0; 

    switch(signalTypeFlag)
    {{
        case ROOT_INPUT_FLAG:
        /* Assert the signal index is less than total number of signals */
            assert(signalIdx < rtwCAPI_GetNumRootInputs(capiMap));
            signals = rtwCAPI_GetRootInputs(capiMap);
            break;
        case ROOT_OUTPUT_FLAG:
        /* Assert the signal index is less than total number of signals */
            assert(signalIdx < rtwCAPI_GetNumRootOutputs(capiMap));
            signals = rtwCAPI_GetRootOutputs(capiMap);
            break;
    }}

    if(signals == NULL) return;
    
    /* Get Signal Name. This has to be done via the SignalBlockPath */
    strcpy(signalName, rtwCAPI_GetSignalBlockPath(signals, signalIdx));
    GetName(signalName);
    RemoveSpaces(signalName);


    /* Get Data Type attributes of the Model Parameter                       */
    /* Accessing the data type information from capiMap is a 3 step process  *
     * 1) Get the dataTypeMap Array from capiMap.                            *
     * 2) Get the index into the above array, dataTypeIdx, from ModelParameter*
     *    structure. The index will point to a rtwCAPI_DataTypeMap structure.*
     * 3) From the structure, get the member values of the structure, namely *
     *       o Simulink Data ID, can be one of the enumerated value          *
     *           SS_DOUBLE, SS_SINGLE, SS_INT8, SS_UINT8, SS_INT16, SS_UINT16*
     *            , SS_INT32, SS_UINT32, SS_BOOLEAN                          *
     *       o Complexity                                                    *
     *       o Data Size                                                     *
     *    For complete structure see matlabroot/rtw/c/src/rtw_capi.h         */

    dataTypeMap = rtwCAPI_GetDataTypeMap(capiMap);
    if (dataTypeMap == NULL) return;
    
    dataTypeIdx = rtwCAPI_GetSignalDataTypeIdx(signals, signalIdx);
    slDataID    = rtwCAPI_GetDataTypeSLId(dataTypeMap, dataTypeIdx);
    cDataName   = rtwCAPI_GetDataTypeCName(dataTypeMap,dataTypeIdx);
    isComplex   = rtwCAPI_GetDataIsComplex(dataTypeMap,dataTypeIdx);

    /* Get Dimensions of the Model Parameter                                 */
    /* Accessing the dimension from capiMap is a 4 step process              *
     * 1) Get the dimMap array, an array of CAPI_DimensionMap struct         *
     * 2) Get the dimArray array, an array of all the dimensions in the Map  *
     * 3) Get the index into the dimMap array (dimIdx) from ModelParameter   *
     *    structure. the index will point to rtwCAPI_Dimension structure     *
     * 4) From the rtwCAPI_Dimension structure, get the following information*
     *       o orientation (scalar | vector | matrix)                        *
     *       o Number of dimensions (numDims)                                *
     *       o index into the dimArray (dimArrayIdx)                         *
     * 5) Using numDims and the dimArrayIdx, get the actual dimensions from  *
     *    the dimArray                                                       *
     *       uint_T ActualDims[numDims] = {{dimArray[dimArrayIdx],            *
     *                                     dimArray[dimArrayIdx+1],          *
     *				       ...                                   *
     *				       dimArray[dimArrayIdx+(numDims-1)]}}    *
     *    For e.g, scalar and 2-D parameters will have numDims = 2, So       *
     *       uint_T ActualDims[2] = {{dimArray[dimArrayIdx],                  *
     *                               dimArray[dimArrayIdx +1]}}               */
    
    dimMap   = rtwCAPI_GetDimensionMap(capiMap);
    dimArray = rtwCAPI_GetDimensionArray(capiMap);

    if ((dimMap == NULL) || (dimArray == NULL)) return;

    dimIndex    = rtwCAPI_GetSignalDimensionIdx(signals, signalIdx);
    numDims     = rtwCAPI_GetNumDims(dimMap, dimIndex); 
    dimArrayIdx = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    orientation = rtwCAPI_GetOrientation(dimMap, dimIndex);

    actualDimensions = (uint_T *) malloc(numDims*sizeof(uint_T));
    for(idx=0; idx < numDims; idx++) {{
        actualDimensions[idx] = dimArray[dimArrayIdx + idx];
    }}
    
    /* Get fixed-point information of the parameter */
    fxpMap = rtwCAPI_GetFixPtMap(capiMap);
    if (fxpMap == NULL) return;
    
    fxpMapIdx = rtwCAPI_GetSignalFixPtIdx(signals, signalIdx);
    if(fxpMapIdx > 0) {{
        /* Only Fixed-point parameters have fxpMapIdx > 0 */
        real_T fracslope = rtwCAPI_GetFxpFracSlope(fxpMap,fxpMapIdx);
        int8_T expt      = rtwCAPI_GetFxpExponent(fxpMap,fxpMapIdx);
        
        /* slope = fractional slope * 2^exponent 
         * fractional slope is also called Slope Adjustment Factor. 
         * Type "help fixdt" in MATLAB command window for more information
         * on fixed point data types  
         */
        
        slope = fracslope*pow(2.0,expt);
        bias  = rtwCAPI_GetFxpBias(fxpMap,fxpMapIdx);
    }}

    /* Get the address to this parameter                                     */
    dataAddrMap = rtwCAPI_GetDataAddressMap(capiMap);
    addrIdx     = rtwCAPI_GetSignalAddrIdx(signals,signalIdx);
    paramAddress= (void *) rtwCAPI_GetDataAddress(dataAddrMap,addrIdx); 

    if (paramAddress == NULL) return;
    
    /* Print the parameter value */
    GetValueFromAdress(signalName, paramAddress, slDataID, isComplex,
                        actualDimensions, numDims, slope, bias);

    /* Modify parameter with itself */
    /* modParamflag is used as a flag to indicate whether you want to modify
     * the parameter after you print the value. Set flag to 1 to modify    */
    /* Disabled until further notice.
    if(modParamFlag &&
       capi_ModifyParameter(paramAddress, paramAddress, orientation,
                            actualDimensions, numDims, slDataID, isComplex)) {{
                                printf("Parameter modified with itself\n");
                            }}
    */
    free(actualDimensions);
    return;
}}



struct ScalarVariable GetVariable(rtwCAPI_ModelMappingInfo* capiMap,
                                   uint_T                    index,
                                   uint_T                    flag) {{
    sVariable = EmptyStruct; 
                 
    switch(flag){{
        case MODEL_PARAMETER_FLAG:
            GetModelParameter(capiMap, index);
            break;
        case ROOT_INPUT_FLAG:
        case ROOT_OUTPUT_FLAG:
            GetSignal(capiMap, index, flag);
            break;
        default:
            printf("Flag passed to GetVariable() was %i, not handled.", flag);
    }}

    return sVariable;

}}



