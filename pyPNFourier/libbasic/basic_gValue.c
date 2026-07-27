/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include <string.h>
#include "basic_gValue.h"
#include "basic_Error.h"

/* warning: garbage until set */
GValue * GValueAlloc(size_t size)
{
	GValue *v = XMalloc(sizeof(*v) + size);
	if (!v)
		X_ERROR_NULL(X_ENOMEM);
	v->size = size;
	return v;
}

/* warning: garbage until set */
GValue * GValueRealloc(GValue *value, size_t size)
{
	if (value == NULL)
		return GValueAlloc(size);
	value = XRealloc(value, sizeof(*value) + size);
	if (!value)
		X_ERROR_NULL(X_ENOMEM);
	value->size = size;
	return value;
}

GValue * GValueDuplicate(const GValue *value)
{
	size_t size = sizeof(GValue) + value->size;
	GValue *copy = XMalloc(size);
	if (!copy)
		X_ERROR_NULL(X_ENOMEM);
	return memcpy(copy, value, size);
}

GValue * GValueCopy(GValue *copy, const GValue *orig)
{
	TYPECODE type = GValueGetType(orig);
	size_t size = GValueGetSize(orig);
	const void * data = GValueGetDataPtr(orig);
	return GValueSet(copy, data, size, type);
}

GValue * GValueSet(GValue *value, const void *data, size_t size, TYPECODE type)
{
	X_CHECK_NULL(value != NULL, X_EFAULT);

	/* sanity check type-size relation */
	switch (type) {
	case CHAR_TYPE_CODE:
		/* variable length strings allowed */
		break;
	case INT2_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(INT2), X_ETYPE, "Wrong size for type");
		break;
	case INT4_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(INT4), X_ETYPE, "Wrong size for type");
		break;
	case INT8_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(INT8), X_ETYPE, "Wrong size for type");
		break;
	case UCHAR_TYPE_CODE:
		/* variable length BLOBs allowed */
		break;
	case UINT2_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(UINT2), X_ETYPE, "Wrong size for type");
		break;
	case UINT4_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(UINT4), X_ETYPE, "Wrong size for type");
		break;
	case UINT8_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(UINT8), X_ETYPE, "Wrong size for type");
		break;
	case REAL4_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(REAL4), X_ETYPE, "Wrong size for type");
		break;
	case REAL8_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(REAL8), X_ETYPE, "Wrong size for type");
		break;
	case COMPLEX8_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(COMPLEX8), X_ETYPE, "Wrong size for type");
		break;
	case COMPLEX16_TYPE_CODE:
		X_CHECK_NULL(size == sizeof(COMPLEX16), X_ETYPE, "Wrong size for type");
		break;
	default:
                X_ERROR_NULL(X_ETYPE, "Unsupported TYPECODE value 0%o", (unsigned int)type);
	}

	/* make sure sizes are compatible */
	X_CHECK_NULL(size == value->size, X_ESIZE, "Value has incompatible size");

	value->type = type;
	memcpy(value->data, data, size);
	return value;
}

void XDestroyValue(GValue *value)
{
	XFree(value);
	return;
}

GValue *XCreateValue(const void * data, size_t size, TYPECODE type)
{
	GValue *v = GValueAlloc(size);
	if (v == NULL)
		X_ERROR_NULL(X_EFUNC);
	v = GValueSet(v, data, size, type);
	if (v == NULL)
		X_ERROR_NULL(X_EFUNC);
	return v;
}

GValue *XCreateBLOBValue(const void * blob, size_t size)
{
	GValue *v = XCreateValue(blob, size, UCHAR_TYPE_CODE);
	if (v == NULL)
		X_ERROR_NULL(X_EFUNC);
	return v;
}

GValue *XCreateStringValue(const char *string)
{
	size_t size = strlen(string) + 1;
	GValue *v = XCreateValue(string, size, CHAR_TYPE_CODE);
	if (v == NULL)
		X_ERROR_NULL(X_EFUNC);
	return v;
}

#define DEFINE_CREATE_FUNC(TYPE, TCODE) \
	GValue *Create ## TYPE ## Value(TYPE value) \
	{ \
		GValue *v = XCreateValue(&value, sizeof(value), TCODE); \
		if (v == NULL) \
			X_ERROR_NULL(X_EFUNC); \
		return v; \
	}

