/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include "basic_StatusMacros.h"
#include "utils_Factories.h"
#include "basic_Error.h"

#define TYPECODE W
#define TYPE COMPLEX64
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Y
#define TYPE COMPLEX32
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Z
#define TYPE COMPLEX16
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE C
#define TYPE COMPLEX8
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE D
#define TYPE REAL8
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE E
#define TYPE REAL16
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE F
#define TYPE REAL32
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE S
#define TYPE REAL4
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I2
#define TYPE INT2
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I4
#define TYPE INT4
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I8
#define TYPE INT8
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U2
#define TYPE UINT2
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U4
#define TYPE UINT4
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U8
#define TYPE UINT8
#include "source_CreateArray.c"
#include "source_ResizeArray.c"
#include "source_DestroyArray.c"
#undef TYPECODE
#undef TYPE

// Vector
#define TYPECODE W
#define TYPE COMPLEX64
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Y
#define TYPE COMPLEX32
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Z
#define TYPE COMPLEX16
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE C
#define TYPE COMPLEX8
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE D
#define TYPE REAL8
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE E
#define TYPE REAL16
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE F
#define TYPE REAL32
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE S
#define TYPE REAL4
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I2
#define TYPE INT2
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I4
#define TYPE INT4
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I8
#define TYPE INT8
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U2
#define TYPE UINT2
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U4
#define TYPE UINT4
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U8
#define TYPE UINT8
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE CHAR
#define TYPE CHAR
#include "source_CreateVector.c"
#include "source_DestroyVector.c"
#include "source_ResizeVector.c"
#undef TYPECODE
#undef TYPE

// #define TYPE REAL4
// #include "source_CreateVector.c"
// #include "source_DestroyVector.c"
// #include "source_ResizeVector.c"
// #undef TYPE

#define TYPECODE W
#define TYPE COMPLEX64
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Y
#define TYPE COMPLEX32
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Z
#define TYPE COMPLEX16
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE C
#define TYPE COMPLEX8
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE D
#define TYPE REAL8
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE E
#define TYPE REAL16
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE F
#define TYPE REAL32
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE S
#define TYPE REAL4
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I2
#define TYPE INT2
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I4
#define TYPE INT4
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I8
#define TYPE INT8
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U2
#define TYPE UINT2
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U4
#define TYPE UINT4
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U8
#define TYPE UINT8
#include "source_CreateArraySequence.c"
#include "source_DestroyArraySequence.c"
#undef TYPECODE
#undef TYPE

// #define TYPE REAL4
// #include "CreateArraySequence_source.c"
// #include "DestroyArraySequence_source.c"
// #undef TYPE

#define TYPECODE W
#define TYPE COMPLEX64
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Y
#define TYPE COMPLEX32
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE Z
#define TYPE COMPLEX16
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE C
#define TYPE COMPLEX8
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE D
#define TYPE REAL8
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE E
#define TYPE REAL16
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE F
#define TYPE REAL32
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE S
#define TYPE REAL4
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I2
#define TYPE INT2
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I4
#define TYPE INT4
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE I8
#define TYPE INT8
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U2
#define TYPE UINT2
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U4
#define TYPE UINT4
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE U8
#define TYPE UINT8
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

#define TYPECODE CHAR
#define TYPE CHAR
#include "source_CreateVectorSequence.c"
#include "source_DestroyVectorSequence.c"
#undef TYPECODE
#undef TYPE

// #define TYPE REAL4
// #include "source_CreateVectorSequence.c"
// #include "source_DestroyVectorSequence.c"
// #undef TYPE


#define DATATYPE REAL4
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE REAL8
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE REAL16
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE REAL32
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE COMPLEX8
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE COMPLEX16
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE COMPLEX32
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE COMPLEX64
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE INT2
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE UINT2
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE INT4
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE UINT4
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE INT8
#include "source_TimeSeries.c"
#undef DATATYPE

#define DATATYPE UINT8
#include "source_TimeSeries.c"
#undef DATATYPE
