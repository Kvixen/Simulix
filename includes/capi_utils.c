#include "bil.h"
#include <stddef.h>
#include <assert.h>
#include "capi_utils.h"
#include <string.h>


struct ScalarVariable sVariable;

void GetValueFromAdress(const char_T*  paramName,
                         void*          paramAddress,
                         uint8_T        slDataID,
                         unsigned short isComplex,
                         uint_T*        actualDims,
                         uint_T         numDims,
                         real_T         slope,
                         real_T         bias) {
    strcpy(sVariable.name, paramName);
    sVariable.DataID = slDataID;

    uint_T numRows  = actualDims[0];
    uint_T numCols  = actualDims[1];
    uint_T numPages = 0;
    int rowIdx;
    int colIdx;
    int pageIdx;
    if (numDims == 3) numPages = actualDims[2];
    switch(slDataID) {
      case SS_DOUBLE :
        if (isComplex) {
            /* If is Complex and SS_DOUBLE, the data is of type creal_T */
            creal_T* paramVal = (creal_T *) paramAddress;
            for(rowIdx = 0; rowIdx < numRows; rowIdx++) {
                for(colIdx = 0; colIdx < numCols; colIdx++) {
                    printf("\t%.4g + %.4g*i", 
                           paramVal[colIdx*numRows + rowIdx].re,
                           paramVal[colIdx*numRows + rowIdx].im);
                }
                putchar('\n');
            }
        } else {
            real_T* paramVal = (real_T *) paramAddress;
            if (numDims == 3) {
                for (pageIdx = 0; pageIdx < numPages; pageIdx++) {
                    printf("ans(:,:,%d) =\n", pageIdx+1); 
                    for (rowIdx = 0; rowIdx < numRows; rowIdx++) {
                        for (colIdx = 0; colIdx < numCols; colIdx++) {
                            uint_T idx = rowIdx + numRows * colIdx +
                                numRows * numCols * pageIdx;
                            printf("\t%.4g", paramVal[idx]);
                        }
                        putchar('\n');
                    }
                    putchar('\n');
                }
            } else {
                for(rowIdx = 0; rowIdx < numRows; rowIdx++) {
                    for(colIdx = 0; colIdx < numCols; colIdx++) {
                        //printf("%.4f\n", paramVal[colIdx*numRows + rowIdx]);
                        sVariable.value = (float)paramVal[colIdx*numRows + rowIdx];
                    }
                    //putchar('\n');
                }
            }
        }
        break;
      case SS_INT32 :
        if (isComplex) {
            cint32_T* paramVal = (cint32_T *) paramAddress;
            for(rowIdx = 0; rowIdx < numRows; rowIdx++) {
                for(colIdx = 0; colIdx < numCols; colIdx++) {
                    printf("\t%d + %d*i", 
                           paramVal[colIdx*numRows + rowIdx].re,
                           paramVal[colIdx*numRows + rowIdx].im);
                }						
                putchar('\n');
            }
        } else {
            int32_T* paramVal = (int32_T *) paramAddress;
            for(rowIdx = 0; rowIdx < numRows; rowIdx++) {
                for(colIdx = 0; colIdx < numCols; colIdx++) {
                    int32_T intVal  = paramVal[colIdx*numRows + rowIdx];
                    /* Real world value = (slope * stored integer) + bias */
                    real_T  realVal = slope*(intVal) + bias;
                    printf("\t%.4g", realVal);
                }
                putchar('\n');
            }
        }
        break;
      default :
        /* TODO: extend the switch-case to other Simulink DataIds
         * case SS_SINGLE:
         * case SS_UINT32:
         * case SS_INT16:
         * case SS_UINT16:
         * case SS_INT8:
         * case SS_UINT8:
         * case SS_BOOLEAN
         * Remove from list if you have extended the switch.
         * This function currently doesn't support these Datatypes. */
        printf("Unhandled Data Type: %s is skipped\n", paramName);
        break;
    }
    return;
}

