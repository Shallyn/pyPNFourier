/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "basic_Error.h"

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif



/* Prints an error message if error printing is enabled by DEBUG_LEVEL. */
int XVPrintError(const char *fmt, va_list ap)
{
    // printf("DEBUG_LEVEL & E_ERROR = %d\n", DEBUG_LEVEL & E_ERROR);
    return (DEBUG_LEVEL & E_ERROR) ? vfprintf(stderr, fmt, ap) : 0;
}

/* Prints a warning message if warning printing is enabled by DEBUG_LEVEL. */
int XVPrintWarning(const char *fmt, va_list ap)
{
    return (DEBUG_LEVEL & E_WARNING) ? vfprintf(stderr, fmt, ap) : 0;
}

/* Prints an info message if info printing is enabled by DEBUG_LEVEL. */
int XVPrintInfo(const char *fmt, va_list ap)
{
    return (DEBUG_LEVEL & E_INFO) ? vfprintf(stderr, fmt, ap) : 0;
}

/* Prints an error message if error printing is enabled by DEBUG_LEVEL. */
int XPrintError(const char *fmt, ...)
{
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    n = XVPrintError(fmt, ap);
    va_end(ap);
    return n;
}

/* Prints a warning message if warning printing is enabled by DEBUG_LEVEL. */
int XPrintWarning(const char *fmt, ...)
{
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    n = XVPrintWarning(fmt, ap);
    va_end(ap);
    return n;
}

/* Prints an info message if info printing is enabled by DEBUG_LEVEL. */
int XPrintInfo(const char *fmt, ...)
{
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    n = XVPrintInfo(fmt, ap);
    va_end(ap);
    return n;
}

/*
 * Prints a standard-formatted error message
 * (if error printing is enabled by DEBUG_LEVEL).
 */

static void XVGetLogTime(unsigned char *ucTime, unsigned long ulBufLen)
{
    struct tm* pstTmSec;
    struct timeval stTmMsec;
	
	if(NULL == ucTime)
	{
		return;
	}
	gettimeofday(&stTmMsec, NULL);
	pstTmSec = localtime(&stTmMsec.tv_sec);
	snprintf((char *)ucTime, ulBufLen - 1, "%04d-%02d-%02d %02d:%02d:%02d %03dms",
            pstTmSec->tm_year + 1900, pstTmSec->tm_mon + 1, pstTmSec->tm_mday, pstTmSec->tm_hour, 
            pstTmSec->tm_min, pstTmSec->tm_sec, stTmMsec.tv_usec / 1000);
	return;
}

#define STR_UCTIME_SIZE 128

void XVPrintErrorMessage(const char *func, const char *file, int line,
                            const char *fmt, va_list ap)
{
	unsigned char ucTime[STR_UCTIME_SIZE] = {0};
    XVGetLogTime(ucTime, sizeof(ucTime));
    // printf("debug: %s_%s_%s\n", func, file, fmt);
    XPrintError("[%s]\033[1;31mError\033[0m", ucTime);
    if (func && *func)
        XPrintError(" - %s", func);
    if (file && *file)
        XPrintError(" (%s:%d)", file, line);
    XPrintError(": ");
    XVPrintError(fmt, ap);
    XPrintError("\n");
    return;
}

void XVPrintWarningMessage(const char *func, const char *file, int line,
                              const char *fmt, va_list ap)
{
	unsigned char ucTime[STR_UCTIME_SIZE] = {0};
    XVGetLogTime(ucTime, sizeof(ucTime));
    XPrintWarning("[%s]\033[1;33mWarning\033[0m", ucTime);
    if (func && *func)
        XPrintWarning(" - %s", func);
    if (file && *file)
        XPrintWarning(" (%s:%d)", file, line);
    XPrintWarning(": ");
    XVPrintWarning(fmt, ap);
    XPrintWarning("\n");
    return;
}

