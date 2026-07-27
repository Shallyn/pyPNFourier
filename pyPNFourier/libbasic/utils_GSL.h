/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_GSL__
#define __INCLUDE_UTILS_GSL__

#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_errno.h>

#include "basic_Alloc.h"
#include "basic_Datatypes.h"
#include "basic_Debug.h"
#include "basic_Error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X_CALLGSL( statement ) \
        do { \
          gsl_error_handler_t *saveGSLErrorHandler_; \
          saveGSLErrorHandler_ = gsl_set_error_handler_off(); \
          statement; \
          gsl_set_error_handler( saveGSLErrorHandler_ ); \
        } while (0)


extern ProcStatus *g_GSLGlobalStatusPtr;

void
LALGSLErrorHandler(const char *reason,
                   const char *file, int line, int errnum);

#define CALLGSL( statement, statusptr )                                       \
  if ( (statusptr) )                                                          \
  {                                                                           \
    ProcStatus *saveLALGSLGlobalStatusPtr_;                                    \
    gsl_error_handler_t *saveGSLErrorHandler_;                                \
    if ( !( (statusptr)->statusPtr ) )                                        \
      { ABORT( (statusptr), -8, "CALLGSL: null status pointer pointer" ); }   \
    saveGSLErrorHandler_ = gsl_set_error_handler( LALGSLErrorHandler );       \
    saveLALGSLGlobalStatusPtr_ = g_GSLGlobalStatusPtr;                       \
    g_GSLGlobalStatusPtr = (statusptr)->statusPtr;                           \
    statement;                                                                \
    g_GSLGlobalStatusPtr = saveLALGSLGlobalStatusPtr_;                       \
    gsl_set_error_handler( saveGSLErrorHandler_ );                            \
  }                                                                           \
  else                                                                        \
    g_AbortHook( "Abort: CALLGSL, file %s, line %d\n"                        \
                  "       Null status pointer passed to CALLGSL\n",           \
                  __FILE__, __LINE__ )


#define TRYGSL( statement, statusptr )                                        \
  if ( (statusptr) )                                                          \
  {                                                                           \
    ProcStatus *saveLALGSLGlobalStatusPtr_;                                    \
    gsl_error_handler_t *saveGSLErrorHandler_;                                \
    if ( !( (statusptr)->statusPtr ) )                                        \
      { ABORT( (statusptr), -8, "CALLGSL: null status pointer pointer" ); }   \
    saveGSLErrorHandler_ = gsl_set_error_handler( LALGSLErrorHandler );       \
    saveLALGSLGlobalStatusPtr_ = g_GSLGlobalStatusPtr;                       \
    g_GSLGlobalStatusPtr = (statusptr)->statusPtr;                           \
    statement;                                                                \
    g_GSLGlobalStatusPtr = saveLALGSLGlobalStatusPtr_;                       \
    gsl_set_error_handler( saveGSLErrorHandler_ );                            \
    if ( (statusptr)->statusPtr->statusCode )                                 \
    {                                                                         \
      SETSTATUS( statusptr, -1, "Recursive error" );                          \
      (void) LALError( statusptr, "Statement \"" #statement "\" failed:" );   \
      (void) LALTrace( statusptr, 1 );                                        \
      return;                                                                 \
    }                                                                         \
  }                                                                           \
  else                                                                        \
    g_AbortHook( "Abort: CALLGSL, file %s, line %d\n"                        \
                  "       Null status pointer passed to CALLGSL\n",           \
                  __FILE__, __LINE__ )

#ifdef __cplusplus
}
#endif
#endif