DEFINE_CREATE_FUNC(CHAR, CHAR_TYPE_CODE)
DEFINE_CREATE_FUNC(INT2, INT2_TYPE_CODE)
DEFINE_CREATE_FUNC(INT4, INT4_TYPE_CODE)
DEFINE_CREATE_FUNC(INT8, INT8_TYPE_CODE)
DEFINE_CREATE_FUNC(UCHAR, UCHAR_TYPE_CODE)
DEFINE_CREATE_FUNC(UINT2, UINT2_TYPE_CODE)
DEFINE_CREATE_FUNC(UINT4, UINT4_TYPE_CODE)
DEFINE_CREATE_FUNC(UINT8, UINT8_TYPE_CODE)
DEFINE_CREATE_FUNC(REAL4, REAL4_TYPE_CODE)
DEFINE_CREATE_FUNC(REAL8, REAL8_TYPE_CODE)
DEFINE_CREATE_FUNC(COMPLEX8, COMPLEX8_TYPE_CODE)
DEFINE_CREATE_FUNC(COMPLEX16, COMPLEX16_TYPE_CODE)

#undef DEFINE_CREATE_FUNC

TYPECODE GValueGetType(const GValue *value)
{
	return value->type;
}

size_t GValueGetSize(const GValue *value)
{
	return value->size;
}

/* warning: shallow pointer */
const void * GValueGetDataPtr(const GValue *value)
{
	return value->data;
}

void * GValueGetData(void *data, size_t size, TYPECODE type, const GValue *value)
{
	if (value->size != size || value->type != type)
		X_ERROR_NULL(X_ETYPE, "Incorrect value type");
	return memcpy(data, value->data, size);
}

int GValueEqual(const GValue *value1, const GValue *value2)
{
	if (value1->size == value2->size && value1->type == value2->type)
		return memcmp(value1->data, value2->data, value1->size) == 0;
	return 0;
}

void * GValueGetBLOB(const GValue *value)
{
	void *blob;
	/* sanity check the type */
	if (value->type != UCHAR_TYPE_CODE)
		X_ERROR_NULL(X_ETYPE, "Value is not a BLOB");
	blob = MYMalloc(value->size);
	if (blob == NULL)
		X_ERROR_NULL(X_ENOMEM);
	return memcpy(blob, value->data, value->size);
}

/* warning: shallow pointer */
const char * GValueGetString(const GValue *value)
{
	/* sanity check the type */
	if (value->type != CHAR_TYPE_CODE)
		X_ERROR_NULL(X_ETYPE, "Value is not a string");
	/* make sure this is a nul-terminated string */
	if (value->size == 0 || ((const char *)(value->data))[value->size - 1] != '\0')
		X_ERROR_NULL(X_ETYPE, "Value is not a string");
	return (const char *)(value->data);
}

#define DEFINE_GET_FUNC(TYPE, TCODE, FAILVAL) \
	TYPE GValueGet ## TYPE (const GValue *value) \
	{ \
		X_CHECK_VAL(FAILVAL, value->type == TCODE, X_ETYPE); \
		return *(const TYPE *)(value->data); \
	}

