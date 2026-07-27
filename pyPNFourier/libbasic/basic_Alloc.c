/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel:
* 20xx.xx.xx, LOC
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "basic_Alloc.h"
#include "basic_Error.h"

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

/* global variables */
size_t g_MallocTotal = 0;	/**< current amount of memory allocated by process */
size_t g_MallocTotalPeak = 0;	/**< peak amount of memory allocated so far */

/*
 *
 * X Routines.
 *
 */

#define X_TEST_POINTER( ptr, size )                                    \
    if ( ! (ptr) && (size) )                                              \
       X_ERROR_NULL( X_ENOMEM );                                    \
    else (void)(0)
#define X_TEST_POINTER_LONG( ptr, size, file, line )                   \
    if ( ! (ptr) && (size) )                                              \
    {                                                                     \
       XPrintError( "XError - %s in %s:%d", __func__, file, line ); \
       X_ERROR_NULL( X_ENOMEM );                                    \
    }                                                                     \
    else (void)(0)
#define X_TEST_POINTER_ALIGNED( ptr, size, retval )                    \
    if ( ! (ptr) && (size) && (retval) )                                  \
       X_ERROR_NULL( X_ENOMEM );                                    \
    else (void)(0)
#define X_TEST_POINTER_ALIGNED_LONG( ptr, size, retval, file, line )   \
    if ( ! (ptr) && (size) && (retval) )                                  \
    {                                                                     \
       XPrintError( "XError - %s in %s:%d", __func__, file, line ); \
       X_ERROR_NULL( X_ENOMEM );                                    \
    }                                                                     \
    else (void)(0)

void *(XMalloc) (size_t n) {
    void *p;
    p = MallocShort(n);
    X_TEST_POINTER(p, n);
    return p;
}

void *XMallocLong(size_t n, const char *file, int line)
{
    void *p;
    p = MallocLong(n, file, line);
    X_TEST_POINTER_LONG(p, n, file, line);
    return p;
}

void *(XCalloc) (size_t m, size_t n) {
    void *p;
    p = CallocShort(m, n);
    X_TEST_POINTER(p, m && n);
    return p;
}

void *XCallocLong(size_t m, size_t n, const char *file, int line)
{
    void *p;
    p = CallocLong(m, n, file, line);
    X_TEST_POINTER_LONG(p, m && n, file, line);
    return p;
}

void *(XRealloc) (void *p, size_t n) {
    p = ReallocShort(p, n);
    X_TEST_POINTER(p, n);
    return p;
}

void *XReallocLong(void *p, size_t n, const char *file, int line)
{
    p = ReallocLong(p, n, file, line);
    X_TEST_POINTER_LONG(p, n, file, line);
    return p;
}

void (XFree) (void *p)
{
    if (p)
        FreeShort(p);
    return;
}

void XFreeLong(void *p, const char *file UNUSED, int line UNUSED)
{
    if (p)
        FreeLong(p, file, line);
    return;
}

#include <pthread.h>
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

char *g_MemDbgArgPtr = NULL;   /* set to ptr arg in free or realloc */
char *g_MemDbgRetPtr = NULL;   /* set to ptr returned in alloc functions */
char *g_MemDbgPtr = NULL;      /* set in both cases */
char *g_MemDbgUsrPtr = NULL;   /* avaliable global memory pointer for user */
void **g_MemDbgUsrHndl = NULL; /* avaliable global memory handle for user */
int g_IsMemDbgArgPtr;  /* ( g_MemDbgUsrPtr == g_MemDbgArgPtr ) */
int g_IsMemDbgRetPtr;  /* ( g_MemDbgUsrPtr == g_MemDbgRetPtr ) */
int g_IsMemDbgPtr;     /* ( g_MemDbgUsrPtr == g_MemDbgPtr ) */

enum { nprefix = 2 };
static const size_t prefix = nprefix * sizeof(size_t);
static const size_t padFactor = 2;
static const size_t padding = 0xDeadBeef;
static const size_t repadding = 0xBeefDead;
static const size_t magic = 0xABadCafe;

#define allocsz(n) ((DEBUG_LEVEL & E_MEMPADBIT) ? (padFactor * (n) + prefix) : (n))

