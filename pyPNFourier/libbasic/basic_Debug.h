/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_DEBUG__
#define __INCLUDE_BASIC_DEBUG__

#include "basic_String.h"

#ifdef __cplusplus
extern "C" {
#endif

int print_debug(char *fmt, ...);
int print_err(const char *fmt, ...);
int print_log(char *fmt, ...);
int print_warning(char *fmt, ...);
int print_error(char *fmt, ...);
int print_out(const char *fmt, ...);


/** lalDebugLevel bit field values */
enum {
    E_ERRORBIT = 0001,   /**< enable error messages */
    E_WARNINGBIT = 0002, /**< enable warning messages */
    E_INFOBIT = 0004,    /**< enable info messages */
    E_TRACEBIT = 0010,   /**< enable tracing messages */
    E_MEMDBGBIT = 0020,  /**< enable memory debugging routines */
    E_MEMPADBIT = 0040,  /**< enable memory padding */
    E_MEMTRKBIT = 0100,  /**< enable memory tracking */
    E_MEMINFOBIT = 0200  /**< enable memory info messages */
};

/** composite lalDebugLevel values */
enum {
    E_NDEBUG = 0,      /**< no debug */
    E_ERROR = E_ERRORBIT,             /**< enable error messages */
    E_WARNING = E_WARNINGBIT,         /**< enable warning messages */
    E_INFO = E_INFOBIT,               /**< enable info messages */
    E_TRACE = E_TRACEBIT,             /**< enable tracing messages */
    E_MSGLVL1 = E_ERRORBIT,           /**< enable error messages */
    E_MSGLVL2 = E_ERRORBIT | E_WARNINGBIT,   /**< enable error and warning messages */
    E_MSGLVL3 = E_ERRORBIT | E_WARNINGBIT | E_INFOBIT,      /**< enable error, warning, and info messages */
    E_MEMDBG = E_MEMDBGBIT | E_MEMPADBIT | E_MEMTRKBIT,     /**< enable memory debugging tools */
    E_MEMTRACE = E_TRACEBIT | E_MEMDBG | E_MEMINFOBIT,      /**< enable memory tracing tools */
    E_ALLDBG = ~E_NDEBUG      /**< enable all debugging */
};

#define DEBUG_LEVEL (XGetDebugLevel())
int XGetDebugLevel(void);
void XClobberDebugLevel(int level);

#ifdef __cplusplus
}
#endif
#endif