void XVPrintInfoMessage(const char *func, const char *file, int line,
                           const char *fmt, va_list ap)
{
	unsigned char ucTime[STR_UCTIME_SIZE] = {0};
    XVGetLogTime(ucTime, sizeof(ucTime));
    XPrintInfo("[%s]\033[1;37mInfo\033[0m", ucTime);
    if (func && *func)
        XPrintInfo(" - %s", func);
    if (file && *file)
        XPrintInfo(" (%s:%d)", file, line);
    XPrintInfo(": ");
    XVPrintInfo(fmt, ap);
    XPrintInfo("\n");
    return;
}

void XPrintErrorMessage(const char *func, const char *file, int line,
                           const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    XVPrintErrorMessage(func, file, line, fmt, ap);
    va_end(ap);
    return;
}

void XPrintWarningMessage(const char *func, const char *file, int line,
                             const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    XVPrintWarningMessage(func, file, line, fmt, ap);
    va_end(ap);
    return;
}

void XPrintInfoMessage(const char *func, const char *file, int line,
                          const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    XVPrintInfoMessage(func, file, line, fmt, ap);
    va_end(ap);
    return;
}



/*
 * Prints a progress bar at the "info" verbosity level.
 */
int XPrintProgressBar(double fraction)
{
    static const char mrk[] =
        "+++++++++++++++++++++++++++++++++++++++++++++++++)";
    static const char spc[] =
        "-------------------------------------------------)";
    int l = ARR_NUM_ELEM(mrk) - 1;
    int offset =
        floor((fraction < 0.0 ? 0.0 : fraction >
               1.0 ? 1.0 : fraction) * l + 0.5);

    return XPrintInfo("[%s%s %.1f%%", mrk + l - offset, spc + offset,
                         100.0 * fraction);
}

// int g_xErrnoGlobal = 0;

// int *XGetErrnoPtr(void)
// {
//     return &g_xErrnoGlobal;
// }

// XErrorHandlerType *g_xErrorHandlerGlobal = NULL;

// XErrorHandlerType **XGetErrorHandlerPtr(void)
// {
//     return &g_xErrorHandlerGlobal;
// }


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <pthread.h>

pthread_key_t xErrnoKey;
pthread_once_t xErrnoKeyOnce = PTHREAD_ONCE_INIT;
pthread_key_t xErrorHandlerKey;
pthread_once_t xErrorHandlerKeyOnce = PTHREAD_ONCE_INIT;

/* routine to free the X error number pointer */
static void XDestroyErrnoPtr(void *xErrnoPtr)
{
    free(xErrnoPtr);
    return;
}

/* routine to free the X error handler pointer */
static void XDestroyErrorHandlerPtr(void *xErrorHandlerPtr)
{
    free(xErrorHandlerPtr);
    return;
}

/* routine to create the X error number key */
static void XCreateErrnoKey(void)
{
    pthread_key_create(&xErrnoKey, XDestroyErrnoPtr);
    return;
}

/* routine to create the X error handler key */
static void XCreateErrorHandlerKey(void)
{
    pthread_key_create(&xErrorHandlerKey, XDestroyErrorHandlerPtr);
    return;
}

/* return the pointer to the X error number in this thread */
int *XGetErrnoPtr(void)
{
    int *xErrnoPtr;

    /* create key on the first call only */
    pthread_once(&xErrnoKeyOnce, XCreateErrnoKey);

    /* get the pointer to the X error number in this thread */
    xErrnoPtr = pthread_getspecific(xErrnoKey);
    if (!xErrnoPtr) {        /* haven't allocated pointer yet... do it now */
        xErrnoPtr = malloc(sizeof(*xErrnoPtr));
        if (!xErrnoPtr)
            g_AbortHook
                ("could not set X error number: malloc failed\n");
        *xErrnoPtr = 0;      /* raises segv if memory allocation fails */
        /* now set the value of the pointer in this thread in the key */
        if (pthread_setspecific(xErrnoKey, xErrnoPtr))
            g_AbortHook
                ("could not set X error number: pthread_setspecific failed\n");
    }
    return xErrnoPtr;
}