static struct allocNode {
    void *addr;
    size_t size;
    const char *file;
    int line;
} **alloc_data = NULL;		/* Allocation hash table with open addressing and linear probing */
static int alloc_data_len = 0;	/* Size of the memory block 'alloc_data', in number of elements */
static int alloc_n = 0;		/* Number of valid elements in the hash */
static int alloc_q = 0;		/* Number of non-NULL elements in the hash */

/* Special allocation hash table element value to indicate elements that have been deleted */
static const void *hash_del = 0;
#define DEL   ((struct allocNode*) &hash_del)

/* Evaluates to the hash value of x, restricted to the length of the allocation hash table */
#define HASHIDX(x)   ((int)( ((intptr_t)( (x)->addr )) % alloc_data_len ))

/* Increment the next hash index, restricted to the length of the allocation hash table */
#define INCRIDX(i)   do { if (++(i) == alloc_data_len) { (i) = 0; } } while(0)

/* Evaluates true if the elements x and y are equal */
#define EQUAL(x, y)   ((x)->addr == (y)->addr)

/* Resize and rebuild the allocation allocation hash table */
UNUSED static int AllocHashTblResize(void)
{
    struct allocNode **old_data = alloc_data;
    int old_data_len = alloc_data_len;
    alloc_data_len = 2;
    while (alloc_data_len < 3*alloc_n) {
        alloc_data_len *= 2;
    }
    alloc_data = calloc(alloc_data_len, sizeof(alloc_data[0]));
    if (alloc_data == NULL) {
        return 0;
    }
    alloc_q = alloc_n;
    for (int k = 0; k < old_data_len; ++k) {
        if (old_data[k] != NULL && old_data[k] != DEL) {
            int i = HASHIDX(old_data[k]);
            while (alloc_data[i] != NULL) {
                INCRIDX(i);
            }
            alloc_data[i] = old_data[k];
        }
    }
    free(old_data);
    return 1;
}

/* Find node in allocation hash table */
UNUSED static struct allocNode *AllocHashTblFind(struct allocNode *x)
{
    struct allocNode *y = NULL;
    if (alloc_data_len > 0) {
        int i = HASHIDX(x);
        while (alloc_data[i] != NULL) {
            y = alloc_data[i];
            if (y != DEL && EQUAL(x, y)) {
                return y;
            }
            INCRIDX(i);
        }
    }
    return NULL;
}

/* Add node to allocation hash table */
UNUSED static int AllocHashTblAdd(struct allocNode *x)
{
    if (2*(alloc_q + 1) > alloc_data_len) {
        /* Resize allocation hash table to preserve maximum 50% occupancy */
        if (!AllocHashTblResize()) {
            return 0;
        }
    }
    int i = HASHIDX(x);
    while (alloc_data[i] != NULL && alloc_data[i] != DEL) {
        INCRIDX(i);
    }
    if (alloc_data[i] == NULL) {
        ++alloc_q;
    }
    ++alloc_n;
    alloc_data[i] = x;
    return 1;
}

/* Extract node from allocation hash table */
UNUSED static struct allocNode *AllocHashTblExtract(struct allocNode *x)
{
    if (alloc_data_len > 0) {
        int i = HASHIDX(x);
        while (alloc_data[i] != NULL) {
            struct allocNode *y = alloc_data[i];
            if (y != DEL && EQUAL(x, y)) {
                alloc_data[i] = DEL;
                --alloc_n;
                if (alloc_n == 0) {
                    /* Free all hash table memory */
                    free(alloc_data);
                    alloc_data = NULL;
                    alloc_data_len = 0;
                    alloc_q = 0;
                } else if (8*alloc_n < alloc_data_len) {
                    /* Resize hash table to preserve minimum 50% occupancy */
                    if (!AllocHashTblResize()) {
                        return NULL;
                    }
                }
                return y;
            }
            INCRIDX(i);
        }
    }
    return NULL;
}

/* Useful function for debugging */
/* Checks to make sure alloc list is OK */
/* Returns 0 if list is corrupted; 1 if list is OK */
UNUSED static int CheckAllocList(void)
{
    int count = 0;
    size_t total = 0;
    for (int k = 0; k < alloc_data_len; ++k) {
        if (alloc_data[k] != NULL && alloc_data[k] != DEL) {
            ++count;
            total += alloc_data[k]->size;
        }
    }
    return count == alloc_n && total == g_MallocTotal;
}

