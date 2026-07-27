/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include "basic_gList.h"
#include "basic_Error.h"
#include "basic_String.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

struct tagGListItem {
	struct tagGListItem *next;
	GValue value;
};

struct tagGList {
	struct tagGListItem *head;
};

/* LIST ITEM ROUTINES */

GListItem * GListItemAlloc(size_t size)
{
	GListItem *item;
	item = XMalloc(sizeof(*item) + size);
	if (!item)
		X_ERROR_NULL(X_ENOMEM);
	item->value.size = size;
	return item;
}

GListItem * GListItemRealloc(GListItem *item, size_t size)
{
	if (item == NULL)
		return GListItemAlloc(size);
	item = XRealloc(item, sizeof(*item) + size);
	if (!item)
		X_ERROR_NULL(X_ENOMEM);
	item->value.size = size;
	return item;
}

GListItem * GListItemSet(GListItem *item, const void *data, size_t size, TYPECODE type)
{
	if (GValueSet(&item->value, data, size, type) == NULL)
		X_ERROR_NULL(X_EFUNC);
	return item;
}

GListItem * GListItemSetValue(GListItem *item, const GValue *value)
{
	if (GValueCopy(&item->value, value) == NULL)
		X_ERROR_NULL(X_EFUNC);
	return item;
}

GListItem * GListItemDuplicate(const GListItem *item)
{
	size_t size = sizeof(GListItem) + item->value.size;
	GListItem *copy = XMalloc(size);
	if (!copy)
		X_ERROR_NULL(X_ENOMEM);
	return memcpy(copy, item, size);
}

const GValue * GListItemGetValue(const GListItem *item)
{
	return &item->value;
}

TYPECODE GListItemGetValueType(const GListItem *item)
{
	const GValue *value = GListItemGetValue(item);
	if (value == NULL)
		X_ERROR(X_EFUNC);
	return GValueGetType(value);
}

void * GListItemGetValueData(void * data, size_t size, TYPECODE type, const GListItem *item)
{
	const GValue *value = GListItemGetValue(item);
	if (value == NULL)
		X_ERROR_NULL(X_EFUNC);
	return GValueGetData(data, size, type, value);
}

void * GListItemGetBLOBValue(const GListItem *item)
{
	const GValue *value = GListItemGetValue(item);
	if (value == NULL)
		X_ERROR_NULL(X_EFUNC);
	return GValueGetBLOB(value);
}

/* warning: shallow pointer */
const char * GListItemGetStringValue(const GListItem *item)
{
	const GValue *value = GListItemGetValue(item);
	if (value == NULL)
		X_ERROR_NULL(X_EFUNC);
	return GValueGetString(value);
}

#define DEFINE_GET_FUNC(TYPE, FAILVAL) \
	TYPE GListItemGet ## TYPE ## Value(const GListItem *item) \
	{ \
		const GValue *value = GListItemGetValue(item); \
		if (value == NULL) \
			X_ERROR_VAL(FAILVAL, X_EFUNC); \
		return GValueGet ## TYPE (value); \
	}

DEFINE_GET_FUNC(CHAR, X_FAILURE)
DEFINE_GET_FUNC(INT2, X_FAILURE)
DEFINE_GET_FUNC(INT4, X_FAILURE)
DEFINE_GET_FUNC(INT8, X_FAILURE)
DEFINE_GET_FUNC(UCHAR, X_FAILURE)
DEFINE_GET_FUNC(UINT2, X_FAILURE)
DEFINE_GET_FUNC(UINT4, X_FAILURE)
DEFINE_GET_FUNC(UINT8, X_FAILURE)
DEFINE_GET_FUNC(REAL4, X_REAL4_FAIL_NAN)
DEFINE_GET_FUNC(REAL8, X_REAL8_FAIL_NAN)
DEFINE_GET_FUNC(COMPLEX8, X_REAL4_FAIL_NAN)
DEFINE_GET_FUNC(COMPLEX16, X_REAL8_FAIL_NAN)

#undef DEFINE_GET_FUNC

