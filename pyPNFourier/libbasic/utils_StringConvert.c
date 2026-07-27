/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include "utils_StringConvert.h"
#include "basic_Constants.h"
#include "basic_StatusMacros.h"
#include "utils_Factories.h"

#define LAL_INT8_ABSMIN MY_UINT8_C(9223372036854775808)
#define LAL_INT4_ABSMIN MY_UINT8_C(2147483648)
#define LAL_INT2_ABSMIN MY_UINT8_C(32768)
#define MY_UINT8_MAXDIGITS (20)

/* Internal function to parse integer strings into UINT8 magnitudes
   plus a sign. */
static UINT8
LALStringToU8AndSign(INT2 * sign, const CHAR * string, CHAR ** endptr)
{
    union {
        char *s;
        const char *cs;
    } bad;      /* there is a REASON for warnings... */
    const CHAR *here = string;  /* current position in string */
    CHAR c;     /* current character in string */
    UINT4 n = MY_UINT8_MAXDIGITS - 1;  /* number of worry-free digits */
    UINT8 value;        /* current converted value */

    /* Skip leading space, and read sign character, if any. */
    *sign = 1;
    while (isspace(*here))
        here++;
    if (*here == '+')
        here++;
    else if (*here == '-') {
        *sign = -1;
        here++;
    }

    /* Read first digit.  Abort if it's not a digit. */
    if (isdigit((int) (c = *here))) {
        value = (UINT8) (c - '0');
        here++;
    } else {
        bad.cs = string;        /* ... and this avoids the warnings... BAD! */
        *endptr = bad.s;
        return 0;
    }

    /* Otherwise, start reading number.  Stop if we get close to
       overflowing. */
    while (isdigit((int) (c = *here)) && --n) {
        value *= MY_INT8_C(10);
        value += (UINT8) (c - '0');
        here++;
    }

    /* Proceed with caution near overflow.  At this point, if n==0, then
       c = *here is the (MY_UINT8_MAXDIGITS)th digit read, but value
       does not yet incorporate it. */
    if (!n) {
        here++;
        if (isdigit((int) (*here))) {
            value = MY_UINT8_MAX;
            do
                here++;
            while (isdigit((int) (*here)));
        } else if (value > MY_UINT8_MAX / MY_INT8_C(10)) {
            value = MY_UINT8_MAX;
        } else {
            UINT8 increment = (UINT8) (c - '0');
            value *= 10;
            if (value > MY_UINT8_MAX - increment)
                value = MY_UINT8_MAX;
            else
                value += increment;
        }
    }

    /* Return appropriate values. */
    bad.cs = here;      /* ... and this avoids the warnings... BAD! */
    *endptr = bad.s;
    return value;
}

/** \endcond */

