/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
**/

#ifndef __INCLUDE_GLIST__
#define __INCLUDE_GLIST__
#include "basic_gValue.h"

struct tagGListItem;
typedef struct tagGListItem GListItem;

struct tagGList;
typedef struct tagGList GList;

struct tagGListIter {
	/* private data */
	struct tagGListItem *next;
};
typedef struct tagGListIter GListIter;

GListItem *GListItemAlloc(size_t size);
GListItem *GListItemRealloc(GListItem *item, size_t size);
GListItem *GListItemSet(GListItem *item, const void *data, size_t size, TYPECODE type);
GListItem *GListItemSetValue(GListItem *item, const GValue *value);
GListItem *GListItemDuplicate(const GListItem *item);

const GValue * GListItemGetValue(const GListItem *item);
TYPECODE GListItemGetValueType(const GListItem *item);
void * GListItemGetValueData(void * data, size_t size, TYPECODE type, const GListItem *item);
void * GListItemGetBLOBValue(const GListItem *item);
const char * GListItemGetStringValue(const GListItem *item);
CHAR GListItemGetCHARValue(const GListItem *item);
INT2 GListItemGetINT2Value(const GListItem *item);
INT4 GListItemGetINT4Value(const GListItem *item);
INT8 GListItemGetINT8Value(const GListItem *item);
UCHAR GListItemGetUCHARValue(const GListItem *item);
UINT2 GListItemGetUINT2Value(const GListItem *item);
UINT4 GListItemGetUINT4Value(const GListItem *item);
UINT8 GListItemGetUINT8Value(const GListItem *item);
REAL4 GListItemGetREAL4Value(const GListItem *item);
REAL8 GListItemGetREAL8Value(const GListItem *item);
COMPLEX8 GListItemGetCOMPLEX8Value(const GListItem *item);
COMPLEX16 GListItemGetCOMPLEX16Value(const GListItem *item);

REAL8 GListItemGetValueAsREAL8(const GListItem *item);

void DestroyGList(GList *list);
GList * CreateGList(void);

GList * GListDuplicate(const GList *list);
int GListReverse(GList *list);
int GListSort(GList *list, int (*cmp)(const GValue *, const GValue *, void *), void *thunk);
size_t GListSize(const GList *list);
void GListForeach(GList *list, void (*func)(GValue *, void *), void *thunk);
GListItem * GListPop(GList *list);
GListItem * GListLast(GList *list);
GListItem * GListFind(GList *list, int (*func)(const GValue *, void *), void *thunk);
GListItem * GListFindValue(GList *list, const GValue *value);
int GListReplace(GList *list, int (*func)(const GValue *, void *), void *thunk, const GValue *replace);
int GListReplaceAll(GList *list, int (*func)(const GValue *, void *), void *thunk, const GValue *replace);
int GListReplaceValue(GList *list, const GValue *value, const GValue *replace);
int GListReplaceValueAll(GList *list, const GValue *value, const GValue *replace);
int GListRemove(GList *list, int (*func)(const GValue *, void *), void *thunk);
int GListRemoveAll(GList *list, int (*func)(const GValue *, void *), void *thunk);
int GListRemoveValue(GList *list, const GValue *value);
int GListRemoveValueAll(GList *list, const GValue *value);
void GListIterInit(GListIter *iter, GList *list);
GListItem * GListIterNext(GListIter *iter);

int GListAdd(GList *list, const void *data, size_t size, TYPECODE type);
int GListAddValue(GList *list, const GValue *value);
int GListAddBLOBValue(GList *list, const void *blob, size_t size);
int GListAddStringValue(GList *list, const char *string);
int GListAddCHARValue(GList *list, CHAR value);
int GListAddINT2Value(GList *list, INT2 value);
int GListAddINT4Value(GList *list, INT4 value);
int GListAddINT8Value(GList *list, INT8 value);
int GListAddUCHARValue(GList *list, UCHAR value);
int GListAddUINT2Value(GList *list, UINT2 value);
int GListAddUINT4Value(GList *list, UINT4 value);
int GListAddUINT8Value(GList *list, UINT8 value);
int GListAddREAL4Value(GList *list, REAL4 value);
int GListAddREAL8Value(GList *list, REAL8 value);
int GListAddCOMPLEX8Value(GList *list, COMPLEX8 value);
int GListAddCOMPLEX16Value(GList *list, COMPLEX16 value);

char * GListAsStringAppend(char *s, const GList *list);
void GListPrint(const GList *list, int fd);



#endif