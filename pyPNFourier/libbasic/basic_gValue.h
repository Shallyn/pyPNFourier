/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
**/

#ifndef __INCLUDE_GVALUE__
#define __INCLUDE_GVALUE__
#include "utils_Datatypes.h"

typedef struct {
    TYPECODE type;
	size_t size;
    union {
        CHAR        i1;
        INT2        i2;
        INT4        i4;
        INT8        i8;
        UCHAR       u1;
        UINT2       u2;
        UINT4       u4;
        UINT8       u8;
        REAL8       s;
        REAL8       d;
        COMPLEX8    c;
        COMPLEX16   z;
    } data[];
}GValue;

GValue *GValueAlloc(size_t size);
GValue *GValueRealloc(GValue *value, size_t size);
GValue *GValueDuplicate(const GValue *value);
GValue *GValueCopy(GValue *copy, const GValue *orig);
GValue *GValueSet(GValue *value, const void *data, size_t size, TYPECODE type);

void DestroyGValue(GValue *value);

GValue *CreateGValue(const void * data, size_t size, TYPECODE type);
GValue *CreateBLOBValue(const void *blob, size_t size);
GValue *CreateStringValue(const char *string);
GValue *CreateCHARValue(CHAR value);
GValue *CreateINT2Value(INT2 value);
GValue *CreateINT4Value(INT4 value);
GValue *CreateINT8Value(INT8 value);
GValue *CreateUCHARValue(UCHAR value);
GValue *CreateUINT2Value(UINT2 value);
GValue *CreateUINT4Value(UINT4 value);
GValue *CreateUINT8Value(UINT8 value);
GValue *CreateREAL4Value(REAL4 value);
GValue *CreateREAL8Value(REAL8 value);
GValue *CreateCOMPLEX8Value(COMPLEX8 value);
GValue *CreateCOMPLEX16Value(COMPLEX16 value);

TYPECODE GValueGetType(const GValue *value);
size_t GValueGetSize(const GValue *value);
/* warning: shallow pointer */
const void * GValueGetDataPtr(const GValue *value);
void * GValueGetData(void *data, size_t size, TYPECODE type, const GValue *value);
int GValueEqual(const GValue *value1, const GValue *value2);

void * GValueGetBLOB(const GValue *value);
/* warning: shallow pointer */
const char * GValueGetString(const GValue *value);
CHAR GValueGetCHAR(const GValue *value);
INT2 GValueGetINT2(const GValue *value);
INT4 GValueGetINT4(const GValue *value);
INT8 GValueGetINT8(const GValue *value);
UCHAR GValueGetUCHAR(const GValue *value);
UINT2 GValueGetUINT2(const GValue *value);
UINT4 GValueGetUINT4(const GValue *value);
UINT8 GValueGetUINT8(const GValue *value);
REAL4 GValueGetREAL4(const GValue *value);
REAL8 GValueGetREAL8(const GValue *value);
COMPLEX8 GValueGetCOMPLEX8(const GValue *value);
COMPLEX16 GValueGetCOMPLEX16(const GValue *value);

REAL8 GValueGetAsREAL8(const GValue *value);

char * GValueAsStringAppend(char *s, const GValue *value);
void GValuePrint(const GValue *value, int fd);

#endif