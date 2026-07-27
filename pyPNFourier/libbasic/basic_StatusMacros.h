/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_STATUSMACROS__
#define __INCLUDE_BASIC_STATUSMACROS__

#include <string.h>
#include "basic_Error.h"
#include "basic_Debug.h"
#include "basic_Datatypes.h"
#include "basic_Alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MC_EXLAL     16384
#define MC_MSGEXLAL  "Failure in an XLAL routine"
#define ABORTXLAL(sp) ABORT(sp,MC_EXLAL,MC_MSGEXLAL)
#define XLAL_CHECK_LAL(sp, assertion, ...) _XLAL_CHECK_IMPL_(ABORTXLAL(sp), assertion, __VA_ARGS__)

#define INITSTATUS( statusptr )                                               \
  do { if ( (statusptr) )                                                     \
  {                                                                           \
    INT4 level_ = (statusptr)->level ;                                        \
    INT4 statp_ = (statusptr)->statusPtr ? 1 : 0 ;                            \
    memset( (statusptr), 0, sizeof( ProcStatus ) ); /* possible memory leak */ \
    (statusptr)->level    = level_ > 0 ? level_ : 1 ;                         \
    (statusptr)->Id       = "$Id$";                                           \
    (statusptr)->function = __func__;                                         \
    SETSTATUSFILELINE( statusptr );                                           \
    (void) PRINT_TRACE( statusptr, 0 );                                          \
    if ( statp_ )                                                             \
      ABORT( statusptr, -2, "INITSTATUS: non-null status pointer" );          \
    else if ( xErrno )                                                     \
      ABORT( statusptr, -16, "INITSTATUS: non-zero xErrno" );              \
  }                                                                           \
  else                                                                        \
    g_AbortHook( "Abort: function %s, file %s, line %d, %s\n"                \
                  "       Null status pointer passed to function\n",          \
                  __func__, __FILE__, __LINE__, "$Id$" );                     \
  } while ( 0 )

#define RETURN( statusptr )                                                   \
  do { if ( 1 )                                                               \
  {                                                                           \
    SETSTATUSFILELINE( statusptr );                                           \
    if ( (statusptr)->statusCode )                                            \
      (void) PRINT_ERROR( statusptr, "RETURN:" );                                \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    if ( xErrno )                                                          \
      ABORT( statusptr, -32, "RETURN: untrapped XLAL error" );                \
    return;                                                                   \
  }                                                                           \
  } while ( 0 )

#define ATTATCHSTATUSPTR(statusptr)                                           \
  do { if ( !(statusptr)->statusPtr )                                         \
  {                                                                           \
    (statusptr)->statusPtr = (ProcStatus *)MYCalloc( 1, sizeof( ProcStatus ) );\
    if ( !(statusptr)->statusPtr )                                            \
      ABORT( statusptr, -4, "ATTATCHSTATUSPTR: memory allocation error" );    \
    (statusptr)->statusPtr->level = (statusptr)->level + 1;                   \
  }                                                                           \
  else                                                                        \
    ABORT( statusptr, -2, "ATTATCHSTATUSPTR: non-null status pointer" );      \
  } while ( 0 )

#define DETATCHSTATUSPTR( statusptr )                                         \
  do { if ( (statusptr)->statusPtr )                                          \
  {                                                                           \
    FREESTATUSPTR( statusptr );                                               \
    (statusptr)->statusCode = 0;                                              \
    (statusptr)->statusDescription = NULL;                                    \
  }                                                                           \
  else                                                                        \
    ABORT( statusptr, -8, "DETATCHSTATUSPTR: null status pointer" );          \
  } while ( 0 )

#define ABORT( statusptr, code, mesg )                                        \
  do { if ( (statusptr)->statusPtr ) FREESTATUSPTR( statusptr );              \
    SETSTATUS( statusptr, code, mesg );                                       \
    if ( code )                                                               \
      (void) PRINT_ERROR( statusptr, "ABORT:" );                                 \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    return;                                                                   \
  } while ( 0 )