REAL8 GListItemGetValueAsREAL8(const GListItem *item)
{
	const GValue *value = GListItemGetValue(item);
	if (value == NULL)
		X_ERROR_REAL8(X_EFUNC);
	return GValueGetAsREAL8(value);
}

/* LIST ROUTINES */

void DestroyGList(GList *list)
{
	if (list != NULL) {
		GListItem *item = list->head;
		while (item) {
			GListItem *next = item->next;
			MYFree(item);
			item = next;
		}
		MYFree(list);
	}
	return;
}

GList * CreateGList(void)
{
	return XCalloc(1, sizeof(GList));
}

GList * GListDuplicate(const GList *list)
{
	GList *newlist;
	const GListItem *item = list->head;
	GListItem *head = NULL;
	GListItem *prev = NULL;

	while (item) {
		GListItem *copy = NULL;
		copy = GListItemDuplicate(item);
		copy->next = NULL;
		if (prev == NULL)
			head = copy;
		else
			prev->next = copy;
		prev = copy;
		item = item->next;
	}

	newlist = CreateGList();
	newlist->head = head;
	return newlist;
}

int GListReverse(GList *list)
{
	if (list != NULL) {
		GListItem *item = list->head;
		GListItem *tsil = NULL; // reversed list
		while (item) {
			GListItem *next = item->next;
			item->next = tsil;
			tsil = item;
			item = next;
		}
		list->head = tsil;
	}
	return 0;
}

static GListItem *GListSortMerge(GListItem *a, GListItem *b, int (*cmp)(const GValue *, const GValue *, void *), void *thunk)
{
	GListItem *result = NULL;
	if (a == NULL)
		return b;
	if (b == NULL)
		return a;

	if (cmp(&a->value, &b->value, thunk) <= 0) {
		result = a;
		result->next = GListSortMerge(a->next, b, cmp, thunk);
	} else {
		result = b;
		result->next = GListSortMerge(a, b->next, cmp, thunk);
	}

	return result;
}

