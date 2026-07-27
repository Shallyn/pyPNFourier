/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// #include <config.h>
#include "basic_Debug.h"
#include "basic_Error.h"

#undef s_DebugLevel

#include <pthread.h>
static pthread_once_t p_Once = PTHREAD_ONCE_INIT;
#define INIT_ONCE(init) pthread_once(&p_Once, (init))

static int s_DebugLevel = 0;

void XClobberDebugLevel(int level)
{
    s_DebugLevel = level;
}

static void XSetDebugLevel(void)
{
    int level;
    char *end;
    const char *env = getenv("DEBUG");
    if (env == NULL || *env == '\0')
        return;

    /* parse the environment string */

    /* see if it is an integer */
    level = (int) strtol(env, &end, 0);
    if (*end == '\0') { /* conversion successful */
        s_DebugLevel = level;
        return;
    }

    /* try to parse as a comma-separated list of debug level names */
    const char *const seps = ",";
    const char *token = env;
    do {
        size_t toklen = strcspn(token, seps);
        if (toklen > 0) {
            if (XStringNCaseCompare("NDEBUG", token, toklen) == 0) {
                level |= 0; /* no debugging */
            } else if (XStringNCaseCompare("ERROR", token, toklen) == 0) {
                level |= E_ERRORBIT; /* enable error messages */
            } else if (XStringNCaseCompare("WARNING", token, toklen) == 0) {
                level |= E_WARNINGBIT; /* enable warning messages */
            } else if (XStringNCaseCompare("INFO", token, toklen) == 0) {
                level |= E_INFOBIT; /* enable info messages */
            } else if (XStringNCaseCompare("TRACE", token, toklen) == 0) {
                level |= E_TRACEBIT; /* enable tracing messages */
            } else if (XStringNCaseCompare("MSGLVL1", token, toklen) == 0) {
                level |= E_ERRORBIT; /* enable error messages */
            } else if (XStringNCaseCompare("MSGLVL2", token, toklen) == 0) {
                level |= E_ERRORBIT | E_WARNINGBIT; /* enable error and warning messages */
            } else if (XStringNCaseCompare("MSGLVL3", token, toklen) == 0) {
                level |= E_ERRORBIT | E_WARNINGBIT | E_INFOBIT; /* enable error, warning, and info messages */
            } else if (XStringNCaseCompare("MEMDBG", token, toklen) == 0) {
                level |= E_MEMDBGBIT | E_MEMPADBIT | E_MEMTRKBIT; /* enable memory debugging tools */
            } else if (XStringNCaseCompare("MEMTRACE", token, toklen) == 0) {
                level |= E_TRACEBIT | E_MEMDBG | E_MEMINFOBIT; /* enable memory tracing tools */
            } else if (XStringNCaseCompare("ALLDBG", token, toklen) == 0) {
                level |= ~E_NDEBUG; /* enable all debugging */
            } else {
              /* not a valid debug level name */
                g_AbortHook("%s: could not parse TRAJECTEUK_DEBUG_LEVEL='%s'\n", __func__, env);
                return;
            }
        }
        token += toklen;
    } while (*(token++) != '\0');
    s_DebugLevel = level;
    return;

}

int XGetDebugLevel(void)
{
    INIT_ONCE(XSetDebugLevel);
    return s_DebugLevel;
}

static int myPrintVstderr(const char *fmt, va_list ap)
{
    return vfprintf(stderr, fmt, ap);
}

int print_debug(char *fmt, ...)
{
    int n = 0;
    int len = strlen(fmt);
    char paste[len+25];
    memset(paste, '\0', sizeof(paste));
    char debug[] = "\033[1;32;47mDEBUG\033[0m:";
    strcat(paste, debug);
    strcat(paste, fmt);
    
    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstderr(paste, ap);
    va_end(ap);
    return n;
}

int print_err(const char *fmt, ...)
{
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstderr(fmt, ap);
    va_end(ap);
    return n;
}

int print_log(char *fmt, ...)
{
    int n = 0;
    int len = strlen(fmt);
    char paste[len+25];
    memset(paste, '\0', sizeof(paste));
    char log[] = "\033[1;34;47mLOG\033[0m:";
    strcat(paste, log);
    strcat(paste, fmt);

    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstderr(paste, ap);
    va_end(ap);
    return n;
}

int print_warning(char *fmt, ...)
{
    int n = 0;
    int len = strlen(fmt);
    char paste[len+25];
    memset(paste, '\0', sizeof(paste));
    char war[] = "\033[1;35mWARNING\033[0m:";
    strcat(paste, war);
    strcat(paste, fmt);
    
    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstderr(paste, ap);
    va_end(ap);
    return n;
}

int print_error(char *fmt, ...)
{
    int n = 0;
    int len = strlen(fmt);
    char paste[len+25];
    memset(paste, '\0', sizeof(paste));
    char war[] = "\033[4;31mERROR\033[0m:";
    strcat(paste, war);
    strcat(paste, fmt);
    
    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstderr(paste, ap);
    va_end(ap);
    return n;
}

static int myPrintVstdout(const char *fmt, va_list ap)
{
    return vfprintf(stdout, fmt, ap);
}


int print_out(const char *fmt, ...)
{
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    n = myPrintVstdout(fmt, ap);
    va_end(ap);
    return n;
}

