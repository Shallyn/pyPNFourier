/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include "basic_gDict.h"
#include "basic_Error.h"
#include <string.h>

struct tagGDictEntry {
	struct tagGDictEntry *next;
	char *key;
	GValue value;
};

struct tagGDict{
	size_t size;
	GDictEntry *hashes[];
};

static size_t hash(const char *s)
{
	size_t hashval;
	for (hashval = 0; *s != '\0'; ++s)
		hashval = *s + 31 * hashval;
	return hashval;
}

/* DICT ENTRY ROUTINES */

void GDictEntryFree(GDictEntry *list)
{
	while (list) {
		GDictEntry *next = list->next;
		if (list->key)
			MYFree(list->key);
		MYFree(list);
		list = next;
	}
	return;
}

GDictEntry * GDictEntryAlloc(size_t size)
{
	GDictEntry *entry;
	entry = XMalloc(sizeof(*entry) + size);
	if (!entry)
		X_ERROR_NULL(X_ENOMEM);
	entry->key = NULL;
	entry->value.size = size;
	return entry;
}

GDictEntry * GDictEntryRealloc(GDictEntry *entry, size_t size)
{
	if (entry == NULL)
		return GDictEntryAlloc(size);
	if (entry->value.size == size)
		return entry;
	entry = XRealloc(entry, sizeof(*entry) + size);
	if (!entry)
		X_ERROR_NULL(X_ENOMEM);
	entry->value.size = size;
	return entry;
}

GDictEntry * GDictEntrySetKey(GDictEntry *entry, const char *key)
{
	if (entry->key)
		MYFree(entry->key);
	if ((entry->key = XStringDuplicate(key)) == NULL)
		X_ERROR_NULL(X_EFUNC);
	return entry;
}

GDictEntry * GDictEntrySetValue(GDictEntry *entry, const void *data, size_t size, TYPECODE type)
{
	if (GValueSet(&entry->value, data, size, type) == NULL)
		X_ERROR_NULL(X_EFUNC);
	return entry;
}

/* warning: shallow pointer */
const char * GDictEntryGetKey(const GDictEntry *entry)
{
	return entry->key;
}

/* warning: shallow pointer */
const GValue * GDictEntryGetValue(const GDictEntry *entry)
{
	return &entry->value;
}

/* DICT ROUTINES */

void ClearGDict(GDict *dict)
{
	if (dict) {
		for (size_t i = 0; i < dict->size; ++i)
			GDictEntryFree(dict->hashes[i]);
		memset(dict, 0, sizeof(dict) + GDICT_HASHSIZE * sizeof(*dict->hashes));
		dict->size = GDICT_HASHSIZE;
	}
}

void DestroyGDict(GDict *dict)
{
	if (dict) {
		size_t i;
		for (i = 0; i < dict->size; ++i)
			GDictEntryFree(dict->hashes[i]);
		MYFree(dict);
	}
	return;
}

GDict * CreateGDict(void)
{
	GDict *dict;
	dict = XCalloc(1, sizeof(dict) + GDICT_HASHSIZE * sizeof(*dict->hashes));
	if (!dict)
		X_ERROR_NULL(X_ENOMEM);
	dict->size = GDICT_HASHSIZE;
	return dict;
}

void GDictForeach(GDict *dict, void (*func)(char *, GValue *, void *), void *thunk)
{
	size_t i;
	for (i = 0; i < dict->size; ++i) {
		GDictEntry *entry;
		for (entry = dict->hashes[i]; entry != NULL; entry = entry->next)
			func(entry->key, &entry->value, thunk);
	}
	return;
}

GDictEntry * GDictFind(GDict *dict, int (*func)(const char *, const GValue *, void *), void *thunk)
{
	size_t i;
	for (i = 0; i < dict->size; ++i) {
		GDictEntry *entry;
		for (entry = dict->hashes[i]; entry != NULL; entry = entry->next)
			if (func(entry->key, &entry->value, thunk))
				return entry;
	}
	return NULL;
}

