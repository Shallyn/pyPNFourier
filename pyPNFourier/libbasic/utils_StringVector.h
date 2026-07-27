/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_STRINGVECTOR__
#define __INCLUDE_UTILS_STRINGVECTOR__

#include <stdarg.h>
#include "utils_Datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

StringVector *CreateStringVector ( const CHAR *str1, ... );
StringVector *CreateEmptyStringVector ( UINT4 length );
StringVector *CopyStringVector ( const StringVector *vect );
StringVector *AppendString2Vector (StringVector *vect, const CHAR *string );
void DestroyStringVector ( StringVector *vect );

char *ConcatStringVector( const StringVector *strings, const char *sep );
StringVector *ParseStringVector( const char *string, const char *delim );

int SortStringVector (StringVector *strings);
INT4 FindStringInVector ( const char *needle, const StringVector *haystack );

char *DeblankString ( const char *start, UINT4 len );

#ifdef __cplusplus
}
#endif
#endif