/* return the pointer to the XLAL error handler in this thread */
XErrorHandlerType **XGetErrorHandlerPtr(void)
{
    XErrorHandlerType **xErrorHandlerPtr;

    /* create key on the first call only */
    pthread_once(&xErrorHandlerKeyOnce, XCreateErrorHandlerKey);

    /* get the pointer to the XLAL error handler in this thread */
    xErrorHandlerPtr = pthread_getspecific(xErrorHandlerKey);
    if (!xErrorHandlerPtr) { /* haven't allocated pointer yet... do it now */
        xErrorHandlerPtr = malloc(sizeof(*xErrorHandlerPtr));
        if (!xErrorHandlerPtr)
            g_AbortHook
                ("could not set xlal error handler: malloc failed\n");
        *xErrorHandlerPtr = NULL;    /* raises segv if memory allocation fails */
        /* now set the value of the pointer in this thread in the key */
        if (pthread_setspecific(xErrorHandlerKey, xErrorHandlerPtr))
            g_AbortHook
                ("could not set xlal error handler: pthread_setspecific failed\n");
    }
    return xErrorHandlerPtr;
}

/* Set the X error number to errnum. */
int XSetErrno(int errnum)
{
    if (errnum == 0) {
        xErrno = 0;
        return xErrno;
    }

    /*
     * if this is an error indicating an internal error then set the bit
     * that indicates this; otherwise, xErrno should presumably be zero
     */
    if (errnum & X_EFUNC) {
        xErrno |= X_EFUNC;        /* make sure X_EFUNC bit is set */
        return xErrno;
    }

    /*
     * if xErrno is not zero, probably forgot to deal with previous
     * error
     */
    if (xErrno)
        X_PRINT_WARNING("Ignoring previous error (xErrno=%d) %s\n",
                           xErrno, XErrorString(xErrno));
    xErrno = errnum;
    return xErrno;
}

/* Gets the basic error number ignoring the internal-function-failed flag. */
int XGetBaseErrno(void)
{
    return xErrno & ~X_EFUNC;
}


/* Clears the X error number. */
int XClearErrno(void)
{
    int olderrno = xErrno;
    xErrno = 0;
    return olderrno;
}

/* Set the X error handler to newHandler; return the old handler. */
XErrorHandlerType *XSetErrorHandler(XErrorHandlerType *
                                          newHandler)
{
    XErrorHandlerType *oldHandler;
    oldHandler = XErrorHandler;
    XErrorHandler = newHandler;
    return oldHandler;
}


/* Set the X error handler to the default handler; return the old handler.  */
XErrorHandlerType *XSetDefaultErrorHandler(void)
{
    XErrorHandlerType *oldHandler;
    oldHandler = XErrorHandler;
    XErrorHandler = XDefaultErrorHandler;
    return oldHandler;
}

/* Set the X error handler to a silent handler; return the old handler. */
XErrorHandlerType *XSetSilentErrorHandler(void)
{
    XErrorHandlerType *oldHandler;
    oldHandler = XErrorHandler;
    XErrorHandler = XSilentErrorHandler;
    return oldHandler;
}