void GDictIterInit(GDictIter *iter, GDict *dict)
{
	iter->dict = dict;
	iter->pos = 0;
	iter->next = NULL;
	return;
}

GDictEntry * GDictIterNext(GDictIter *iter)
{
	while (1) {

		if (iter->next) {
			GDictEntry *entry = iter->next;
			iter->next = entry->next;
			return entry;
		}

		/* check end of iteration */
		if (iter->pos >= iter->dict->size)
			return NULL;

		iter->next = iter->dict->hashes[iter->pos++];
	}
	return NULL;
}

int GDictUpdate(GDict *dst, const GDict *src)
{
	size_t i;
	X_CHECK(dst, X_EFAULT);
	X_CHECK(src, X_EFAULT);
	for (i = 0; i < src->size; ++i) {
		const GDictEntry *entry;
		for (entry = src->hashes[i]; entry != NULL; entry = entry->next) {
			const char *key = GDictEntryGetKey(entry);
			const GValue *value = GDictEntryGetValue(entry);
			if (GDictInsertValue(dst, key, value) < 0)
				X_ERROR(X_EFUNC);
		}
	}
	return X_SUCCESS;
}

GDict * GDictMerge(const GDict *dict1, const GDict *dict2)
{
	GDict *new = CreateGDict();
	X_CHECK_NULL(new, X_EFUNC);
	if (dict1)
		X_CHECK_FAIL(GDictUpdate(new, dict1) == X_SUCCESS, X_EFUNC);
	if (dict2)
		X_CHECK_FAIL(GDictUpdate(new, dict2) == X_SUCCESS, X_EFUNC);
	return new;
X_FAIL:
	DestroyGDict(new);
	return NULL;
}

GDict * GDictDuplicate(const GDict *orig)
{
	return GDictMerge(orig, NULL);
}

GList * GDictKeys(const GDict *dict)
{
	GList *list;
	size_t i;
	list = CreateGList();
	if (!list)
		X_ERROR_NULL(X_EFUNC);
	for (i = 0; i < dict->size; ++i) {
		const GDictEntry *entry;
		for (entry = dict->hashes[i]; entry != NULL; entry = entry->next) {
			const char *key = GDictEntryGetKey(entry);
			if (GListAddStringValue(list, key) < 0) {
				DestroyGList(list);
				X_ERROR_NULL(X_EFUNC);
			}
		}
	}
	return list;
}

GList * GDictValues(const GDict *dict)
{
	GList *list;
	size_t i;
	list = CreateGList();
	if (!list)
		X_ERROR_NULL(X_EFUNC);
	for (i = 0; i < dict->size; ++i) {
		const GDictEntry *entry;
		for (entry = dict->hashes[i]; entry != NULL; entry = entry->next) {
			const GValue *value = GDictEntryGetValue(entry);
			if (GListAddValue(list, value) < 0) {
				DestroyGList(list);
				X_ERROR_NULL(X_EFUNC);
			}
		}
	}
	return list;
}

int GDictContains(const GDict *dict, const char *key)
{
	const GDictEntry *entry;
	for (entry = dict->hashes[hash(key) % dict->size]; entry != NULL; entry = entry->next)
		if (strcmp(key, entry->key) == 0)
			return 1;
	return 0;
}

size_t GDictSize(const GDict *dict)
{
	size_t size = 0;
	size_t i;
	for (i = 0; i < dict->size; ++i) {
		const GDictEntry *entry;
		for (entry = dict->hashes[i]; entry != NULL; entry = entry->next)
			++size;
	}
	return size;
}

GDictEntry *GDictLookup(const GDict *dict, const char *key)
{
	GDictEntry *entry;
	for (entry = dict->hashes[hash(key) % dict->size]; entry != NULL; entry = entry->next)
		if (strcmp(key, entry->key) == 0)
			return entry;
	return NULL;
}

