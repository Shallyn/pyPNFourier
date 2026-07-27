/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_OPTIONPARSER__
#define __INCLUDE_UTILS_OPTIONPARSER__

#include <string.h>
#include "utils_Datatypes.h"

#undef no_argument
#undef required_argument
#undef optional_argument

#define opt_no_argument        0
#define opt_required_argument    1
#define opt_optional_argument    2

#ifdef __cplusplus
extern "C" {
#endif

extern CHAR *g_optarg;
extern INT4 g_optind;

typedef struct tagOptionParserArg
{
    const CHAR *name;
    INT4 has_arg;
    INT4 *flag;
    INT4 val;
} OptionParserArg;

INT4 getopt_long_only(INT4 argc, CHAR *const *argv, const CHAR *options,
                     const OptionParserArg *opt, INT4 *opt_index);


#ifdef __cplusplus
}
#endif
#endif

