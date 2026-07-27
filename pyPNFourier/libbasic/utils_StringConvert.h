/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_STRINGCONVERT__
#define __INCLUDE_UTILS_STRINGCONVERT__
#include <complex.h>
#include <ctype.h>
#include "utils_Datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \name Error Codes */ /** @{ */
#define STRINGINPUTH_ENUL 1     /**< Unexpected null pointer in arguments */
#define STRINGINPUTH_EOUT 2     /**< Output handle points to a non-null pointer */
#define STRINGINPUTH_EMEM 3     /**< Memory allocation error */
/** @} */
/** \cond DONT_DOXYGEN */
#define STRINGINPUTH_MSGENUL "Unexpected null pointer in arguments"
#define STRINGINPUTH_MSGEOUT "Output handle points to a non-null pointer"
#define STRINGINPUTH_MSGEMEM "Memory allocation error"
/** \endcond */

/**
 * This structure stores a number of null-terminated strings of arbitrary
 * length.  The entire list is stored flattened in a \c CHARVector,
 * and individual tokens are pointed to by a <tt>CHAR *[]</tt> handle.
 */
typedef struct tagTokenList {
    UINT4 nTokens;  /**< The number of tokens in the list */
    CHAR **tokens;  /**< A list of pointers to the individual tokens;
                     * the elements <tt>tokens[0..nTokens-1]</tt> point to tokens, and
                     * the element <tt>tokens[nTokens]</tt> is explicitly \c NULL (as is
                     the convention for an \c argv argument list */
    CHARVector *list;
                    /**< The flattened list of tokens, separated by (and terminated with) <tt>'\0'</tt> characters */
} TokenList;

/* Function prototypes. */

void
LALCreateTokenList(ProcStatus * status,
                   TokenList ** list,
                   const CHAR * string, const CHAR * delimiters);

void LALDestroyTokenList(ProcStatus * status, TokenList ** list);


int
XLALCreateTokenList(TokenList ** list,
                    const CHAR * string, const CHAR * delimiters);

void XLALDestroyTokenList(TokenList * list);

void
LALStringToU2(ProcStatus * status, UINT2 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToU4(ProcStatus * status, UINT4 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToU8(ProcStatus * status, UINT8 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToI2(ProcStatus * status, INT2 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToI4(ProcStatus * status, INT4 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToI8(ProcStatus * status, INT8 * value, const CHAR * string,
              CHAR ** endptr);

void
LALStringToS(ProcStatus * status, REAL4 * value, const CHAR * string,
             CHAR ** endptr);

void
LALStringToD(ProcStatus * status, REAL8 * value, const CHAR * string,
             CHAR ** endptr);

void
LALStringToC(ProcStatus * status, COMPLEX8 * value, const CHAR * string,
             CHAR ** endptr);

void
LALStringToZ(ProcStatus * status, COMPLEX16 * value, const CHAR * string,
             CHAR ** endptr);

void DestroyTokenList(TokenList * list);


#ifdef __cplusplus
}
#endif
#endif