GDictEntry *GDictPop(GDict *dict, const char *key)
{
	size_t hashidx = hash(key) % dict->size;
	GDictEntry *this = dict->hashes[hashidx];
	GDictEntry *prev = this;
	while (this) {
		if (strcmp(this->key, key) == 0) { /* found it! */
			if (prev == this) /* head is removed */
				dict->hashes[hashidx] = this->next;
			else
				prev->next = this->next;
			this->next = NULL;
			return this;
		}
		prev = this;
		this = this->next;
	}
	/* not found */
	X_ERROR_NULL(X_ENAME, "Key `%s' not found", key);
}

int GDictRemove(GDict *dict, const char *key)
{
	GDictEntry *entry;
	entry = GDictPop(dict, key);
	X_CHECK(entry, X_EFUNC); /* not found */
	GDictEntryFree(entry);
	return X_SUCCESS;
}

int GDictInsert(GDict *dict, const char *key, const void *data, size_t size, TYPECODE type)
{
	size_t hashidx = hash(key) % dict->size;
	GDictEntry *this = dict->hashes[hashidx];
	GDictEntry *prev = NULL;
	GDictEntry *entry;

	/* see if entry already exists */
	while (this) {
		if (strcmp(this->key, key) == 0) { /* found it! */
			entry = GDictEntryRealloc(this, size);
			if (entry == NULL)
				X_ERROR(X_EFUNC);
			if (entry != this) { /* relink */
				if (prev == NULL) /* head is moved */
					dict->hashes[hashidx] = entry;
				else
					prev->next = entry;
			}
			entry = GDictEntrySetValue(entry, data, size, type);

			if (entry == NULL)
				X_ERROR(X_EFUNC);

			return 0;
		}
		prev = this;
		this = this->next;
	}

	/* not found: create new entry */
	entry = GDictEntryAlloc(size);
	if (entry == NULL)
		X_ERROR(X_EFUNC);

	entry = GDictEntrySetKey(entry, key);
	if (entry == NULL) {
		MYFree(entry);
		X_ERROR(X_EFUNC);
	}

	entry = GDictEntrySetValue(entry, data, size, type);
	if (entry == NULL) {
		MYFree(entry);
		X_ERROR(X_EFUNC);
	}

	entry->next = dict->hashes[hashidx];
	dict->hashes[hashidx] = entry;
	return 0;
}

int GDictInsertValue(GDict *dict, const char *key, const GValue *value)
{
	TYPECODE type = GValueGetType(value);
	size_t size = GValueGetSize(value);
	const void * data = GValueGetDataPtr(value);
	return GDictInsert(dict, key, data, size, type);
}

int GDictInsertBLOBValue(GDict *dict, const char *key, const void *blob, size_t size)
{
	if (GDictInsert(dict, key, blob, size, UCHAR_TYPE_CODE) < 0)
		X_ERROR(X_EFUNC);
	return 0;
}

int GDictInsertStringValue(GDict *dict, const char *key, const char *string)
{
	size_t size = strlen(string) + 1;
	if (GDictInsert(dict, key, string, size, CHAR_TYPE_CODE) < 0)
		X_ERROR(X_EFUNC);
	return 0;
}

#define DEFINE_INSERT_FUNC(TYPE, TCODE) \
	int GDictInsert ## TYPE ## Value(GDict *dict, const char *key, TYPE value) \
	{ \
		if (GDictInsert(dict, key, &value, sizeof(value), TCODE) < 0) \
			X_ERROR(X_EFUNC); \
		return 0; \
	}