/* Useful function for debugging */
/* Finds the node of the alloc list for the desired alloc */
/* Returns NULL if not found  */
UNUSED static struct allocNode *FindAlloc(void *p)
{
    struct allocNode key = { .addr = p };
    return AllocHashTblFind(&key);
}

static void *PadAlloc(size_t * p, size_t n, int keep, const char *func, const char *file, int line)
{
    size_t i;

    if (!(DEBUG_LEVEL & E_MEMPADBIT)) {
        return p;
    }

    if (!p) {
        return NULL;
    }

    if (DEBUG_LEVEL & E_MEMINFOBIT) {
        XPrintError("%s meminfo: allocating %zu bytes at address %p in %s:%d\n",
                       func, n, p + nprefix, file, line);
    }

    /* store the size in a known position */
    p[0] = n;
    p[1] = magic;

    /* pad the memory */
    for (i = keep ? n : 0; i < padFactor * n; ++i) {
        ((char *) p)[i + prefix] = (char) (i ^ padding);
    }

    pthread_mutex_lock(&mut);
    g_MallocTotal += n;
    g_MallocTotalPeak = (g_MallocTotalPeak > g_MallocTotal) ? g_MallocTotalPeak : g_MallocTotal;
    pthread_mutex_unlock(&mut);

    return (void *) (((char *) p) + prefix);
}

static void *UnPadAlloc(void *p, int keep, const char *func, const char *file, int line)
{
    size_t n;
    size_t i;
    size_t *q;
    char *s;

    if (!(DEBUG_LEVEL & E_MEMPADBIT)) {
        return p;
    }

    if (!p || !(q = ((size_t *) p) - nprefix)) {
        g_RaiseHook(SIGSEGV, "%s error: tried to free NULL pointer in %s:%d\n",
                     func, file, line);
        return NULL;
    }

    n = q[0];
    s = (char *) q;

    if (DEBUG_LEVEL & E_MEMINFOBIT) {
        XPrintError("%s meminfo: freeing %zu bytes at address %p in %s:%d\n",
                       func, n, p, file, line);
    }

    if (n == (size_t)(-1)) {
        g_RaiseHook(SIGSEGV,
                     "%s error: tried to free a freed pointer at address %p in %s:%d\n",
                     func, p, file, line);
        return NULL;
    }

    if (q[1] != magic) {
        g_RaiseHook(SIGSEGV,
                     "%s error: wrong magic for pointer at address %p in %s:%d\n",
                     func, p, file, line);
        return NULL;
    }

    if (((long) n) < 0) {
        g_RaiseHook(SIGSEGV,
                     "%s error: corrupt size descriptor for pointer at address %p in %s:%d\n",
                     func, p, file, line);
        return NULL;
    }

    /* check for writing past end of array: */
    for (i = n; i < padFactor * n; ++i) {
        if (s[i + prefix] != (char) (i ^ padding)) {
            g_RaiseHook(SIGSEGV, "%s error: array bounds overwritten\n"
                         "Byte %ld past end of array has changed\n"
                         "Corrupted address: %p\nArray address: %p\n"
                         "Location: %s:%d\n",
                         func, i - n + 1, s + i + prefix, s + prefix, file, line);
            return NULL;
        }
    }

    /* see if there is enough allocated memory to be freed */
    if (g_MallocTotal < n) {
        g_RaiseHook(SIGSEGV, "%s error: g_MallocTotal too small\n",
                     func);
        return NULL;
    }

    /* repad the memory */
    for (i = keep ? n : 0; i < padFactor * n; ++i) {
        s[i + prefix] = (char) (i ^ repadding);
    }

    q[0] = -1;  /* set negative to detect duplicate frees */
    q[1] = ~magic;

    pthread_mutex_lock(&mut);
    g_MallocTotal -= n;
    pthread_mutex_unlock(&mut);

    return q;
}


static void *PushAlloc(void *p, size_t n, const char *file, int line)
{
    struct allocNode *newnode;
    if (!(DEBUG_LEVEL & E_MEMTRKBIT)) {
        return p;
    }
    if (!p) {
        return NULL;
    }
    if (!(newnode = malloc(sizeof(*newnode)))) {
        return NULL;
    }
    pthread_mutex_lock(&mut);
    newnode->addr = p;
    newnode->size = n;
    newnode->file = file;
    newnode->line = line;
    if (!AllocHashTblAdd(newnode)) {
        free(newnode);
        return NULL;
    }
    pthread_mutex_unlock(&mut);
    return p;
}