void
LALStringToU2(ProcStatus * stat, UINT2 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (absValue > MY_UINT2_MAX)
        *value = (UINT2) (MY_UINT2_MAX);
    else
        *value = (UINT2) (absValue);
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToU4(ProcStatus * stat, UINT4 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (absValue > MY_UINT4_MAX)
        *value = (UINT4) (MY_UINT4_MAX);
    else
        *value = (UINT4) (absValue);
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToU8(ProcStatus * stat, UINT8 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Set values and return. */
    *value = absValue;
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToI2(ProcStatus * stat, INT2 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (sign > 0) {
        if (absValue > MY_INT2_MAX)
            *value = (INT2) (MY_INT2_MAX);
        else
            *value = (INT2) (absValue);
    } else {
        if (absValue > LAL_INT2_ABSMIN)
            *value = (INT2) (-LAL_INT2_ABSMIN);
        else
            *value = (INT2) (-absValue);
    }
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToI4(ProcStatus * stat, INT4 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (sign > 0) {
        if (absValue > MY_INT4_MAX)
            *value = (INT4) (MY_INT4_MAX);
        else
            *value = (INT4) (absValue);
    } else {
        if (absValue > LAL_INT4_ABSMIN)
            *value = (INT4) (-LAL_INT4_ABSMIN);
        else
            *value = (INT4) (-absValue);
    }
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToI8(ProcStatus * stat, INT8 * value, const CHAR * string,
              CHAR ** endptr)
{
    UINT8 absValue;     /* magnitude of parsed number */
    INT2 sign;  /* sign of parsed number */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    absValue = LALStringToU8AndSign(&sign, string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (sign > 0) {
        if (absValue > MY_INT8_MAX)
            *value = (INT8) (MY_INT8_MAX);
        else
            *value = (INT8) (absValue);
    } else {
        if (absValue > LAL_INT8_ABSMIN)
            *value = (INT8) (-LAL_INT8_ABSMIN);
        else
            *value = (INT8) (-absValue);
    }
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToS(ProcStatus * stat, REAL4 * value, const CHAR * string,
             CHAR ** endptr)
{
    REAL8 myValue;      /* internal representation of value */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    myValue = strtod(string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Cap (if necessary), cast, and return. */
    if (myValue > MY_REAL4_MAX)
        *value = (REAL4) (MY_REAL4_MAX);
    else if (myValue < -MY_REAL4_MAX)
        *value = (REAL4) (-MY_REAL4_MAX);
    else
        *value = (REAL4) (myValue);
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToD(ProcStatus * stat, REAL8 * value, const CHAR * string,
             CHAR ** endptr)
{
    REAL8 myValue;      /* internal representation of value */
    CHAR *end;  /* substring following parsed number */

    INITSTATUS(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    myValue = strtod(string, &end);
    if (string == end) {
        if (endptr)
            *endptr = end;
        RETURN(stat);
    }

    /* Set values and return. */
    if (myValue > MY_REAL8_MAX)
        *value = MY_REAL8_MAX;
    else if (myValue < -MY_REAL8_MAX)
        *value = -MY_REAL8_MAX;
    else
        *value = myValue;
    if (endptr)
        *endptr = end;
    RETURN(stat);
}



void
LALStringToC(ProcStatus * stat, COMPLEX8 * value, const CHAR * string,
             CHAR ** endptr)
{
    REAL4 re, im;       /* real and imaginary parts */
    CHAR *end;  /* substring following parsed numbers */

    INITSTATUS(stat);
    ATTATCHSTATUSPTR(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    TRY(LALStringToS(stat->statusPtr, &re, string, &end), stat);
    TRY(LALStringToS(stat->statusPtr, &im, end, &end), stat);
    if (string == end) {
        if (endptr)
            *endptr = end;
        DETATCHSTATUSPTR(stat);
        RETURN(stat);
    }

    /* Set values and return. */
    *value = re;
    *value += im * I;
    if (endptr)
        *endptr = end;
    DETATCHSTATUSPTR(stat);
    RETURN(stat);
}



void
LALStringToZ(ProcStatus * stat, COMPLEX16 * value, const CHAR * string,
             CHAR ** endptr)
{
    REAL8 re, im;       /* real and imaginary parts */
    CHAR *end;  /* substring following parsed numbers */

    INITSTATUS(stat);
    ATTATCHSTATUSPTR(stat);

    /* Check for valid input arguments. */
    ASSERT(value, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Parse string.  Return if nothing was parsed. */
    TRY(LALStringToD(stat->statusPtr, &re, string, &end), stat);
    TRY(LALStringToD(stat->statusPtr, &im, end, &end), stat);
    if (string == end) {
        if (endptr)
            *endptr = end;
        DETATCHSTATUSPTR(stat);
        RETURN(stat);
    }

    /* Set values and return. */
    *value = re;
    *value += im * I;
    if (endptr)
        *endptr = end;
    DETATCHSTATUSPTR(stat);
    RETURN(stat);
}


void
LALCreateTokenList(ProcStatus * stat,
                   TokenList ** list,
                   const CHAR * string, const CHAR * delimiters)
{
    BOOLEAN delimiter = 1;      /* whether current character is a delimiter */
    UINT4 i = 0, j = 0; /* indecies */
    UINT4 nTokens = 0;  /* number of tokens */
    UINT4 sLength;      /* length of string */
    UINT4 tLength = 0;  /* length of token list */
    CHAR *copy; /* working copy of token list */

    INITSTATUS(stat);
    ATTATCHSTATUSPTR(stat);

    /* Check for valid input arguments. */
    ASSERT(list, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(string, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(delimiters, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(!*list, stat, STRINGINPUTH_EOUT, STRINGINPUTH_MSGEOUT);

    /* Create working copy of token list. */
    sLength = strlen(string) + 1;
    if (!(copy = (CHAR *) MYMalloc(sLength * sizeof(CHAR)))) {
        ABORT(stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    }
    for (i = 0; i < sLength; i++) {
        CHAR c = string[i];
        if (strchr(delimiters, c)) {
            copy[i] = '\0';
            delimiter = 1;
        } else {
            copy[i] = c;
            tLength++;
            if (delimiter) {
                delimiter = 0;
                nTokens++;
            }
        }
    }

    /* Create the token list. */
    if (!(*list = (TokenList *) MYMalloc(sizeof(TokenList)))) {
        MYFree(copy);
        ABORT(stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    }
    if (!((*list)->tokens =
          (CHAR **) MYMalloc((nTokens + 1) * sizeof(CHAR *)))) {
        MYFree(*list);
        *list = NULL;
        MYFree(copy);
        ABORT(stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    }
    (*list)->nTokens = nTokens;
    (*list)->list = NULL;


    /* If tokens were found, copy them over and set up pointers. */
    if (nTokens) {
        CHAR *listData; /* pointer to token list data */
        LALCHARCreateVector(stat->statusPtr, &((*list)->list),
                            nTokens + tLength);
        BEGINFAIL(stat) {
            MYFree((*list)->tokens);
            MYFree(*list);
            *list = NULL;
            MYFree(copy);
        }
        ENDFAIL(stat);
        listData = (*list)->list->data;
        i = 0;
        while (i < sLength) {
            if (copy[i]) {
                tLength = strlen(copy + i) + 1;
                memcpy(listData, copy + i, tLength * sizeof(CHAR));
                (*list)->tokens[j++] = listData;
                i += tLength;
                listData += tLength;
            } else
                i++;
        }
    }
    (*list)->tokens[j] = NULL;

    /* Clean up and exit. */
    MYFree(copy);
    DETATCHSTATUSPTR(stat);
    RETURN(stat);
}

/** Split given input string into a list of 'tokens' separated by any
 * of the characters given in 'delimiters'
 */
int XLALCreateTokenList(TokenList ** list,         //!< [out] list of tokens
                        const CHAR * string,       //!< [in] string to split into tokens
                        const CHAR * delimiters    //!< [in] set of token-delimiter characters
                        )
{
    X_CHECK((list != NULL) && ((*list) == NULL), X_EINVAL);
    X_CHECK(string != NULL, X_EINVAL);
    X_CHECK(delimiters != NULL, X_EINVAL);

    // prepare output TokenList structure
    TokenList *ret;
    X_CHECK((ret = XCalloc(1, sizeof(*ret))) != NULL, X_ENOMEM);

    size_t stringLen = strlen(string);
    if ((ret->list = CreateCHARVector(stringLen + 1)) == NULL) {
        XFree(ret);
        X_ERROR(X_ENOMEM);
    }
    strcpy(ret->list->data, string);

    // initialize pointers to walk along local copy of input string
    char *ptr = ret->list->data;
    const char *endPtr = ptr + stringLen;

    UINT4 nTokens = 0;
    UINT4 nTokensAlloc = 0;

    while ((ptr != NULL) && (ptr < endPtr)) {
        // skip and nuke delimiter
        size_t skip = strspn(ptr, delimiters);
        memset(ptr, 0, skip);   // fill with '0'
        ptr += skip;

        if (ptr >= endPtr) {
            break;
        }
        // 'ptr' points at next token
        nTokens++;

        // allocate next batch of token-pointers if required
        if (nTokens > nTokensAlloc) {
            nTokensAlloc = 2 * nTokens; // proceed by doubling current space
            if ((ret->tokens = XRealloc(ret->tokens, nTokensAlloc * sizeof(char *))) == NULL) {
                DestroyTokenList(ret);
                X_ERROR(X_ENOMEM);
            }
        }       // if nTokens > nTokensAlloc

        // enter new token-pointer into list
        ret->tokens[nTokens - 1] = ptr;

        // advance to next delimiter
        ptr = strpbrk(ptr, delimiters);

    }   // while ptr < endPtr

    // reduce tokens-array to actual size
    if ((ret->tokens = XRealloc(ret->tokens, nTokens * sizeof(char *))) == NULL) {
        DestroyTokenList(ret);
        X_ERROR(X_ENOMEM);
    }
    ret->nTokens = nTokens;

    // return result
    (*list) = ret;

    return X_SUCCESS;

}       // XLALCreateTokenList()

/**
 * \deprecated Use XLALDestroyTokenList() instead
 */
void LALDestroyTokenList(ProcStatus * stat, TokenList ** list)
{
    INITSTATUS(stat);
    ATTATCHSTATUSPTR(stat);

    /* Check for valid input arguments. */
    ASSERT(list, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);
    ASSERT(*list, stat, STRINGINPUTH_ENUL, STRINGINPUTH_MSGENUL);

    /* Free everything and exit. */
    if ((*list)->list) {
        TRY(LALCHARDestroyVector(stat->statusPtr, &((*list)->list)), stat);
    }
    MYFree((*list)->tokens);
    MYFree(*list);
    *list = NULL;
    DETATCHSTATUSPTR(stat);
    RETURN(stat);
}

/** See StringToken.c for documentation */
void DestroyTokenList(TokenList * list)
{
    /* Free everything and exit. */
    if (list) {
        if (list->list)
            DestroyCHARVector(list->list);
        XFree(list->tokens);
        XFree(list);
    }
}