DEFINE_INSERT_FUNC(CHAR, CHAR_TYPE_CODE)
DEFINE_INSERT_FUNC(INT2, INT2_TYPE_CODE)
DEFINE_INSERT_FUNC(INT4, INT4_TYPE_CODE)
DEFINE_INSERT_FUNC(INT8, INT8_TYPE_CODE)
DEFINE_INSERT_FUNC(UCHAR, UCHAR_TYPE_CODE)
DEFINE_INSERT_FUNC(UINT2, UINT2_TYPE_CODE)
DEFINE_INSERT_FUNC(UINT4, UINT4_TYPE_CODE)
DEFINE_INSERT_FUNC(UINT8, UINT8_TYPE_CODE)
DEFINE_INSERT_FUNC(REAL4, REAL4_TYPE_CODE)
DEFINE_INSERT_FUNC(REAL8, REAL8_TYPE_CODE)
DEFINE_INSERT_FUNC(COMPLEX8, COMPLEX8_TYPE_CODE)
DEFINE_INSERT_FUNC(COMPLEX16, COMPLEX16_TYPE_CODE)

#undef DEFINE_INSERT_FUNC

void * GDictLookupBLOBValue(const GDict *dict, const char *key)
{
	GDictEntry *entry = GDictLookup(dict, key);
	const GValue *value;
	if (entry == NULL)
		X_ERROR_NULL(X_ENAME, "Key `%s' not found", key);
	value = GDictEntryGetValue(entry);
	if (value == NULL)
		X_ERROR_NULL(X_EFUNC);
	return GValueGetBLOB(value);
}

/* warning: shallow pointer */
const char * GDictLookupStringValue(const GDict *dict, const char *key)
{
	GDictEntry *entry = GDictLookup(dict, key);
	const GValue *value;
	if (entry == NULL)
		X_ERROR_NULL(X_ENAME, "Key `%s' not found", key);
	value = GDictEntryGetValue(entry);
	if (value == NULL)
		X_ERROR_NULL(X_EFUNC);
	return GValueGetString(value);
}

#define DEFINE_LOOKUP_FUNC(TYPE, FAILVAL) \
	TYPE GDictLookup ## TYPE ## Value(const GDict *dict, const char *key) \
	{ \
		GDictEntry *entry; \
		const GValue *value; \
		entry = GDictLookup(dict, key); \
		if (entry == NULL) \
			X_ERROR_VAL(FAILVAL, X_ENAME, "Key `%s' not found", key); \
		value = GDictEntryGetValue(entry); \
		if (value == NULL) \
			X_ERROR_VAL(FAILVAL, X_EFUNC); \
		return GValueGet ## TYPE (value); \
	}

DEFINE_LOOKUP_FUNC(CHAR, X_FAILURE)
DEFINE_LOOKUP_FUNC(INT2, X_FAILURE)
DEFINE_LOOKUP_FUNC(INT4, X_FAILURE)
DEFINE_LOOKUP_FUNC(INT8, X_FAILURE)
DEFINE_LOOKUP_FUNC(UCHAR, X_FAILURE)
DEFINE_LOOKUP_FUNC(UINT2, X_FAILURE)
DEFINE_LOOKUP_FUNC(UINT4, X_FAILURE)
DEFINE_LOOKUP_FUNC(UINT8, X_FAILURE)
DEFINE_LOOKUP_FUNC(REAL4, X_REAL4_FAIL_NAN)
DEFINE_LOOKUP_FUNC(REAL8, X_REAL8_FAIL_NAN)
DEFINE_LOOKUP_FUNC(COMPLEX8, X_REAL4_FAIL_NAN)
DEFINE_LOOKUP_FUNC(COMPLEX16, X_REAL8_FAIL_NAN)

REAL8 GDictLookupValueAsREAL8(const GDict *dict, const char *key)
{
	GDictEntry *entry;
	const GValue *value;
	entry = GDictLookup(dict, key);
	if (entry == NULL)
		X_ERROR_REAL8(X_ENAME, "Key `%s' not found", key);
	value = GDictEntryGetValue(entry);
	if (value == NULL)
		X_ERROR_REAL8(X_EFUNC);
	return GValueGetAsREAL8(value);
}