int GListSort(GList *list, int (*cmp)(const GValue *, const GValue *, void *), void *thunk)
{
	GListItem *slow;
	GListItem *fast;
	GList head;
	GList tail;

	if (list->head == NULL || list->head->next == NULL)
		return 0;

	/* find midpoint */
	slow = list->head;
	fast = slow->next;
	while (fast != NULL) {
		fast = fast->next;
		if (fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	/* split at midpoint to get back half */
	head.head = list->head;
	tail.head = slow->next;
	slow->next = NULL;

	/* recursively sort front and back */
	GListSort(&head, cmp, thunk);
	GListSort(&tail, cmp, thunk);

	/* merge sort front and back */
	list->head = GListSortMerge(head.head, tail.head, cmp, thunk);
	return 0;
}

size_t GListSize(const GList *list)
{
	size_t size = 0;
	if (list != NULL) {
		const GListItem *item = list->head;
		while (item) {
			item = item->next;
			++size;
		}
	}
	return size;
}

void GListForeach(GList *list, void (*func)(GValue *, void *), void *thunk)
{
	if (list != NULL) {
		GListItem *item = list->head;
		while (item) {
			func(&item->value, thunk);
			item = item->next;
		}
	}
	return;
}

GListItem * GListPop(GList *list)
{
	GListItem *item = NULL;
	if (list != NULL && list->head != NULL) {
		item = list->head;
		list->head = item->next;
	}
	return item;
}

GListItem * GListLast(GList *list)
{
	GListItem *item = NULL;
	if (list != NULL && list->head != NULL) {
		item = list->head;
		while (item->next != NULL)
			item = item->next;
	}
	return item;
}

GListItem * GListFind(GList *list, int (*func)(const GValue *, void *), void *thunk)
{
	GListItem *item = NULL;
	if (list != NULL) {
		item = list->head;
		while (item) {
			if (func(&item->value, thunk))
				break;
			item = item->next;
		}
	}
	return item;
}

static int GListFindValueFunc(const GValue *value, void *thunk)
{
	const GValue *target = (const GValue *)thunk;
	return GValueEqual(value, target);
}

GListItem * GListFindValue(GList *list, const GValue *value)
{
	void *thunk = (void *)(uintptr_t)value; /* discard const qualifier */
	return GListFind(list, GListFindValueFunc, thunk);
}

int GListReplace(GList *list, int (*func)(const GValue *, void *), void *thunk, const GValue *replace)
{
	size_t size = GValueGetSize(replace);
	if (list != NULL) {
		GListItem *item = list->head;
		GListItem *prev = NULL;
		while (item) {
			if (func(&item->value, thunk)) { /* found it! */
				GListItem *orig = item;

				if (GValueGetSize(&item->value) != size)
					item = GListItemRealloc(orig, size);

				GListItemSetValue(item, replace);

				/* repair links if necessary */
				if (orig != item) {
					if (prev == NULL) /* head is changed */
						list->head = item;
					else
						prev->next = item;
				}

				return 0;
			}
			prev = item;
			item = item->next;
		}
	}
	return -1; /* not found */
}

int GListReplaceAll(GList *list, int (*func)(const GValue *, void *), void *thunk, const GValue *replace)
{
	int replaced = 0;
	size_t size = GValueGetSize(replace);
	if (list != NULL) {
		GListItem *item = list->head;
		GListItem *prev = NULL;
		while (item) {
			if (func(&item->value, thunk)) { /* found it! */
				GListItem *orig = item;

				if (GValueGetSize(&item->value) != size)
					item = GListItemRealloc(orig, size);

				GListItemSetValue(item, replace);

				/* repair links if necessary */
				if (orig != item) {
					if (prev == NULL) /* head is changed */
						list->head = item;
					else
						prev->next = item;
				}

				++replaced;
			}
			prev = item;
			item = item->next;
		}
	}
	return replaced;
}

int GListReplaceValue(GList *list, const GValue *value, const GValue *replace)
{
	void *thunk = (void *)(uintptr_t)value; /* discard const qualifier */
	return GListReplace(list, GListFindValueFunc, thunk, replace);
}

int GListReplaceValueAll(GList *list, const GValue *value, const GValue *replace)
{
	void *thunk = (void *)(uintptr_t)value; /* discard const qualifier */
	return GListReplaceAll(list, GListFindValueFunc, thunk, replace);
}

int GListRemove(GList *list, int (*func)(const GValue *, void *), void *thunk)
{
	if (list != NULL) {
		GListItem *item = list->head;
		GListItem *prev = NULL;
		while (item) {
			if (func(&item->value, thunk)) { /* found it! */
				if (prev == NULL) /* head is removed */
					list->head = item->next;
				else
					prev->next = item->next;
				MYFree(item);
				return 0;
			}
			prev = item;
			item = item->next;
		}
	}
	return -1; /* not found */
}

int GListRemoveAll(GList *list, int (*func)(const GValue *, void *), void *thunk)
{
	int removed = 0;
	if (list != NULL) {
		GListItem *item = list->head;
		GListItem *prev = NULL;
		while (item) {
			if (func(&item->value, thunk)) { /* found it! */
				GListItem *next = item->next;
				if (prev == NULL) /* head is removed */
					list->head = next;
				else
					prev->next = next;
				MYFree(item);
				item = next;
				++removed;
			} else {
				prev = item;
				item = item->next;
			}
		}
	}
	return removed;
}

int GListRemoveValue(GList *list, const GValue *value)
{
	void *thunk = (void *)(uintptr_t)value; /* discard const qualifier */
	return GListRemove(list, GListFindValueFunc, thunk);
}

int GListRemoveValueAll(GList *list, const GValue *value)
{
	void *thunk = (void *)(uintptr_t)value; /* discard const qualifier */
	return GListRemoveAll(list, GListFindValueFunc, thunk);
}

void GListIterInit(GListIter *iter, GList *list)
{
	iter->next = list == NULL ? NULL : list->head;
}

GListItem * GListIterNext(GListIter *iter)
{
	if (iter->next == NULL) /* iteration terminated */
		return NULL;
	GListItem *next = iter->next;
	iter->next = next->next;
	return next;
}

int GListAdd(GList *list, const void *data, size_t size, TYPECODE type)
{
	GListItem *item;

	X_CHECK(list != NULL, X_EFAULT);

	item = GListItemAlloc(size);
	if (item == NULL)
		X_ERROR(X_EFUNC);

	if (GListItemSet(item, data, size, type) == NULL) {
		MYFree(item);
		X_ERROR(X_EFUNC);
	}

	item->next = list->head;
	list->head = item;
	return 0;
}

int GListAddValue(GList *list, const GValue *value)
{
	TYPECODE type = GValueGetType(value);
	size_t size = GValueGetSize(value);
	const void * data = GValueGetDataPtr(value);
	return GListAdd(list, data, size, type);
}

int GListAddBLOBValue(GList *list, const void *blob, size_t size)
{
	return GListAdd(list, blob, size, UCHAR_TYPE_CODE);
}

int GListAddStringValue(GList *list, const char *string)
{
	size_t size = strlen(string) + 1;
	return GListAdd(list, string, size, CHAR_TYPE_CODE);
}

#define DEFINE_ADD_FUNC(TYPE, TCODE) \
	int GListAdd ## TYPE ## Value (GList *list, TYPE value) \
	{ return GListAdd(list, &value, sizeof(value), TCODE); } \

DEFINE_ADD_FUNC(CHAR, CHAR_TYPE_CODE)
DEFINE_ADD_FUNC(INT2, INT2_TYPE_CODE)
DEFINE_ADD_FUNC(INT4, INT4_TYPE_CODE)
DEFINE_ADD_FUNC(INT8, INT8_TYPE_CODE)
DEFINE_ADD_FUNC(UCHAR, UCHAR_TYPE_CODE)
DEFINE_ADD_FUNC(UINT2, UINT2_TYPE_CODE)
DEFINE_ADD_FUNC(UINT4, UINT4_TYPE_CODE)
DEFINE_ADD_FUNC(UINT8, UINT8_TYPE_CODE)
DEFINE_ADD_FUNC(REAL4, REAL4_TYPE_CODE)
DEFINE_ADD_FUNC(REAL8, REAL8_TYPE_CODE)
DEFINE_ADD_FUNC(COMPLEX8, COMPLEX8_TYPE_CODE)
DEFINE_ADD_FUNC(COMPLEX16, COMPLEX16_TYPE_CODE)

#undef DEFINE_ADD_FUNC

struct GListAsStringAppendValueFuncParams {char *s; int first;};

static void GListAsStringAppendValueFunc(GValue *value, void *thunk)
{
	struct GListAsStringAppendValueFuncParams *p = (struct GListAsStringAppendValueFuncParams *)(thunk);
	if (p->first)
		p->first = 0;
	else
		p->s = XStringAppend(p->s, ", ");
	p->s = GValueAsStringAppend(p->s, value);
	return;
}

char * GListAsStringAppend(char *s, const GList *list)
{
	GList *l = (GList *)(uintptr_t)list; // discarding const qual is harmless

	struct GListAsStringAppendValueFuncParams p = {s, 1};
	p.s = XStringAppend(p.s, "[");
	GListForeach(l, GListAsStringAppendValueFunc, &p);
	p.s = XStringAppend(p.s, "]");
	return p.s;
}

void GListPrint(const GList *list, int fd)
{
	char *s = NULL;
	s = GListAsStringAppend(s, list);
	X_CHECK_VOID(s, X_EFUNC);
#if HAVE_DPRINTF
	dprintf(fd, "%s", s);
#else
	/* hack... */
	switch (fd) {
	case 1:
		fprintf(stdout, "%s", s);
		break;
	case 2:
		fprintf(stderr, "%s", s);
		break;
	default:
		MYFree(s);
		X_ERROR_VOID(X_EIO, "Don't know what to do with file descriptor %d", fd);
	}
#endif
	MYFree(s);
	return;
}