#define ASSERT( assertion, statusptr, code, mesg )                            \
  do { if ( !(assertion) )                                                    \
  {                                                                           \
    if ( (statusptr)->statusPtr )                                               \
      FREESTATUSPTR( statusptr );                                             \
    SETSTATUS( statusptr, code, mesg );                                       \
    (void) PRINT_ERROR( statusptr, "Assertion \"" #assertion "\" failed:" );     \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    return;                                                                   \
  }                                                                           \
  } while ( 0 )

#define TRY( func, statusptr )                                                \
  do { if ( (func), (statusptr)->statusPtr->statusCode )                      \
  {                                                                           \
    SETSTATUS( statusptr, -1, "Recursive error" );                            \
    (void) PRINT_ERROR( statusptr, "Function call \"" #func "\" failed:" );      \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    return;                                                                   \
  }                                                                           \
  } while ( 0 )

#define CHECKSTATUSPTR( statusptr )                                           \
  do { if ( (statusptr)->statusPtr->statusCode )                              \
  {                                                                           \
    SETSTATUS( statusptr, -1, "Recursive error" );                            \
    (void) PRINT_ERROR( statusptr, "CHECKSTATUSPTR:" );                          \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    return;                                                                   \
  }                                                                           \
  } while ( 0 )

#define FREESTATUSPTR( statusptr )                                            \
  do                                                                          \
  {                                                                           \
    ProcStatus *next_ = (statusptr)->statusPtr->statusPtr;                     \
    MYFree( (statusptr)->statusPtr );                                        \
    (statusptr)->statusPtr = next_;                                           \
  }                                                                           \
  while ( (statusptr)->statusPtr )

#define REPORTSTATUS( statusptr )                                             \
  do                                                                          \
  {                                                                           \
    ProcStatus *ptr_;                                                          \
    for ( ptr_ = (statusptr); ptr_; ptr_ = ptr_->statusPtr )                  \
    {                                                                         \
      PrintError( "\nLevel %i: %s\n", ptr_->level, ptr_->Id );             \
      if ( ptr_->statusCode )                                                 \
        PrintError( "\tStatus code %i: %s\n", ptr_->statusCode,            \
                       ptr_->statusDescription );                             \
      else                                                                    \
        PrintError( "\tStatus code 0: Nominal\n" );                        \
      PrintError( "\tfunction %s, file %s, line %i\n",                     \
                     ptr_->function, ptr_->file, ptr_->line );                \
    }                                                                         \
  } while ( 0 )

#define BEGINFAIL( statusptr )                                                \
do {                                                                          \
  if ( !(statusptr) )                                                         \
    ABORT( statusptr, -8, "BEGINFAIL: null status pointer" );                 \
  if ( !( (statusptr)->statusPtr ) )                                          \
    ABORT( statusptr, -8, "BEGINFAIL: null status pointer pointer" );         \
  if ( (statusptr)->statusPtr->statusCode ) {                                 \
    ProcStatus *statusPtrSave_ = (statusptr)->statusPtr;                       \
    (statusptr)->statusPtr = NULL;                                            \
    ATTATCHSTATUSPTR( statusptr );                                            \
    do

#define ENDFAIL( statusptr )                                                  \
    while ( 0 );                                                              \
    DETATCHSTATUSPTR( statusptr );                                            \
    (statusptr)->statusPtr = statusPtrSave_;                                  \
    SETSTATUS( statusptr, -1, "Recursive error" );                            \
    (void) PRINT_ERROR( statusptr, "ENDFAIL:" );                                 \
    (void) PRINT_TRACE( statusptr, 1 );                                          \
    return;                                                                   \
  }                                                                           \
} while ( 0 )

#define SETSTATUSFILELINE( statusptr ) \
  ( ( void ) ( (statusptr)->file = __FILE__, (statusptr)->line = __LINE__ ) )

#define SETSTATUS( statusptr, code, mesg )                                    \
  ( SETSTATUSFILELINE( statusptr ),                                           \
    (statusptr)->statusDescription = (mesg),                                  \
    (statusptr)->statusCode = (code) )


#ifdef __cplusplus
}
#endif
#endif