void GetModelParameter(rtwCAPI_ModelMappingInfo* capiMap,
                              uint_T                    paramIdx) {
    
    const rtwCAPI_ModelParameters* modelParams;
    const rtwCAPI_DataTypeMap*     dataTypeMap;
    const rtwCAPI_DimensionMap*    dimMap;
    const rtwCAPI_FixPtMap*        fxpMap;
    const uint_T*                  dimArray;
    void**                         dataAddrMap;

    const char_T* paramName;
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
     *       uint_T ActualDims[numDims] = {dimArray[dimArrayIdx],            *
     *                                     dimArray[dimArrayIdx+1],          *
     *				       ...                                   *
     *				       dimArray[dimArrayIdx+(numDims-1)]}    *
     *    For e.g, scalar and 2-D parameters will have numDims = 2, So       *
     *       uint_T ActualDims[2] = {dimArray[dimArrayIdx],                  *
     *                               dimArray[dimArrayIdx +1]}               */
    
    dimMap   = rtwCAPI_GetDimensionMap(capiMap);
    dimArray = rtwCAPI_GetDimensionArray(capiMap);

    if ((dimMap == NULL) || (dimArray == NULL)) return;

    dimIndex    = rtwCAPI_GetModelParameterDimensionIdx(modelParams, paramIdx);
    numDims     = rtwCAPI_GetNumDims(dimMap, dimIndex); 
    dimArrayIdx = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    orientation = rtwCAPI_GetOrientation(dimMap, dimIndex);

    actualDimensions = (uint_T *) malloc(numDims*sizeof(uint_T));
    for(idx=0; idx < numDims; idx++) {
        actualDimensions[idx] = dimArray[dimArrayIdx + idx];
    }
    
    /* Get fixed-point information of the parameter */
    fxpMap = rtwCAPI_GetFixPtMap(capiMap);
    if (fxpMap == NULL) return;
    
    fxpMapIdx = rtwCAPI_GetModelParameterFixPtIdx(modelParams, paramIdx);
    if(fxpMapIdx > 0) {
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
    }

    /* Get the address to this parameter                                     */
    dataAddrMap = rtwCAPI_GetDataAddressMap(capiMap);
    addrIdx     = rtwCAPI_GetModelParameterAddrIdx(modelParams,paramIdx);
    paramAddress= (void *) rtwCAPI_GetDataAddress(dataAddrMap,addrIdx); 

    if (paramAddress == NULL) return;
    
    /* Print the parameter value */
    GetValueFromAdress(paramName, paramAddress, slDataID, isComplex,
                        actualDimensions, numDims, slope, bias);

    /* Modify parameter with itself */
    /* modParamflag is used as a flag to indicate whether you want to modify
     * the parameter after you print the value. Set flag to 1 to modify    */
    /* Disabled until further notice.
    if(modParamFlag &&
       capi_ModifyParameter(paramAddress, paramAddress, orientation,
                            actualDimensions, numDims, slDataID, isComplex)) {
                                printf("Parameter modified with itself\n");
                            }
    */
    free(actualDimensions);
    return;
}

void GetSignal(rtwCAPI_ModelMappingInfo* capiMap,
                       uint_T                    signalIdx,
                       uint_T                    signalTypeFlag) {
    
    const rtwCAPI_Signals*         signals;
    const rtwCAPI_DataTypeMap*     dataTypeMap;
    const rtwCAPI_DimensionMap*    dimMap;
    const rtwCAPI_FixPtMap*        fxpMap;
    const uint_T*                  dimArray;
    void**                         dataAddrMap;

    const char_T* signalName;
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
    assert(signalIdx < rtwCAPI_GetNumModelParameters(capiMap));

    switch(signalTypeFlag)
    {
        case 1:
            signals = rtwCAPI_GetRootInputs(capiMap);
            break;
        case 2:
            signals = rtwCAPI_GetRootOutputs(capiMap);
            break;
    }

    if(signals == NULL) return;
    
    /* Get Parameter Name */
    signalName   = rtwCAPI_GetSignalName(signals, signalIdx); 

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
     *       uint_T ActualDims[numDims] = {dimArray[dimArrayIdx],            *
     *                                     dimArray[dimArrayIdx+1],          *
     *				       ...                                   *
     *				       dimArray[dimArrayIdx+(numDims-1)]}    *
     *    For e.g, scalar and 2-D parameters will have numDims = 2, So       *
     *       uint_T ActualDims[2] = {dimArray[dimArrayIdx],                  *
     *                               dimArray[dimArrayIdx +1]}               */
    
    dimMap   = rtwCAPI_GetDimensionMap(capiMap);
    dimArray = rtwCAPI_GetDimensionArray(capiMap);

    if ((dimMap == NULL) || (dimArray == NULL)) return;

    dimIndex    = rtwCAPI_GetSignalDimensionIdx(signals, signalIdx);
    numDims     = rtwCAPI_GetNumDims(dimMap, dimIndex); 
    dimArrayIdx = rtwCAPI_GetDimArrayIndex(dimMap, dimIndex);
    orientation = rtwCAPI_GetOrientation(dimMap, dimIndex);

    actualDimensions = (uint_T *) malloc(numDims*sizeof(uint_T));
    for(idx=0; idx < numDims; idx++) {
        actualDimensions[idx] = dimArray[dimArrayIdx + idx];
    }
    
    /* Get fixed-point information of the parameter */
    fxpMap = rtwCAPI_GetFixPtMap(capiMap);
    if (fxpMap == NULL) return;
    
    fxpMapIdx = rtwCAPI_GetSignalFixPtIdx(signals, signalIdx);
    if(fxpMapIdx > 0) {
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
    }

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
                            actualDimensions, numDims, slDataID, isComplex)) {
                                printf("Parameter modified with itself\n");
                            }
    */
    free(actualDimensions);
    return;
}



struct ScalarVariable GetVariable(rtwCAPI_ModelMappingInfo* capiMap,
                                   uint_T                    index,
                                   uint_T                    flag) {
    sVariable = EmptyStruct; 
                 
    switch(flag){
        case 0:
            strcpy(sVariable.type, "Parameter");
            GetModelParameter(capiMap, index);
            break;
        case 1:
            strcpy(sVariable.type, "Input");
            GetSignal(capiMap, index, flag);
            break;
        case 2:
            strcpy(sVariable.type, "Output");
            GetSignal(capiMap, index, flag);
            break;
        default:
            printf("Flag passed to getVariable was not 0 nor 1-2");
    }

    return sVariable;

}