static void *PopAlloc(void *p, const char *func, const char *file, int line)
{
    if (!(DEBUG_LEVEL & E_MEMTRKBIT)) {
        return p;
    }
    if (!p) {
        return NULL;
    }
    pthread_mutex_lock(&mut);
    struct allocNode key = { .addr = p };
    struct allocNode *node = AllocHashTblExtract(&key);
    if (node == NULL) {
        pthread_mutex_unlock(&mut);
        g_RaiseHook(SIGSEGV, "%s error: alloc %p not found\n"
                     "Location: %s:%d\n",
                     func, p, file, line);
        return NULL;
    }
    free(node);
    pthread_mutex_unlock(&mut);
    return p;
}


static void *ModAlloc(void *p, void *q, size_t n, const char *func,
                      const char *file, int line)
{
    if (!(DEBUG_LEVEL & E_MEMTRKBIT)) {
        return q;
    }
    if (!p || !q) {
        return NULL;
    }
    pthread_mutex_lock(&mut);
    struct allocNode key = { .addr = p };
    struct allocNode *node = AllocHashTblExtract(&key);
    if (node == NULL) {
        pthread_mutex_unlock(&mut);
        g_RaiseHook(SIGSEGV, "%s error: alloc %p not found\n"
                     "Location: %s:%d\n",
                     func, p, file, line);
        return NULL;
    }
    node->addr = q;
    node->size = n;
    node->file = file;
    node->line = line;
    if (!AllocHashTblAdd(node)) {
        free(node);
        return NULL;
    }
    pthread_mutex_unlock(&mut);
    return q;
}

// alloc memory

void *MallocShort(size_t n)
{
    return (DEBUG_LEVEL & E_MEMDBGBIT) ? MallocLong(n, "unknown", -1) : malloc(n);
}



void *MallocLong(size_t n, const char *file, int line)
{
    void *p;
    void *q;
    // printf("(DEBUG_LEVEL & E_MEMDBGBIT) = %d\n", (DEBUG_LEVEL & E_MEMDBGBIT));
    if (!(DEBUG_LEVEL & E_MEMDBGBIT)) {
        return malloc(n);
    }

    p = malloc(allocsz(n));
    q = PushAlloc(PadAlloc(p, n, 0, "Malloc", file, line), n, file, line);
    g_MemDbgPtr = g_MemDbgRetPtr = q;
    g_IsMemDbgPtr = g_IsMemDbgRetPtr = (g_MemDbgRetPtr == g_MemDbgUsrPtr);
    if (!q) {
        XPrintError("Malloc: failed to allocate %zd bytes of memory\n", n);
        XPrintError("Malloc: %zd bytes of memory already allocated\n", g_MallocTotal);
        if (DEBUG_LEVEL & E_MEMINFOBIT) {
            XPrintError("Malloc meminfo: out of memory\n");
        }
        if (p) {
            free(p);
        }
    }
    return q;
}



void *CallocShort(size_t m, size_t n)
{
    return (DEBUG_LEVEL & E_MEMDBGBIT) ? CallocLong(m, n, "unknown", -1) :
 calloc(m, n);
}



void *CallocLong(size_t m, size_t n, const char *file, int line)
{
    size_t sz;
    void *p;
    void *q;

    if (!(DEBUG_LEVEL & E_MEMDBGBIT)) {
        return calloc(m, n);
    }

    sz = m * n;
    p = malloc(allocsz(sz));
    q = PushAlloc(PadAlloc(p, sz, 1, "LALCalloc", file, line), sz, file, line);
    g_MemDbgPtr = g_MemDbgRetPtr = q;
    g_IsMemDbgPtr = g_IsMemDbgRetPtr = (g_MemDbgRetPtr == g_MemDbgUsrPtr);
    if (!q) {
        XPrintError("Malloc: failed to allocate %zd bytes of memory\n", n);
        XPrintError("Malloc: %zd bytes of memory already allocated\n", g_MallocTotal);
        if (DEBUG_LEVEL & E_MEMINFOBIT) {
            XPrintError("LALCalloc meminfo: out of memory\n");
        }
        if (p) {
            free(p);
        }
    }
    return q ? memset(q, 0, sz) : NULL;
}



