/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_ALLOC__
#define __INCLUDE_BASIC_ALLOC__
#include <stddef.h>

#ifdef __cplusplus
extern "C" \{
#endif

#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define _SWAP(a,b,tmp) \
do{typeof(a) tmp = (a);(a)=(b);(b)=tmp;} while(0)

#define SWAP(a,b) \
_SWAP(a,b,__UNIQUE_ID(tmp))

#define STRUCTFREE(vec, struc) \
do{\
if((vec)){\
__PASTE(Destroy,struc)((vec));\
(vec)=NULL;}\
}while(0)

extern size_t g_MallocTotal;
extern size_t g_MallocTotalPeak;

void *XMalloc(size_t n);
void *XMallocLong(size_t n, const char *file, int line);
void *XCalloc(size_t m, size_t n);
void *XCallocLong(size_t m, size_t n, const char *file, int line);
void *XRealloc(void *p, size_t n);
void *XReallocLong(void *p, size_t n, const char *file, int line);
void XFree(void *p);
void XFreeLong(void *p, const char *file, const int line);

#define XMalloc( n )        XMallocLong( n, __FILE__, __LINE__ )
#define XCalloc( m, n )     XCallocLong( m, n, __FILE__, __LINE__ )
#define XRealloc( p, n )    XReallocLong( p, n, __FILE__, __LINE__ )
#define XFree( p )          XFreeLong( p, __FILE__, __LINE__ )

#define MYMalloc( n )        MallocLong( n, __FILE__, __LINE__ )
#define MYCalloc( m, n )     CallocLong( m, n, __FILE__, __LINE__ )
#define MYRealloc( p, n )    ReallocLong( p, n, __FILE__, __LINE__ )
#define MYFree( p )          FreeLong( p, __FILE__, __LINE__ )

extern char *g_MemDbgArgPtr;   /* set to ptr arg in free or realloc */
extern char *g_MemDbgRetPtr;   /* set to ptr returned in alloc functions */
extern char *g_MemDbgPtr;      /* set in both cases */
extern char *g_MemDbgUsrPtr;   /* avaliable global memory pointer for user */
extern void **g_MemDbgUsrHndl; /* avaliable global memory handle for user */
extern int g_IsMemDbgArgPtr;   /* ( g_MemDbgUsrPtr == g_MemDbgArgPtr ) */
extern int g_IsMemDbgRetPtr;   /* ( g_MemDbgUsrPtr == g_MemDbgRetPtr ) */
extern int g_IsMemDbgPtr;      /* ( g_MemDbgUsrPtr == g_MemDbgPtr ) */

void *MallocShort(size_t n);
void *MallocLong(size_t n, const char *file, int line);
void *CallocShort(size_t m, size_t n);
void *CallocLong(size_t m, size_t n, const char *file, int line);
void *ReallocShort(void *p, size_t n);
void *ReallocLong(void *p, size_t n, const char *file, int line);
void FreeShort(void *p);
void FreeLong(void *p, const char *file, int line);

void (CheckMemoryLeaks) (void);


#ifdef __cplusplus
\}
#endif
#endif