const char *XErrorString(int code)
{

    if (code <= 0) {    /* this is a return code, not an error number */
        if (code == 0)
            return "Success";
        else if (code == -1)
            return "Failure";
        else
            return "Unknown return code";
    }

    /* check to see if an internal function call has failed, but the error
     * number was not "or"ed against the mask X_EFUNC */
    if (code == X_EFUNC)
        return "Internal function call failed";

    /* use this to report error strings... deals with possible mask for
     * errors arising from internal function calls */
# define X_ERROR_STRING(s) \
    ( ( code & X_EFUNC ) ? "Internal function call failed: " s : (const char *) s )
    switch (code & ~X_EFUNC) {
        /* these are standard error numbers */
    case X_ENOENT:
        return X_ERROR_STRING("No such file or directory");
    case X_EIO:
        return X_ERROR_STRING("I/O error");
    case X_ENOMEM:
        return X_ERROR_STRING("Memory allocation error");
    case X_EFAULT:
        return X_ERROR_STRING("Invalid pointer");
    case X_EINVAL:
        return X_ERROR_STRING("Invalid argument");
    case X_EDOM:
        return X_ERROR_STRING("Input domain error");
    case X_ERANGE:
        return X_ERROR_STRING("Output range error");
    case X_ENOSYS:
        return X_ERROR_STRING("Function not implemented");

        /* extended error numbers start at 128 ...
         * should be beyond normal errnos */

        /* these are common errors for X functions */
    case X_EFAILED:
        return X_ERROR_STRING("Generic failure");
    case X_EBADLEN:
        return X_ERROR_STRING("Inconsistent or invalid vector length");
    case X_ESIZE:
        return X_ERROR_STRING("Wrong size");
    case X_EDIMS:
        return X_ERROR_STRING("Wrong dimensions");
    case X_ETYPE:
        return X_ERROR_STRING("Wrong or unknown type");
    case X_ETIME:
        return X_ERROR_STRING("Invalid time");
    case X_EFREQ:
        return X_ERROR_STRING("Invalid freqency");
    case X_EUNIT:
        return X_ERROR_STRING("Invalid units");
    case X_ENAME:
        return X_ERROR_STRING("Wrong name");
    case X_EDATA:
        return X_ERROR_STRING("Invalid data");

        /* user-defined errors */
    case X_EUSR0:
        return X_ERROR_STRING("User-defined error 0");
    case X_EUSR1:
        return X_ERROR_STRING("User-defined error 1");
    case X_EUSR2:
        return X_ERROR_STRING("User-defined error 2");
    case X_EUSR3:
        return X_ERROR_STRING("User-defined error 3");
    case X_EUSR4:
        return X_ERROR_STRING("User-defined error 4");
    case X_EUSR5:
        return X_ERROR_STRING("User-defined error 5");
    case X_EUSR6:
        return X_ERROR_STRING("User-defined error 6");
    case X_EUSR7:
        return X_ERROR_STRING("User-defined error 7");
    case X_EUSR8:
        return X_ERROR_STRING("User-defined error 8");
    case X_EUSR9:
        return X_ERROR_STRING("User-defined error 9");

        /* external or internal errors */
    case X_ESYS:
        return X_ERROR_STRING("System error");
    case X_EERR:
        return X_ERROR_STRING("Internal error");

        /* specific mathematical and numerical errors start at 256 */

        /* IEEE floating point errors */
    case X_EFPINVAL:
        return
            X_ERROR_STRING
            ("Invalid floating point operation, eg sqrt(-1), 0/0");
    case X_EFPDIV0:
        return X_ERROR_STRING("Division by zero floating point error");
    case X_EFPOVRFLW:
        return X_ERROR_STRING("Floating point overflow error");
    case X_EFPUNDFLW:
        return X_ERROR_STRING("Floating point underflow error");
    case X_EFPINEXCT:
        return X_ERROR_STRING("Floating point inexact error");

        /* numerical algorithm errors */
    case X_EMAXITER:
        return X_ERROR_STRING("Exceeded maximum number of iterations");
    case X_EDIVERGE:
        return X_ERROR_STRING("Series is diverging");
    case X_ESING:
        return X_ERROR_STRING("Apparent singularity detected");
    case X_ETOL:
        return X_ERROR_STRING("Failed to reach specified tolerance");
    case X_ELOSS:
        return X_ERROR_STRING("Loss of accuracy");

        /* unrecognized error number */
    default:
        return "Unknown error";
    }
# undef X_ERROR_STRING
    return NULL;        /* impossible to get here */
}