GValue * GDictPopValue(GDict *dict, const char *key)
{
	GDictEntry *entry;
	GValue *value;
	entry = GDictPop(dict, key);
	X_CHECK_NULL(entry, X_EFUNC);
	value = GValueDuplicate(GDictEntryGetValue(entry));
	GDictEntryFree(entry);
	return value;
}

void * GDictPopBLOBValue(GDict *dict, const char *key)
{
	GDictEntry *entry;
	void *value;
	entry = GDictPop(dict, key);
	X_CHECK_NULL(entry, X_EFUNC);
	value = GValueGetBLOB(GDictEntryGetValue(entry));
	GDictEntryFree(entry);
	return value;
}

char * GDictPopStringValue(GDict *dict, const char *key)
{
	GDictEntry *entry;
	char *value;
	entry = GDictPop(dict, key);
	X_CHECK_NULL(entry, X_EFUNC);
	value = XStringDuplicate(GValueGetString(GDictEntryGetValue(entry)));
	GDictEntryFree(entry);
	X_CHECK_NULL(value, X_EFUNC);
	return value;
}

#define DEFINE_POP_FUNC(TYPE, FAILVAL) \
	TYPE GDictPop ## TYPE ## Value(GDict *dict, const char *key) \
	{ \
		GDictEntry *entry; \
		TYPE value; \
		entry = GDictPop(dict, key); \
		X_CHECK_VAL(FAILVAL, entry, X_EFUNC); \
		value = GValueGet ## TYPE(GDictEntryGetValue(entry)); \
		GDictEntryFree(entry); \
		return value; \
	}

DEFINE_POP_FUNC(CHAR, X_FAILURE)
DEFINE_POP_FUNC(INT2, X_FAILURE)
DEFINE_POP_FUNC(INT4, X_FAILURE)
DEFINE_POP_FUNC(INT8, X_FAILURE)
DEFINE_POP_FUNC(UCHAR, X_FAILURE)
DEFINE_POP_FUNC(UINT2, X_FAILURE)
DEFINE_POP_FUNC(UINT4, X_FAILURE)
DEFINE_POP_FUNC(UINT8, X_FAILURE)
DEFINE_POP_FUNC(REAL4, X_REAL4_FAIL_NAN)
DEFINE_POP_FUNC(REAL8, X_REAL8_FAIL_NAN)
DEFINE_POP_FUNC(COMPLEX8, X_REAL4_FAIL_NAN)
DEFINE_POP_FUNC(COMPLEX16, X_REAL8_FAIL_NAN)

REAL8 GDictPopValueAsREAL8(GDict *dict, const char *key)
{
	GDictEntry *entry;
	REAL8 value;
	entry = GDictPop(dict, key);
	X_CHECK_REAL8(entry, X_EFUNC);
	value = GValueGetAsREAL8(GDictEntryGetValue(entry));
	GDictEntryFree(entry);
	return value;
}

struct LALDictAsStringAppendValueFuncParams {char *s; int first;};

static void GDictAsStringAppendValueFunc(char *key, GValue *value, void *thunk)
{
	struct LALDictAsStringAppendValueFuncParams *p = (struct LALDictAsStringAppendValueFuncParams *)(thunk);
	if (p->first)
		p->first = 0;
	else
		p->s = XStringAppend(p->s, ", ");
	p->s = XStringAppendFmt(p->s, "\"%s\": ", key);
	p->s = GValueAsStringAppend(p->s, value);
	return;
}

char * GDictAsStringAppend(char *s, const GDict *dict)
{
	GDict *d = (GDict *)(uintptr_t)dict; // discarding const qual is harmless
	struct LALDictAsStringAppendValueFuncParams p = {s, 1};
	p.s = XStringAppend(p.s, "{");
	GDictForeach(d, GDictAsStringAppendValueFunc, &p);
	p.s = XStringAppend(p.s, "}");
	return p.s;
}

void GDictPrint(const GDict *dict, int fd)
{
	char *s = NULL;
	s = GDictAsStringAppend(s, dict);
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
