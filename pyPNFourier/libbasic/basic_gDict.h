/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
**/

#ifndef __INCLUDE_GDICT__
#define __INCLUDE_GDICT__
#include "basic_gValue.h"
#include "basic_gList.h"
#define GDICT_HASHSIZE 101

struct tagGDictEntry;
typedef struct tagGDictEntry GDictEntry;

struct tagGDict;
typedef struct tagGDict GDict;

typedef struct {
	/* private data */
	struct tagGDict *dict;
	struct tagGDictEntry *next;
	size_t pos;
}GDictIter;

void GDictEntryFree(GDictEntry *list);
GDictEntry * GDictEntryAlloc(size_t size);
GDictEntry * GDictEntryRealloc(GDictEntry *entry, size_t size);
GDictEntry * GDictEntrySetKey(GDictEntry *entry, const char *key);
GDictEntry * GDictEntrySetValue(GDictEntry *entry, const void *data, size_t size, TYPECODE type);

/* warning: shallow pointer */
const char * GDictEntryGetKey(const GDictEntry *entry);
/* warning: shallow pointer */
const GValue * GDictEntryGetValue(const GDictEntry *entry);

void ClearGDict(GDict *dict);
void DestroyGDict(GDict *dict);
GDict * CreateGDict(void);
int GDictUpdate(GDict *dst, const GDict *src);
GDict * GDictMerge(const GDict *dict1, const GDict *dict2);
GDict * GDictDuplicate(const GDict *orig);

void GDictForeach(GDict *dict, void (*func)(char *, GValue *, void *), void *thunk);
GDictEntry * GDictFind(GDict *dict, int (*func)(const char *, const GValue *, void *), void *thunk);
void GDictIterInit(GDictIter *iter, GDict *dict);
GDictEntry * GDictIterNext(GDictIter *iter);

GList * GDictKeys(const GDict *dict);
GList * GDictValues(const GDict *dict);

int GDictContains(const GDict *dict, const char *key);
size_t GDictSize(const GDict *dict);
int GDictRemove(GDict *dict, const char *key);
int GDictInsert(GDict *dict, const char *key, const void *data, size_t size, TYPECODE type);
int GDictInsertValue(GDict *dict, const char *key, const GValue *value);
int GDictInsertBLOBValue(GDict *dict, const char *key, const void *blob, size_t size);
int GDictInsertStringValue(GDict *dict, const char *key, const char *string);
int GDictInsertCHARValue(GDict *dict, const char *key, CHAR value);
int GDictInsertINT2Value(GDict *dict, const char *key, INT2 value);
int GDictInsertINT4Value(GDict *dict, const char *key, INT4 value);
int GDictInsertINT8Value(GDict *dict, const char *key, INT8 value);
int GDictInsertUCHARValue(GDict *dict, const char *key, UCHAR value);
int GDictInsertUINT2Value(GDict *dict, const char *key, UINT2 value);
int GDictInsertUINT4Value(GDict *dict, const char *key, UINT4 value);
int GDictInsertUINT8Value(GDict *dict, const char *key, UINT8 value);
int GDictInsertREAL4Value(GDict *dict, const char *key, REAL4 value);
int GDictInsertREAL8Value(GDict *dict, const char *key, REAL8 value);
int GDictInsertCOMPLEX8Value(GDict *dict, const char *key, COMPLEX8 value);
int GDictInsertCOMPLEX16Value(GDict *dict, const char *key, COMPLEX16 value);

GDictEntry *GDictLookup(const GDict *dict, const char *key);
void * GDictLookupBLOBValue(const GDict *dict, const char *key);
/* warning: shallow pointer */
const char * GDictLookupStringValue(const GDict *dict, const char *key);
CHAR GDictLookupCHARValue(const GDict *dict, const char *key);
INT2 GDictLookupINT2Value(const GDict *dict, const char *key);
INT4 GDictLookupINT4Value(const GDict *dict, const char *key);
INT8 GDictLookupINT8Value(const GDict *dict, const char *key);
UCHAR GDictLookupUCHARValue(const GDict *dict, const char *key);
UINT2 GDictLookupUINT2Value(const GDict *dict, const char *key);
UINT4 GDictLookupUINT4Value(const GDict *dict, const char *key);
UINT8 GDictLookupUINT8Value(const GDict *dict, const char *key);
REAL4 GDictLookupREAL4Value(const GDict *dict, const char *key);
REAL8 GDictLookupREAL8Value(const GDict *dict, const char *key);
COMPLEX8 GDictLookupCOMPLEX8Value(const GDict *dict, const char *key);
COMPLEX16 GDictLookupCOMPLEX16Value(const GDict *dict, const char *key);
REAL8 GDictLookupValueAsREAL8(const GDict *dict, const char *key);

GDictEntry * GDictPop(GDict *dict, const char *key);
GValue * GDictPopValue(GDict *dict, const char *key);
void * GDictPopBLOBValue(GDict *dict, const char *key);
char * GDictPopStringValue(GDict *dict, const char *key);
CHAR GDictPopCHARValue(GDict *dict, const char *key);
INT2 GDictPopINT2Value(GDict *dict, const char *key);
INT4 GDictPopINT4Value(GDict *dict, const char *key);
INT8 GDictPopINT8Value(GDict *dict, const char *key);
UCHAR GDictPopUCHARValue(GDict *dict, const char *key);
UINT2 GDictPopUINT2Value(GDict *dict, const char *key);
UINT4 GDictPopUINT4Value(GDict *dict, const char *key);
UINT8 GDictPopUINT8Value(GDict *dict, const char *key);
REAL4 GDictPopREAL4Value(GDict *dict, const char *key);
REAL8 GDictPopREAL8Value(GDict *dict, const char *key);
COMPLEX8 GDictPopCOMPLEX8Value(GDict *dict, const char *key);
COMPLEX16 GDictPopCOMPLEX16Value(GDict *dict, const char *key);
REAL8 GDictPopValueAsREAL8(GDict *dict, const char *key);


char * GDictAsStringAppend(char *s, const GDict *dict);
void GDictPrint(const GDict *dict, int fd);


#endif