/* Print an error message associated with an error number or return code. */
void XPerror(const char *func, const char *file, int line, int code)
{
    if (code > 0)
        XPrintError("X Error");
    else
        XPrintError("X Result");
    if (func && *func)
        XPrintError(" - %s", func);
    if (file && *file)
        XPrintError(" (%s:%d)", file, line);
    XPrintError(": %s\n", XErrorString(code));
    return;
}


/*
 *
 * Here is the default error handler.
 *
 */

/* Default X error handler */
void XDefaultErrorHandler(const char *func, const char *file, int line,
                             int errnum)
{
    XPerror(func, file, line, errnum);
    return;
}

/* Silent X error handler */
void XSilentErrorHandler(const char UNUSED * func,
                            const char UNUSED * file, int UNUSED line,
                            int UNUSED errnum)
{
    return;
}


/*
 *
 * Routine to set the error number and invoke the current error handler.
 *
 */
void XError(const char *func, const char *file, int line, int errnum)
{
    XSetErrno(errnum);
    if (!XErrorHandler)
        XErrorHandler = XDefaultErrorHandler;
    XErrorHandler(func, file, line, xErrno);
    return;
}


/*
 *
 * Useful standard error handlers
 *
 */

/* X error handler to abort on error. */
void XAbortErrorHandler(const char *func, const char *file, int line,
                           int errnum)
{
    XPerror(func, file, line, errnum);
    abort();
}

/* X error handler to exit on error. */
void XExitErrorHandler(const char *func, const char *file, int line,
                          int errnum)
{
    XPerror(func, file, line, errnum);
    exit(1);
}

/* X error handler to abort on error and print a backtrace (if possible). */
void XBacktraceErrorHandler(const char *func, const char *file,
                               int line, int errnum)
{
    XPerror(func, file, line, errnum);
#if defined(HAVE_BACKTRACE) && defined(BACKTRACE_LEVELMAX)
    void *callstack[BACKTRACE_LEVELMAX];
    size_t frames = backtrace(callstack, BACKTRACE_LEVELMAX);
    fprintf(stderr, "backtrace:\n");
    backtrace_symbols_fd(callstack, frames, fileno(stderr));
#endif
    abort();
}

void FREESTATUSPTR(ProcStatus * status);
void REPORTSTATUS(ProcStatus * status);

#undef ProcError
#undef ProcWarning
#undef ProcInfo
#undef ProcTrace

int PrintError(const char *fmt, ...)
{
    int n;
    va_list ap;
    va_start(ap, fmt);
    n = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return n;
}
int ProcRaise(int sig, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    (void) vfprintf(stderr, fmt, ap);
    va_end(ap);
#if defined(HAVE_BACKTRACE) && defined(BACKTRACE_LEVELMAX)
    void *callstack[BACKTRACE_LEVELMAX];
    size_t frames = backtrace(callstack, BACKTRACE_LEVELMAX);
    fprintf(stderr, "backtrace:\n");
    backtrace_symbols_fd(callstack, frames, fileno(stderr));
#endif
// printf("going to raise\n");
    return raise(sig);
}
int (*g_RaiseHook) (int, const char *, ...) = ProcRaise;

void ProcAbort(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    (void) vfprintf(stderr, fmt, ap);
    va_end(ap);
#if defined(HAVE_BACKTRACE) && defined(BACKTRACE_LEVELMAX)
    void *callstack[BACKTRACE_LEVELMAX];
    size_t frames = backtrace(callstack, BACKTRACE_LEVELMAX);
    fprintf(stderr, "backtrace:\n");
    backtrace_symbols_fd(callstack, frames, fileno(stderr));
#endif
    abort();
}
void (*g_AbortHook) (const char *, ...) = ProcAbort;

