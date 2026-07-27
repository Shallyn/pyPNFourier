/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_STRING__
#define __INCLUDE_BASIC_STRING__
#include <stddef.h>
#include "basic_Datatypes.h"

#ifdef __cplusplus
extern "C" \{
#endif

int XStringPrint(char *s, size_t n, const char *fmt, ...) _GCC_PRINTF_FORMAT_(3,4);
char *XStringAppend(char *s, const char *append);
char *XStringAppendFmt(char *s, const char *fmt, ...) _GCC_PRINTF_FORMAT_(2,3);
char *XStringDuplicate(const char *s);
size_t XStringCopy(char *dst, const char *src, size_t size);
size_t XStringConcatenate(char *dst, const char *src, size_t size);
int XStringToLowerCase(char * string);
int XStringToUpperCase(char * string);
int XStringCaseCompare(const char *s1, const char *s2);
int XStringNCaseCompare(const char *s1, const char *s2, size_t n);
char *XStringCaseSubstring(const char *haystack, const char *needle);
char *XStringToken(char **s, const char *delim, int empty);
char *XStringReplaceChar(char *s, const int from, const int to);


#ifdef __cplusplus
\}
#endif
#endif