void *ReallocShort(void *p, size_t n)
{
    return (DEBUG_LEVEL & E_MEMDBGBIT) ? ReallocLong(p, n, "unknown", -1): realloc(p, n);
}



void *ReallocLong(void *q, size_t n, const char *file, const int line)
{
    void *p;
    if (!(DEBUG_LEVEL & E_MEMDBGBIT)) {
        return realloc(q, n);
    }

    g_MemDbgPtr = g_MemDbgArgPtr = q;
    g_IsMemDbgPtr = g_IsMemDbgArgPtr = (g_MemDbgArgPtr == g_MemDbgUsrPtr);
    if (!q) {
        p = malloc(allocsz(n));
        q = PushAlloc(PadAlloc(p, n, 0, "LALRealloc", file, line), n, file, line);
        if (!q) {
            XPrintError("Malloc: failed to allocate %zd bytes of memory\n", n);
            XPrintError("Malloc: %zd bytes of memory already allocated\n", g_MallocTotal);
            if (DEBUG_LEVEL & E_MEMINFOBIT) {
                XPrintError("LALRealloc meminfo: out of memory\n");
            }
            if (p) {
                free(p);
            }
        }
        return q;
    }

    if (!n) {
        p = UnPadAlloc(PopAlloc(q, "LALRealloc", file, line), 0, "LALRealloc", file, line);
        if (p) {
            free(p);
        }
        return NULL;
    }

    p = UnPadAlloc(q, 1, "LALRealloc", file, line);
    if (!p) {
        return NULL;
    }

    q = ModAlloc(q, PadAlloc(realloc(p, allocsz(n)), n, 1, "LALRealloc", file, line), n, "LALRealloc", file, line);
    g_MemDbgPtr = g_MemDbgRetPtr = q;
    g_IsMemDbgPtr = g_IsMemDbgRetPtr = (g_MemDbgRetPtr == g_MemDbgUsrPtr);

    return q;
}

void (LALFree)(void *q);
void (LALFree)(void *q)
{
    FreeShort(q);
    return;
}
void FreeShort(void *q)
{
    FreeLong(q, "unknown", -1);
    return;
}

void FreeLong(void *q, const char *file, const int line)
{
    void *p;
    if (q == NULL)
        return;
    if (!(DEBUG_LEVEL & E_MEMDBGBIT)) {
        free(q);
        return;
    }
    g_MemDbgPtr = g_MemDbgArgPtr = q;
    g_IsMemDbgPtr = g_IsMemDbgArgPtr = (g_MemDbgArgPtr == g_MemDbgUsrPtr);
    p = UnPadAlloc(PopAlloc(q, "Free", file, line), 0, "Free", file, line);
    if (p) {
        free(p);
    }
    return;
}



void CheckMemoryLeaks(void)
{
    int leak = 0;
    if (!(DEBUG_LEVEL & E_MEMDBGBIT)) {
        return;
    }
    // printf("DEBUG_LEVEL & E_MEMTRKBIT = %d, alloc_data_len = %d\n", DEBUG_LEVEL & E_MEMTRKBIT, alloc_data_len);
    /* alloc_data_len should be zero */
    if ((DEBUG_LEVEL & E_MEMTRKBIT) && alloc_data_len > 0) {
        XPrintError("CheckMemoryLeaks: allocation list\n");
        for (int k = 0; k < alloc_data_len; ++k) {
            if (alloc_data[k] != NULL && alloc_data[k] != DEL) {
                XPrintError("%p: %zu bytes (%s:%d)\n", alloc_data[k]->addr,
                               alloc_data[k]->size, alloc_data[k]->file,
                               alloc_data[k]->line);
            }
        }
        leak = 1;
    }

    /* g_MallocTotal and alloc_n should be zero */
    if ((DEBUG_LEVEL & E_MEMPADBIT) && (g_MallocTotal || alloc_n)) {
        XPrintError("CheckMemoryLeaks: %d allocs, %zd bytes\n", alloc_n, g_MallocTotal);
        leak = 1;
    }

    if (leak) {
        g_RaiseHook(SIGSEGV, "CheckMemoryLeaks: memory leak, we are going to raise Segmentation fault\n");
    } else if (DEBUG_LEVEL & E_MEMINFOBIT) {
        XPrintError
            ("CheckMemoryLeaks meminfo: no memory leaks detected\n");
    }

    return;
}