int ProcError(ProcStatus * status, const char *statement)
{
    int n = 0;
    if (DEBUG_LEVEL & E_ERROR) {
        n = PrintError
            ("Error[%d] %d: function %s, file %s, line %d, %s\n"
             "        %s %s\n", status->level, status->statusCode,
             status->function, status->file, status->line, status->Id,
             statement ? statement : "", status->statusDescription);
    }
    return n;
}



int ProcWarning(ProcStatus * status, const char *warning)
{
    int n = 0;
    if (DEBUG_LEVEL & E_WARNING) {
        n = PrintError
            ("Warning[%d]: function %s, file %s, line %d, %s\n"
             "        %s\n", status->level, status->function, status->file,
             status->line, status->Id, warning);
    }
    return n;
}



int ProcInfo(ProcStatus * status, const char *info)
{
    int n = 0;
    if (DEBUG_LEVEL & E_INFO) {
        n = PrintError("Info[%d]: function %s, file %s, line %d, %s\n"
                          "        %s\n", status->level, status->function,
                          status->file, status->line, status->Id, info);
    }
    return n;
}

int
PrepareAbort(ProcStatus * status, const INT4 code, const char *mesg,
                const char *file, const int line);


int ProcTrace(ProcStatus * status, int exitflg)
{
    int n = 0;
    if (DEBUG_LEVEL & E_TRACE) {
        n = PrintError("%s[%d]: function %s, file %s, line %d, %s\n",
                          exitflg ? "Leave" : "Enter", status->level,
                          status->function, status->file, status->line,
                          status->Id);
    }
    return n;
}

int
InitProcStatus(ProcStatus * status, const char *function, const char *id,
              const char *file, const int line)
{
    int exitcode = 0;
    if (status) {
        INT4 level = status->level;
        exitcode = status->statusPtr ? 1 : 0;
        memset(status, 0, sizeof(ProcStatus));   /* possible memory leak */
        status->level = level > 0 ? level : 1;
        status->Id = id;
        status->function = function;
        status->file = file;
        status->line = line;
        (void) ProcTrace(status, 0);
        if (exitcode) {
            PrepareAbort(status, -2,
                            "INITSTATUS: non-null status pointer", file,
                            line);
        } else if (xErrno) {
            PrepareAbort(status, -16, "INITSTATUS: non-zero xlalErrno",
                            file, line);
            exitcode = 1;
        }
    } else {
        g_AbortHook("Abort: function %s, file %s, line %d, %s\n"
                     "       Null status pointer passed to function\n",
                     function, file, line, id);
    }
    return exitcode;
}

int PrepareReturn(ProcStatus * status, const char *file, const int line)
{
    status->file = file;
    status->line = line;
    if (status->statusCode) {
        (void) ProcError(status, "RETURN:");
    }
    (void) ProcTrace(status, 1);
    if (xErrno) {
        PrepareAbort(status, -32, "RETURN: untrapped XLAL error",
                        file, line);
    }
    return 1;
}



int
AttatchStatusPtr(ProcStatus * status, const char *file, const int line)
{
    int exitcode = 0;
    if (status->statusPtr) {
        PrepareAbort(status, -2,
                        "ATTATCHSTATUSPTR: non-null status pointer", file,
                        line);
        exitcode = 1;
    } else {
        status->statusPtr = (ProcStatus *) calloc(1, sizeof(ProcStatus));
        if (!status->statusPtr) {
            PrepareAbort(status, -4,
                            "ATTATCHSTATUSPTR: memory allocation error",
                            file, line);
            exitcode = 1;
        } else {
            status->statusPtr->level = status->level + 1;
        }
    }
    return exitcode;
}



int
DetatchStatusPtr(ProcStatus * status, const char *file, const int line)
{
    int exitcode = 0;
    if (status->statusPtr) {
        FREESTATUSPTR(status);
        status->statusCode = 0;
        status->statusDescription = NULL;
    } else {
        PrepareAbort(status, -8,
                        "DETATCHSTATUSPTR: null status pointer", file,
                        line);
        exitcode = 1;
    }
    return exitcode;
}