DEFINE_GET_FUNC(CHAR, CHAR_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(INT2, INT2_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(INT4, INT4_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(INT8, INT8_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(UCHAR, UCHAR_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(UINT2, UINT2_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(UINT4, UINT4_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(UINT8, UINT8_TYPE_CODE, X_FAILURE)
DEFINE_GET_FUNC(REAL4, REAL4_TYPE_CODE, X_REAL4_FAIL_NAN)
DEFINE_GET_FUNC(REAL8, REAL8_TYPE_CODE, X_REAL8_FAIL_NAN)
DEFINE_GET_FUNC(COMPLEX8, COMPLEX8_TYPE_CODE, X_REAL4_FAIL_NAN)
DEFINE_GET_FUNC(COMPLEX16, COMPLEX16_TYPE_CODE, X_REAL8_FAIL_NAN)

#undef DEFINE_GET_FUNC

REAL8 GValueGetAsREAL8(const GValue *value)
{
	const INT8 max_as_double = INT8_C(9007199254740992);
	const UINT8 umax_as_double = INT8_C(9007199254740992);
	INT8 i;
	UINT8 u;
	REAL8 result;
	switch (value->type) {
	case CHAR_TYPE_CODE:
		if (value->size == 1)
			result = *(const CHAR *)value;
		else
			X_ERROR_REAL8(X_ETYPE, "Cannot convert string to float");
		break;
	case INT2_TYPE_CODE:
		result = GValueGetINT2(value);
		break;
	case INT4_TYPE_CODE:
		result = GValueGetINT4(value);
		break;
	case INT8_TYPE_CODE:
		result = i = GValueGetINT8(value);
		if (i > max_as_double || -i > max_as_double)
			X_PRINT_WARNING("Loss of precision");
		break;
	case UCHAR_TYPE_CODE:
		if (value->size == 1)
			result = GValueGetUCHAR(value);
		else
			X_ERROR_REAL8(X_ETYPE, "Cannot convert BLOB to float");
		break;
	case UINT2_TYPE_CODE:
		result = GValueGetUINT2(value);
		break;
	case UINT4_TYPE_CODE:
		result = GValueGetUINT4(value);
		break;
	case UINT8_TYPE_CODE:
		result = u = GValueGetUINT8(value);
		if (u > umax_as_double)
			X_PRINT_WARNING("Loss of precision");
		break;
	case REAL4_TYPE_CODE:
		result = GValueGetREAL4(value);
		break;
	case REAL8_TYPE_CODE:
		result = GValueGetREAL8(value);
		break;
	case COMPLEX8_TYPE_CODE:
	case COMPLEX16_TYPE_CODE:
		X_ERROR_REAL8(X_ETYPE, "Cannot convert complex to float");
	default:
                X_ERROR_REAL8(X_ETYPE, "Unsupported TYPECODE value 0%o", (unsigned int)value->type);
	}
	return result;
}

char * GValueAsStringAppend(char *s, const GValue *value)
{
	COMPLEX8 c;
	COMPLEX16 z;
	switch (value->type) {
	case CHAR_TYPE_CODE:
		if (value->size == 1)
			s = XStringAppendFmt(s, "'%c'", *(const CHAR *)(value->data));
		else
			s = XStringAppendFmt(s, "\"%s\"", (const CHAR *)(value->data));
		break;
	case INT2_TYPE_CODE:
		if (value->size != sizeof(INT2))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT2_PRId, GValueGetINT2(value));
		break;
	case INT4_TYPE_CODE:
		if (value->size != sizeof(INT4))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT4_PRId, GValueGetINT4(value));
		break;
	case INT8_TYPE_CODE:
		if (value->size != sizeof(INT8))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT8_PRId, GValueGetINT8(value));
		break;
	case UCHAR_TYPE_CODE:
		if (value->size == sizeof(UCHAR))
			s = XStringAppendFmt(s, "0x%x", GValueGetUCHAR(value));
		else {
			s = XStringAppendFmt(s, "b\"");
			for (size_t i = 0; i < value->size; ++i)
				s = XStringAppendFmt(s, "\\x%02x", ((const UCHAR *)(value->data))[i]);
			s = XStringAppendFmt(s, "\"");
		}
		break;
	case UINT2_TYPE_CODE:
		if (value->size != sizeof(UINT2))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT2_PRIu, GValueGetUINT2(value));
		break;
	case UINT4_TYPE_CODE:
		if (value->size != sizeof(UINT4))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT4_PRIu, GValueGetUINT4(value));
		break;
	case UINT8_TYPE_CODE:
		if (value->size != sizeof(UINT8))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%" INT8_PRIu, GValueGetUINT8(value));
		break;
	case REAL4_TYPE_CODE:
		if (value->size != sizeof(REAL4))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%.8g", GValueGetREAL4(value));
		break;
	case REAL8_TYPE_CODE:
		if (value->size != sizeof(REAL8))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%.16g", GValueGetREAL8(value));
		break;
	case COMPLEX8_TYPE_CODE:
		c = GValueGetCOMPLEX8(value);
		if (value->size != sizeof(COMPLEX8))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%.8g%+.8gj", crealf(c), cimagf(c));
		break;
	case COMPLEX16_TYPE_CODE:
		z = GValueGetCOMPLEX16(value);
		if (value->size != sizeof(COMPLEX16))
			X_ERROR_NULL(X_ESIZE, "Value has incorrect size for type");
		s = XStringAppendFmt(s, "%.16g%+.16gj", creal(z), cimag(z));
		break;
	default:
		X_ERROR_NULL(X_ETYPE, "Unsupported TYPECODE value 0%o", (unsigned int)value->type);
	}
	return s;
}

void GValuePrint(const GValue *value, int fd)
{
	char *s = NULL;
	s = GValueAsStringAppend(s, value);
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
