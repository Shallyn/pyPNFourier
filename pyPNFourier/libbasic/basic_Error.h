/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_ERROR__
#define __INCLUDE_BASIC_ERROR__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include "utils_Datatypes.h"
#include "basic_Debug.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** Prints an error message if error printing is enabled by DEBUG_LEVEL. */
int XPrintError(const char *fmt, ...) _GCC_PRINTF_FORMAT_(1,2);

/** Prints a warning message if warning printing is enabled by DEBUG_LEVEL. */
int XPrintWarning(const char *fmt, ...) _GCC_PRINTF_FORMAT_(1,2);

/** Prints an info message if info printing is enabled by DEBUG_LEVEL. */
int XPrintInfo(const char *fmt, ...) _GCC_PRINTF_FORMAT_(1,2);

/** Prints an error message if error printing is enabled by DEBUG_LEVEL. */
int XVPrintError(const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(1);

/** Prints a warning message if warning printing is enabled by DEBUG_LEVEL. */
int XVPrintWarning(const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(1);

/** Prints an info message if info printing is enabled by DEBUG_LEVEL. */
int XVPrintInfo(const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(1);

/**
 * Print an error message with standard X formatting (if error messages
 * are enabled by DEBUG_LEVEL).
 */
void XPrintErrorMessage(const char *func, const char *file, int line,
                           const char *fmt, ...) _GCC_PRINTF_FORMAT_(4,5);

/**
 * Print an warning message with standard X formatting (if warning messages
 * are enabled by DEBUG_LEVEL).
 */
void XPrintWarningMessage(const char *func, const char *file, int line,
                             const char *fmt, ...) _GCC_PRINTF_FORMAT_(4,5);

/**
 * Print an info message with standard X formatting (if info messages
 * are enabled by DEBUG_LEVEL).
 */
void XPrintInfoMessage(const char *func, const char *file, int line,
                          const char *fmt, ...) _GCC_PRINTF_FORMAT_(4,5);

/**
 * Print an error message with standard X formatting (if error messages
 * are enabled by DEBUG_LEVEL).
 */
void XVPrintErrorMessage(const char *func, const char *file, int line,
                            const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(4);

/**
 * Print an warning message with standard X formatting (if warning messages
 * are enabled by DEBUG_LEVEL).
 */
void XVPrintWarningMessage(const char *func, const char *file, int line,
                              const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(4);

/**
 * Print an error message with standard X formatting (if error messages
 * are enabled by DEBUG_LEVEL).
 */
void XVPrintInfoMessage(const char *func, const char *file, int line,
                           const char *fmt, va_list ap) _GCC_VPRINTF_FORMAT_(4);

/** Prints a progress bar at the "info" verbosity level. */
int XPrintProgressBar(double);

/** Prints a deprecation warning at the "warning" verbosity level. */
#define X_PRINT_DEPRECATION_WARNING(replacement) \
  do { \
    static int _x_print_deprecation_warning_ = 1; \
    if (_x_print_deprecation_warning_) { \
      XPrintWarning( \
        "\nDEPRECATION WARNING: program has invoked obsolete function %s(). " \
        "Please see %s() for information about a replacement.\n", \
        __func__, replacement); \
      _x_print_deprecation_warning_ = 0; \
    } \
  } while(0)

#define X_PRINT_ERROR(...) \
	XPrintErrorMessage(__func__, __FILE__, __LINE__, __VA_ARGS__)

#define X_PRINT_WARNING(...) \
	XPrintWarningMessage(__func__, __FILE__, __LINE__, __VA_ARGS__)

#define X_PRINT_INFO(...) \
	XPrintInfoMessage(__func__, __FILE__, __LINE__, __VA_ARGS__)

#define X_REAL4_FAIL_NAN_INT 0x7fc001a1 /**< Hexadecimal representation of <tt>REAL4</tt> NaN failure bit pattern */
#define X_REAL8_FAIL_NAN_INT MY_INT8_C(0x7ff80000000001a1) /**< Hexadecimal representation of <tt>REAL8</tt> NaN failure bit pattern */

/** Returns the value of the X <tt>REAL4</tt> failure NaN. */
static _INLINE_ REAL4 XREAL4FailNaN(void);
static _INLINE_ REAL4 XREAL4FailNaN(void)
{
    volatile const union {
        INT4 i;
        REAL4 x;
    } val = {
    X_REAL4_FAIL_NAN_INT};
    return val.x;
}

/** Returns the value of the X <tt>REAL8</tt> failure NaN. */
static _INLINE_ REAL8 XREAL8FailNaN(void);
static _INLINE_ REAL8 XREAL8FailNaN(void)
{
    volatile const union {
        INT8 i;
        REAL8 x;
    } val = {
    X_REAL8_FAIL_NAN_INT};
    return val.x;
}

/** Tests if a value is an X <tt>REAL4</tt> failure NaN. */
static _INLINE_ int XIsREAL4FailNaN(REAL4 val);
static _INLINE_ int XIsREAL4FailNaN(REAL4 val)
{
    volatile const union {
        INT4 i;
        unsigned char s[4];
    } a = {
    X_REAL4_FAIL_NAN_INT};
    volatile union {
        REAL4 x;
        unsigned char s[4];
    } b;
    size_t n;
    b.x = val;
    for (n = 0; n < sizeof(val); ++n)
        if (a.s[n] != b.s[n])
            return 0;
    return 1;
}

/** Tests if a value is an X <tt>REAL8</tt> failure NaN. */
static _INLINE_ int XIsREAL8FailNaN(REAL8 val);
static _INLINE_ int XIsREAL8FailNaN(REAL8 val)
{
    volatile const union {
        INT8 i;
        unsigned char s[8];
    } a = {
    X_REAL8_FAIL_NAN_INT};
    volatile union {
        REAL8 x;
        unsigned char s[8];
    } b;
    size_t n;
    b.x = val;
    for (n = 0; n < sizeof(val); ++n)
        if (a.s[n] != b.s[n])
            return 0;
    return 1;
}

/* Here are the macro constants for the fail NaNs. */
#define X_REAL4_FAIL_NAN ( XREAL4FailNaN() ) /**< Floating-point value of the X <tt>REAL4</tt> failure NaN. */
#define X_REAL8_FAIL_NAN ( XREAL8FailNaN() ) /**< Floating-point value of the X <tt>REAL8</tt> failure NaN. */

/* Here are the macros to test for fail NaNs. */
#define X_IS_REAL4_FAIL_NAN(val) XIsREAL4FailNaN(val) /**< Tests if <tt>val</tt> is a X <tt>REAL4</tt> failure NaN. */
#define X_IS_REAL8_FAIL_NAN(val) XIsREAL8FailNaN(val) /**< Tests if <tt>val</tt> is a X <tt>REAL8</tt> failure NaN. */

/** X error numbers and return values. */
enum XErrorValue {
    X_SUCCESS = 0,      /**< Success return value (not an error number) */
    X_FAILURE = -1,     /**< Failure return value (not an error number) */

    /* these are standard error numbers */
    X_ENOENT = 2,        /**< No such file or directory */
    X_EIO = 5,           /**< I/O error */
    X_ENOMEM = 12,       /**< Memory allocation error */
    X_EZERODIV = 13,        /**< tried to divide by zero */
    X_EFAULT = 14,       /**< Invalid pointer */
    X_EINVAL = 22,       /**< Invalid argument */
    X_EDOM = 33,         /**< Input domain error */
    X_ERANGE = 34,       /**< Output range error */
    X_ENOSYS = 38,       /**< Function not implemented */

    /* extended error numbers start at 128 ...
     * should be beyond normal errnos */

    /* these are common errors for X functions */
    X_EFAILED = 128,     /**< Generic failure */
    X_EBADLEN = 129,     /**< Inconsistent or invalid length */
    X_ESIZE = 130,       /**< Wrong size */
    X_EDIMS = 131,       /**< Wrong dimensions */
    X_ETYPE = 132,       /**< Wrong or unknown type */
    X_ETIME = 133,       /**< Invalid time */
    X_EFREQ = 134,       /**< Invalid freqency */
    X_EUNIT = 135,       /**< Invalid units */
    X_ENAME = 136,       /**< Wrong name */
    X_EDATA = 137,       /**< Invalid data */

    /* user-defined errors */
    X_EUSR0 = 200,       /**< User-defined error 0 */
    X_EUSR1 = 201,       /**< User-defined error 1 */
    X_EUSR2 = 202,       /**< User-defined error 2 */
    X_EUSR3 = 203,       /**< User-defined error 3 */
    X_EUSR4 = 204,       /**< User-defined error 4 */
    X_EUSR5 = 205,       /**< User-defined error 5 */
    X_EUSR6 = 206,       /**< User-defined error 6 */
    X_EUSR7 = 207,       /**< User-defined error 7 */
    X_EUSR8 = 208,       /**< User-defined error 8 */
    X_EUSR9 = 209,       /**< User-defined error 9 */

    /* external or internal errors */
    X_ESYS = 254,        /**< System error */
    X_EERR = 255,        /**< Internal error */

    /* specific mathematical and numerical errors start at 256 */

    /* IEEE floating point errors */
    X_EFPINVAL = 256,      /**< IEEE Invalid floating point operation, eg sqrt(-1), 0/0 */
    X_EFPDIV0 = 257,       /**< IEEE Division by zero floating point error */
    X_EFPOVRFLW = 258,     /**< IEEE Floating point overflow error */
    X_EFPUNDFLW = 259,     /**< IEEE Floating point underflow error */
    X_EFPINEXCT = 260,     /**< IEEE Floating point inexact error */

    /* numerical algorithm errors */
    X_EMAXITER = 261,      /**< Exceeded maximum number of iterations */
    X_EDIVERGE = 262,      /**< Series is diverging */
    X_ESING = 263,         /**< Apparent singularity detected */
    X_ETOL = 264,          /**< Failed to reach specified tolerance */
    X_ELOSS = 265,         /**< Loss of accuracy */

    /* failure from within a function call: "or" error number with this */
    X_EFUNC = 1024         /**< Internal function call failed bit: "or" this with existing error number */
};

/** Returns the error message associated with an error number. */
const char *XErrorString(int errnum);

/** Prints an error message for a particular error code in a standard format. */
void XPerror(const char *func, const char *file, int line, int errnum);

/** Prints an error message for the current value of <tt>xErrno</tt>. */
#define X_PERROR( ) XPerror(__func__, __FILE__, __LINE__, xErrno)

/** The X error handler type. */
typedef void XErrorHandlerType(const char *func, const char *file,
                                  int line, int errnum);

/** The default X error handler. */
void XDefaultErrorHandler(const char *func, const char *file, int line,
                             int errnum);
/** A silent X error handler. */
void XSilentErrorHandler(const char *func, const char *file, int line,
                            int errnum);

/* Other useful X error handlers. */
/** The X error handler that raises SIGABRT. */
void XAbortErrorHandler(const char *func, const char *file, int line,
                           int errnum);
/** The X error handler that calls exit. */
void XExitErrorHandler(const char *func, const char *file, int line,
                          int errnum);
/** The X error handler that prints a function call backtrace then raises SIGABRT. */
void XBacktraceErrorHandler(const char *func, const char *file,
                               int line, int errnum);

/** Function to return pointer to the X error handler function pointer. */
XErrorHandlerType **XGetErrorHandlerPtr(void);

/** Sets the error handler to a new handler and returns the old handler. */
XErrorHandlerType *XSetErrorHandler(XErrorHandlerType *
                                          newHandler);

/** Sets the error handler to the default handler and returns the old handler. */
XErrorHandlerType *XSetDefaultErrorHandler(void);
/** Sets the error handler to a silent handler and returns the old handler. */
XErrorHandlerType *XSetSilentErrorHandler(void);

/** Sets the X error number to errnum, returns the new value. */
int XSetErrno(int errnum);

/** Gets the X base error number ignoring the internal-function-failed flag. */
int XGetBaseErrno(void);

/** Clears the X error number, returns the old value. */
int XClearErrno(void);

/** Function to return pointer to the X error number. */
int *XGetErrnoPtr(void);

/* these are the modifiable lvalues for xErrno and XErrorHandler */
#define xErrno ( * XGetErrnoPtr() ) /**< Modifiable lvalue containing the X error number */
#define XErrorHandler ( * XGetErrorHandlerPtr() ) /**< Modifiable lvalue containing the X error handler */

/**
 * A macro to (i) disable the X error handling and preserve the
 * current value of xErrno (ii) perform a statement that involves an
 * X function call and (iii) restore the X error handler and value of
 * xErrno while setting variable errnum to the xErrno set by the
 * statement.
 */
#define X_TRY( statement, errnum ) \
	do { \
		XErrorHandlerType *xSaveErrorHandler; \
		int xSaveErrno; \
		xSaveErrorHandler = XSetSilentErrorHandler(); \
		xSaveErrno = xErrno; \
		XClearErrno(); \
		statement ; \
		errnum = xErrno; \
		xErrno = xSaveErrno; \
		XSetErrorHandler(xSaveErrorHandler); \
	} while (0)

/**
 * Performs the same actions as X_TRY(), but additionally silences
 * any error/warning/etc. messages being printed while statement is
 * executed, regardless of the value of #DEBUG_LEVEL.
 */
#define X_TRY_SILENT( statement, errnum ) \
	do { \
		int xSaveDebugLevel = DEBUG_LEVEL; \
		XClobberDebugLevel(xSaveDebugLevel & ~(E_ERRORBIT | E_WARNINGBIT | E_INFOBIT | E_TRACEBIT)); \
		X_TRY(statement, errnum); \
		XClobberDebugLevel(xSaveDebugLevel); \
	} while (0)

/**
 * Routine to set the X error number and invoke the X error handler.
 * It is used by the error macros.
 */
void XError(const char *func,
                          /**< name of function where the error occurs */
               const char *file,
                          /**< source file name (use the __FILE__ macro) */
               int line,  /**< source line number (use the __LINE__ macro) */
               int errnum /**< error code */
    );
    
#define _X_ERROR_IMPL_(statement, errnum, ...) \
	do { \
		char _X_ERROR_IMPL_buf_[1024]; \
		XStringPrint(_X_ERROR_IMPL_buf_, sizeof(_X_ERROR_IMPL_buf_), "X" __VA_ARGS__); \
		if (_X_ERROR_IMPL_buf_[1] != 0) { \
			X_PRINT_ERROR("%s", &_X_ERROR_IMPL_buf_[1]); \
		} \
		XError(__func__, __FILE__, __LINE__, errnum); \
		statement; \
	} while (0)

#define _X_CHECK_IMPL_(statement, assertion, errnum, ...) \
	do { \
		if (!(assertion)) { \
			char _X_CHECK_IMPL_buf_[1024]; \
			XStringPrint(_X_CHECK_IMPL_buf_, sizeof(_X_CHECK_IMPL_buf_), "X" __VA_ARGS__); \
			if (_X_CHECK_IMPL_buf_[1] != 0) { \
				X_PRINT_ERROR("%s", &_X_CHECK_IMPL_buf_[1]); \
			} else { \
				X_PRINT_ERROR("Check failed: %s", #assertion); \
			} \
			XError(__func__, __FILE__, __LINE__, errnum); \
			statement; \
		} \
	} while (0)

#define X_ERROR_VAL(val, ...) _X_ERROR_IMPL_(return val, __VA_ARGS__)
#define X_ERROR(...) _X_ERROR_IMPL_(return (int)X_FAILURE, __VA_ARGS__)
#define X_ERROR_NULL(...) _X_ERROR_IMPL_(return NULL, __VA_ARGS__)
#define X_ERROR_VOID(...) _X_ERROR_IMPL_(return, __VA_ARGS__)
#define X_ERROR_REAL4(...) _X_ERROR_IMPL_(return X_REAL4_FAIL_NAN, __VA_ARGS__)
#define X_ERROR_REAL8(...) _X_ERROR_IMPL_(return X_REAL8_FAIL_NAN, __VA_ARGS__)
#define X_ERROR_MAIN(...) _X_ERROR_IMPL_(return EXIT_FAILURE, __VA_ARGS__)
#define X_ERROR_FAIL(...) _X_ERROR_IMPL_(goto X_FAIL, __VA_ARGS__)
#define X_CHECK_VAL(val, assertion, ...) _X_CHECK_IMPL_(return val, assertion, __VA_ARGS__)
#define X_CHECK(assertion, ...) _X_CHECK_IMPL_(return (int)X_FAILURE, assertion, __VA_ARGS__)
#define X_CHECK_NULL(assertion, ...) _X_CHECK_IMPL_(return NULL, assertion, __VA_ARGS__)
#define X_CHECK_VOID(assertion, ...) _X_CHECK_IMPL_(return, assertion, __VA_ARGS__)
#define X_CHECK_REAL4(assertion, ...) _X_CHECK_IMPL_(return X_REAL4_FAIL_NAN, assertion, __VA_ARGS__)
#define X_CHECK_REAL8(assertion, ...) _X_CHECK_IMPL_(return X_REAL8_FAIL_NAN, assertion, __VA_ARGS__)
#define X_CHECK_MAIN(assertion, ...) _X_CHECK_IMPL_(return EXIT_FAILURE, assertion, __VA_ARGS__)
#define X_CHECK_FAIL(assertion, ...) _X_CHECK_IMPL_(goto X_FAIL, assertion, __VA_ARGS__)


extern int (*g_RaiseHook) (int, const char *, ...);
extern void (*g_AbortHook) (const char *, ...);

int PrintError(const char *fmt, ...);

int ProcRaise(int sig, const char *fmt, ...);

void ProcAbort(const char *fmt, ...);

int ProcError(ProcStatus * status, const char *statement);

int ProcWarning(ProcStatus * status, const char *warning);

int ProcInfo(ProcStatus * status, const char *info);

int ProcTrace(ProcStatus * status, int exitflg);

int
InitProcStatus(ProcStatus * status, const char *function,
              const char *id, const char *file, const int line);

int PrepareReturn(ProcStatus * status, const char *file, const int line);

int
AttatchStatusPtr(ProcStatus * status, const char *file, const int line);

int
DetatchStatusPtr(ProcStatus * status, const char *file, const int line);

int
PrepareAbort(ProcStatus * status, const INT4 code, const char *mesg,
                const char *file, const int line);

int
PrepareAssertFail(ProcStatus * status, const INT4 code,
                     const char *mesg, const char *statement,
                     const char *file, const int line);

int
CheckStatusPtr(ProcStatus * status, const char *statement,
                  const char *file, const int line);

void FREESTATUSPTR(ProcStatus * status);

void REPORTSTATUS(ProcStatus * status);

#define PRINT_ERROR( statusptr, statement )                                    \
  ( DEBUG_LEVEL & E_ERROR ?                                                 \
    PrintError( "Error[%d] %d: function %s, file %s, line %d, %s\n"       \
        "        %s %s\n", (statusptr)->level, (statusptr)->statusCode,      \
        (statusptr)->function, (statusptr)->file, (statusptr)->line,        \
        (statusptr)->Id, (statement),                    \
        (statusptr)->statusDescription ) : 0 )

#define PRINT_WARNING( statusptr, warning )                                    \
  ( DEBUG_LEVEL & E_WARNING ?                                               \
    PrintError( "Warning[%d]: function %s, file %s, line %d, %s\n"        \
        "        %s\n", (statusptr)->level, (statusptr)->function,          \
        (statusptr)->file, (statusptr)->line, (statusptr)->Id, (warning) )  \
      : 0 )

#define PRINT_INFO( statusptr, info )                                          \
  ( DEBUG_LEVEL & E_INFO ?                                                  \
    PrintError( "Info[%d]: function %s, file %s, line %d, %s\n"          \
        "        %s\n", (statusptr)->level, (statusptr)->function,          \
        (statusptr)->file, (statusptr)->line, (statusptr)->Id, (info) )     \
      : 0 )

#define PRINT_TRACE( statusptr, exitflg ) \
  ( DEBUG_LEVEL & E_TRACE ? \
    PrintError( "%s[%d]: function %s, file %s, line %d, %s\n",      \
        (exitflg) ? "Leave" : "Enter", (statusptr)->level, \
        (statusptr)->function, (statusptr)->file, (statusptr)->line, \
        (statusptr)->Id )     \
      : 0 )

/*
 * Error handlers
 */

typedef int ( *g_errhandler_t )(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    );

#define PROC_ERR_DFLT PROC_ERR_ABRT
extern g_errhandler_t g_errhandler;

extern int PROC_ERR_EXIT(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    );
extern int PROC_ERR_ABRT(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    );
extern int PROC_ERR_RTRN(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    );

#define FUNC_CALL( function, statusptr ) \
  ((function),lal_errhandler(statusptr,#function,__FILE__,__LINE__,"$Id$"))


/*
 * Error codes and corresponding error messages.
 */

#define CEV_FAIL_ERR	X_EFAILED
#define CEV_FAIL_MSG	"operation failed"
#define CEV_NULL_ERR	X_EFAULT
#define CEV_NULL_MSG	"unexpected NULL pointer"
#define CEV_NNULL_ERR	X_EFAULT
#define CEV_NNULL_MSG	"unexpected non-NULL pointer"
#define CEV_NOMEM_ERR	X_ENOMEM
#define CEV_NOMEM_MSG	"out of memory"
#define CEV_RANGE_ERR	X_ERANGE
#define CEV_RANGE_MSG	"parameter out of range"
#define CEV_BADPARM_ERR X_EINVAL
#define CEV_BADPARM_MSG "invalid parameter value"


#ifdef  __cplusplus
}
#endif
#endif