int
PrepareAbort(ProcStatus * status, const INT4 code, const char *mesg,
                const char *file, const int line)
{
    if (status->statusPtr) {
        FREESTATUSPTR(status);
    }
    status->file = file;
    status->line = line;
    status->statusCode = code;
    status->statusDescription = mesg;
    if (code) {
        (void) ProcError(status, "ABORT:");
    }
    (void) ProcTrace(status, 1);
    return 1;
}



int
PrepareAssertFail(ProcStatus * status, const INT4 code, const char *mesg,
                     const char *statement, const char *file,
                     const int line)
{
    if (status->statusPtr) {
        FREESTATUSPTR(status);
    }
    status->file = file;
    status->line = line;
    status->statusCode = code;
    status->statusDescription = mesg;
    if (DEBUG_LEVEL & E_ERROR)
        (void) statement;
    (void) ProcError(status, statement);
    (void) ProcTrace(status, 1);
    return 1;
}



int
CheckStatusPtr(ProcStatus * status, const char *statement,
                  const char *file, const int line)
{
    if (status->statusPtr->statusCode) {
        status->file = file;
        status->line = line;
        status->statusCode = -1;
        status->statusDescription = "Recursive error";
        if (DEBUG_LEVEL & E_ERROR)
            (void) statement;
        (void) ProcError(status, statement);
        (void) ProcTrace(status, 1);
        return 1;
    }
    return 0;
}



/*
 * This function is somewhat dangerous: need to check to see
 * if status->statusPtr is initially null before calling FREESTATUSPTR
 */

void FREESTATUSPTR(ProcStatus * status)
{
    do {
        ProcStatus *next = status->statusPtr->statusPtr;
        free(status->statusPtr);
        status->statusPtr = next;
    }
    while (status->statusPtr);
    return;
}



void REPORTSTATUS(ProcStatus * status)
{
    ProcStatus *ptr;
    for (ptr = status; ptr; ptr = ptr->statusPtr) {
        PrintError("\nLevel %i: %s\n", ptr->level, ptr->Id);
        if (ptr->statusCode) {
            PrintError("\tStatus code %i: %s\n", ptr->statusCode,
                          ptr->statusDescription);
        } else {
            PrintError("\tStatus code 0: Nominal\n");
        }
        PrintError("\tfunction %s, file %s, line %i\n",
                      ptr->function, ptr->file, ptr->line);
    }
    return;
}




/*
 * Error handlers for LALApps applications
 */

#define FAILMSG( stat, func, file, line, id )                                  \
  do {                                                                         \
    if ( DEBUG_LEVEL & E_ERROR )                                            \
    {                                                                          \
      PrintError( "Error[0]: file %s, line %d, %s\n"                        \
          "\tLAL_CALL: Function call `%s' failed.\n", file, line, id, func );  \
    }                                                                          \
    if ( vrbflg )                                                              \
    {                                                                          \
      fprintf(stderr,"Level 0: %s\n\tFunction call `%s' failed.\n"             \
          "\tfile %s, line %d\n", id, func, file, line );                      \
      REPORTSTATUS( stat );                                                    \
    }                                                                          \
  } while( 0 )

int vrbflg = 0;

g_errhandler_t g_errhandler = PROC_ERR_DFLT;

int PROC_ERR_EXIT(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    )
{
  if ( stat->statusCode )
  {
    FAILMSG( stat, func, file, line, id );
    exit( 1 );
  }
  return stat->statusCode;
}

int PROC_ERR_ABRT(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    )
{
  if ( stat->statusCode )
  {
    FAILMSG( stat, func, file, line, id );
    abort();
  }
  return 0;
}

int PROC_ERR_RTRN(
    ProcStatus  *stat,
    const char *func,
    const char *file,
    const int   line,
    volatile const char *id
    )
{
  if ( stat->statusCode )
  {
    FAILMSG( stat, func, file, line, id );
  }
  return stat->statusCode;
}
