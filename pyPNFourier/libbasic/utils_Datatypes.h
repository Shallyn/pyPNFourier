/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_DATATYPES__
#define __INCLUDE_UTILS_DATATYPES__

#include "basic_Datatypes.h"

/* ---------- constants ---------- */

/** Type size constants */
enum {
    E_1_BYTE_TYPE_SIZE = 000,  /**< One byte size 00 = 0 */
    E_2_BYTE_TYPE_SIZE = 001,  /**< Two byte size 01 = 1 */
    E_4_BYTE_TYPE_SIZE = 002,  /**< Four byte size 010 = 2 */
    E_8_BYTE_TYPE_SIZE = 003,  /**< Eight byte size 011 = 3 */
    E_16_BYTE_TYPE_SIZE = 004, /**< Sixteen byte size 0100 = 4 */
    E_32_BYTE_TYPE_SIZE = 005,  /**< Double Sixteen byte size 0101 = 5 */
    E_64_BYTE_TYPE_SIZE = 006,  /**< Double Sixteen byte size 0110 = 6 */
    E_TYPE_SIZE_MASK = 017     /**< Type size mask 1111 = 15 */
};

/** Type flag constants */
enum {
    E_FLTPT_TYPE_FLAG = 020,   /**< Floating-point (vs integer) type 10000 =  16 */
    E_CMPLX_TYPE_FLAG = 040,   /**< Complex (vs real) type  0100000 = 32 */
    E_UNSGN_TYPE_FLAG = 0100    /**< Unsigned (vs signed) type 01000000 = 64 */
};

typedef enum tagTYPECODE {
    CHAR_TYPE_CODE = E_1_BYTE_TYPE_SIZE,  /**< CHAR type code (0) */
    INT2_TYPE_CODE = E_2_BYTE_TYPE_SIZE,    /**< INT2 type code (1) */
    INT4_TYPE_CODE = E_4_BYTE_TYPE_SIZE,    /**< INT4 type code (2) */
    INT8_TYPE_CODE = E_8_BYTE_TYPE_SIZE,    /**< INT8 type code (3) */
    UCHAR_TYPE_CODE = E_1_BYTE_TYPE_SIZE | E_UNSGN_TYPE_FLAG,   /**< UCHAR type code (32) */
    UINT2_TYPE_CODE = E_2_BYTE_TYPE_SIZE | E_UNSGN_TYPE_FLAG,      /**< UINT2 type code (33) */
    UINT4_TYPE_CODE = E_4_BYTE_TYPE_SIZE | E_UNSGN_TYPE_FLAG,      /**< UINT4 type code (34) */
    UINT8_TYPE_CODE = E_8_BYTE_TYPE_SIZE | E_UNSGN_TYPE_FLAG,      /**< UINT8 type code (35) */
    REAL4_TYPE_CODE = E_4_BYTE_TYPE_SIZE | E_FLTPT_TYPE_FLAG,       /**< REAL4 type code (18) */
    REAL8_TYPE_CODE = E_8_BYTE_TYPE_SIZE | E_FLTPT_TYPE_FLAG,       /**< REAL8 type code (19) */
    REAL16_TYPE_CODE = E_16_BYTE_TYPE_SIZE | E_FLTPT_TYPE_FLAG,       /**< REAL16 type code (20) */
    REAL32_TYPE_CODE = E_64_BYTE_TYPE_SIZE | E_FLTPT_TYPE_FLAG,     /**< REAL32 type code (21) */
    COMPLEX8_TYPE_CODE = E_8_BYTE_TYPE_SIZE | E_CMPLX_TYPE_FLAG | E_FLTPT_TYPE_FLAG,         /**< COMPLEX8 type code (27) */
    COMPLEX16_TYPE_CODE = E_16_BYTE_TYPE_SIZE | E_CMPLX_TYPE_FLAG | E_FLTPT_TYPE_FLAG,        /**< COMPLEX16 type code (28) */
    COMPLEX32_TYPE_CODE = E_32_BYTE_TYPE_SIZE | E_CMPLX_TYPE_FLAG | E_FLTPT_TYPE_FLAG,        /**< COMPLEX32 type code (29) */
    COMPLEX64_TYPE_CODE = E_64_BYTE_TYPE_SIZE | E_CMPLX_TYPE_FLAG | E_FLTPT_TYPE_FLAG         /**< COMPLEX32 type code (30) */
} TYPECODE;

#ifdef  __cplusplus
extern "C" {
#endif

/* ---------- Vector types ---------- */
/** Vector of type CHAR, see \ref ss_Vector for more details. */
typedef struct tagCHARVector {
    UINT4 length; /**< Number of elements in array. */
    CHAR *data; /**< Pointer to the data array. */
} CHARVector;

/** Vector of type CHAR*, ie 'strings', see \ref ss_Vector for more details.  */
typedef struct tagStringVector {
    UINT4 length; /**< Number of elements in array. */
    CHAR **data; /**< Pointer to the data array. */
} StringVector;

/** Vector of type INT2, see \ref ss_Vector for more details. */
typedef struct tagINT2Vector {
    UINT4 length; /**< Number of elements in array. */
    INT2 *data; /**< Pointer to the data array. */
} INT2Vector;

/** Vector of type UINT2, see \ref ss_Vector for more details. */
typedef struct tagUINT2Vector {
    UINT4 length; /**< Number of elements in array. */
    UINT2 *data; /**< Pointer to the data array. */
} UINT2Vector;

/** Vector of type INT4, see \ref ss_Vector for more details. */
typedef struct tagINT4Vector {
    UINT4 length; /**< Number of elements in array. */
    INT4 *data; /**< Pointer to the data array. */
} INT4Vector;

/** Vector of type UINT4, see \ref ss_Vector for more details. */
typedef struct tagUINT4Vector {
    UINT4 length; /**< Number of elements in array. */
    UINT4 *data; /**< Pointer to the data array. */
} UINT4Vector;

/** Vector of type INT8, see \ref ss_Vector for more details. */
typedef struct tagINT8Vector {
    UINT4 length; /**< Number of elements in array. */
    INT8 *data; /**< Pointer to the data array. */
} INT8Vector;

/** Vector of type UINT8, see \ref ss_Vector for more details. */
typedef struct tagUINT8Vector {
    UINT4 length; /**< Number of elements in array. */
    UINT8 *data; /**< Pointer to the data array. */
} UINT8Vector;

/** Vector of type REAL4, see \ref ss_Vector for more details. */
typedef struct tagREAL4Vector {
    UINT4 length; /**< Number of elements in array. */
    REAL4 *data; /**< Pointer to the data array. */
} REAL4Vector;

/** Vector of type REAL8, see \ref ss_Vector for more details. */
typedef struct tagREAL8Vector {
    UINT4 length; /**< Number of elements in array. */
    REAL8 *data; /**< Pointer to the data array. */
} REAL8Vector;

/** Vector of type REAL16, see \ref ss_Vector for more details. */
typedef struct tagREAL16Vector {
    UINT4 length; /**< Number of elements in array. */
    REAL16 *data; /**< Pointer to the data array. */
} REAL16Vector;

/** Vector of type REAL32, see \ref ss_Vector for more details. */
typedef struct tagREAL32Vector {
    UINT4 length; /**< Number of elements in array. */
    REAL32 *data; /**< Pointer to the data array. */
} REAL32Vector;

/** Vector of type COMPLEX8, see \ref ss_Vector for more details. */
typedef struct tagCOMPLEX8Vector {
    UINT4 length; /**< Number of elements in array. */
    COMPLEX8 *data; /**< Pointer to the data array. */
} COMPLEX8Vector;

/** Vector of type COMPLEX16, see \ref ss_Vector for more details. */
typedef struct tagCOMPLEX16Vector {
    UINT4 length;    /**< Number of elements in array. */
    COMPLEX16 *data; /**< Pointer to the data array. */
} COMPLEX16Vector;

/** Vector of type COMPLEX32, see \ref ss_Vector for more details. */
typedef struct tagCOMPLEX32Vector {
    UINT4 length;    /**< Number of elements in array. */
    COMPLEX32 *data; /**< Pointer to the data array. */
} COMPLEX32Vector;

/** Vector of type COMPLEX64, see \ref ss_Vector for more details. */
typedef struct tagCOMPLEX64Vector {
    UINT4 length;    /**< Number of elements in array. */
    COMPLEX64 *data; /**< Pointer to the data array. */
} COMPLEX64Vector;


/* ---------- Array types ---------- */

/** Multidimentional array of INT2, see \ref ss_Array for more details. */
typedef struct tagINT2Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    INT2 *data;      /**< Pointer to the data array. */
} INT2Array;

/** Multidimentional array of UINT2, see \ref ss_Array for more details. */
typedef struct tagUINT2Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    UINT2 *data;     /**< Pointer to the data array. */
} UINT2Array;

/** Multidimentional array of INT4, see \ref ss_Array for more details. */
typedef struct tagINT4Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    INT4 *data;      /**< Pointer to the data array. */
} INT4Array;

/** Multidimentional array of UINT4, see \ref ss_Array for more details. */
typedef struct tagUINT4Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    UINT4 *data;     /**< Pointer to the data array. */
} UINT4Array;

/** Multidimentional array of INT8, see \ref ss_Array for more details. */
typedef struct tagINT8Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    INT8 *data;      /**< Pointer to the data array. */
} INT8Array;

/** Multidimentional array of UINT8, see \ref ss_Array for more details. */
typedef struct tagUINT8Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    UINT8 *data;     /**< Pointer to the data array. */
} UINT8Array;

/** Multidimentional array of REAL4, see \ref ss_Array for more details. */
typedef struct tagREAL4Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    REAL4 *data;     /**< Pointer to the data array. */
} REAL4Array;

/** Multidimentional array of REAL8, see \ref ss_Array for more details. */
typedef struct tagREAL8Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    REAL8 *data;     /**< Pointer to the data array. */
} REAL8Array;

/** Multidimentional array of REAL16, see \ref ss_Array for more details. */
typedef struct tagREAL16Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    REAL16 *data;     /**< Pointer to the data array. */
} REAL16Array;

/** Multidimentional array of REAL16, see \ref ss_Array for more details. */
typedef struct tagREAL32Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    REAL32 *data;     /**< Pointer to the data array. */
} REAL32Array;

/** Multidimentional array of COMPLEX8, see \ref ss_Array for more details. */
typedef struct tagCOMPLEX8Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    COMPLEX8 *data; /**< Pointer to the data array. */
} COMPLEX8Array;

/** Multidimentional array of COMPLEX16, see \ref ss_Array for more details. */
typedef struct tagCOMPLEX16Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    COMPLEX16 *data; /**< Pointer to the data array. */
} COMPLEX16Array;

/** Multidimentional array of COMPLEX32, see \ref ss_Array for more details. */
typedef struct tagCOMPLEX32Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    COMPLEX32 *data; /**< Pointer to the data array. */
} COMPLEX32Array;

/** Multidimentional array of COMPLEX64, see \ref ss_Array for more details. */
typedef struct tagCOMPLEX64Array {
    size_t size;
    UINT4Vector *dimLength; /**< Vector of array dimensions. */
    COMPLEX64 *data; /**< Pointer to the data array. */
} COMPLEX64Array;

    /* ---------- Sequence types ---------- */

typedef CHARVector CHARSequence;        /**< See \ref ss_Sequence for documentation */
typedef INT2Vector INT2Sequence;        /**< See \ref ss_Sequence for documentation */
typedef UINT2Vector UINT2Sequence;      /**< See \ref ss_Sequence for documentation */
typedef INT4Vector INT4Sequence;        /**< See \ref ss_Sequence for documentation */
typedef UINT4Vector UINT4Sequence;      /**< See \ref ss_Sequence for documentation */
typedef INT8Vector INT8Sequence;        /**< See \ref ss_Sequence for documentation */
typedef UINT8Vector UINT8Sequence;      /**< See \ref ss_Sequence for documentation */
typedef REAL4Vector REAL4Sequence;      /**< See \ref ss_Sequence for documentation */
typedef REAL8Vector REAL8Sequence;      /**< See \ref ss_Sequence for documentation */
typedef REAL16Vector REAL16Sequence;      /**< See \ref ss_Sequence for documentation */
typedef REAL32Vector REAL32Sequence;      /**< See \ref ss_Sequence for documentation */
typedef COMPLEX8Vector COMPLEX8Sequence; /**< See \ref ss_Sequence for documentation */
typedef COMPLEX16Vector COMPLEX16Sequence; /**< See \ref ss_Sequence for documentation */
typedef COMPLEX32Vector COMPLEX32Sequence; /**< See \ref ss_Sequence for documentation */
typedef COMPLEX64Vector COMPLEX64Sequence; /**< See \ref ss_Sequence for documentation */

    /* ---------- VectorSequence types ---------- */

/** Sequence of CHAR Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagCHARVectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    CHAR *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} CHARVectorSequence;

/** Sequence of INT2 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagINT2VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    INT2 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} INT2VectorSequence;

/** Sequence of UINT2 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagUINT2VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    UINT2 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} UINT2VectorSequence;

/** Sequence of INT4 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagINT4VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    INT4 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} INT4VectorSequence;

/** Sequence of UINT4 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagUINT4VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    UINT4 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} UINT4VectorSequence;

/** Sequence of INT8 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagINT8VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    INT8 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} INT8VectorSequence;

/** Sequence of UINT8 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagUINT8VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    UINT8 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} UINT8VectorSequence;

/** Sequence of REAL4 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagREAL4VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    REAL4 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} REAL4VectorSequence;

/** Sequence of REAL8 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagREAL8VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    REAL8 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} REAL8VectorSequence;

typedef struct tagREAL16VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    REAL16 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} REAL16VectorSequence;

typedef struct tagREAL32VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    REAL32 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} REAL32VectorSequence;

/** Sequence of COMPLEX8 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagCOMPLEX8VectorSequence {
    UINT4 length; /**< The number \a l of vectors. */
    UINT4 vectorLength; /**< The length \a n of each vector. */
    COMPLEX8 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} COMPLEX8VectorSequence;

/** Sequence of COMPLEX16 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagCOMPLEX16VectorSequence {
    UINT4 length;    /**< The number \a l of vectors. */
    UINT4 vectorLength;    /**< The length \a n of each vector. */
    COMPLEX16 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} COMPLEX16VectorSequence;

/** Sequence of COMPLEX32 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagCOMPLEX32VectorSequence {
    UINT4 length;    /**< The number \a l of vectors. */
    UINT4 vectorLength;    /**< The length \a n of each vector. */
    COMPLEX32 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} COMPLEX32VectorSequence;

/** Sequence of COMPLEX64 Vectors, see \ref ss_VectorSequence for more details. */
typedef struct tagCOMPLEX64VectorSequence {
    UINT4 length;    /**< The number \a l of vectors. */
    UINT4 vectorLength;    /**< The length \a n of each vector. */
    COMPLEX64 *data; /**< Pointer to the data array.  Element \a i of vector \a j is \c data[ \a jn + \a i \c ]. */
} COMPLEX64VectorSequence;

/* ---------- ArraySequence types ---------- */

/** Sequence of INT2 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagINT2ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    INT2 *data;      /**< Pointer to the data array. */
} INT2ArraySequence;

/** Sequence of UINT2 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagUINT2ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    UINT2 *data;     /**< Pointer to the data array. */
} UINT2ArraySequence;

/** Sequence of INT4 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagINT4ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    INT4 *data;      /**< Pointer to the data array. */
} INT4ArraySequence;

/** Sequence of UINT4 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagUINT4ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    UINT4 *data;     /**< Pointer to the data array. */
} UINT4ArraySequence;

/** Sequence of INT8 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagINT8ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    INT8 *data;      /**< Pointer to the data array. */
} INT8ArraySequence;

/** Sequence of UINT8 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagUINT8ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    UINT8 *data;     /**< Pointer to the data array. */
} UINT8ArraySequence;

/** Sequence of REAL4 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagREAL4ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    REAL4 *data;     /**< Pointer to the data array. */
} REAL4ArraySequence;

/** Sequence of REAL8 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagREAL8ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    REAL8 *data;     /**< Pointer to the data array. */
} REAL8ArraySequence;

/** Sequence of REAL16 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagREAL16ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    REAL16 *data;     /**< Pointer to the data array. */
} REAL16ArraySequence;

/** Sequence of REAL32 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagREAL32ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    REAL32 *data;     /**< Pointer to the data array. */
} REAL32ArraySequence;

/** Sequence of COMPLEX8 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagCOMPLEX8ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    COMPLEX8 *data; /**< Pointer to the data array. */
} COMPLEX8ArraySequence;

/** Sequence of COMPLEX16 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagCOMPLEX16ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    COMPLEX16 *data; /**< Pointer to the data array. */
} COMPLEX16ArraySequence;

/** Sequence of COMPLEX16 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagCOMPLEX32ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    COMPLEX32 *data; /**< Pointer to the data array. */
} COMPLEX32ArraySequence;

/** Sequence of COMPLEX64 multidimensional arrays, see \ref ss_ArraySequence for more details. */
typedef struct tagCOMPLEX64ArraySequence {
    UINT4 length;      /**< The number \a l of vectors. */
    UINT4 arrayDim;      /**< The number of data \a N in each array element (this is not the number \a m of indices). */
    UINT4Vector *dimLength; /**< Pointer to a vector of length \a m storing the array dimensions */
    COMPLEX64 *data; /**< Pointer to the data array. */
} COMPLEX64ArraySequence;

    /* ---------- TimeSeries types ---------- */

/** Length of name fields of LAL structured data types. */
enum enumTSNameLength { TSNameLength = 64 };

/** Time series of INT2 data, see \ref ss_TimeSeries for more details. */
typedef struct tagINT2TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;       /**< The time step between samples of the time series in seconds. */
    INT2Vector *data; /**< The sequence of sampled data. */
} INT2TimeSeries;

/** Time series of UINT2 data, see \ref ss_TimeSeries for more details. */
typedef struct tagUINT2TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */ REAL8 f0;        /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    UINT2Vector *data; /**< The sequence of sampled data. */
} UINT2TimeSeries;

/** Time series of INT4 data, see \ref ss_TimeSeries for more details. */
typedef struct tagINT4TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;       /**< The time step between samples of the time series in seconds. */
    INT4Vector *data; /**< The sequence of sampled data. */
} INT4TimeSeries;

/** Time series of UINT4 data, see \ref ss_TimeSeries for more details. */
typedef struct tagUINT4TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    UINT4Vector *data; /**< The sequence of sampled data. */
} UINT4TimeSeries;

/** Time series of INT8 data, see \ref ss_TimeSeries for more details. */
typedef struct tagINT8TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;       /**< The time step between samples of the time series in seconds. */
    INT8Vector *data; /**< The sequence of sampled data. */
} INT8TimeSeries;

/** Time series of UINT8 data, see \ref ss_TimeSeries for more details. */
typedef struct tagUINT8TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    UINT8Vector *data; /**< The sequence of sampled data. */
} UINT8TimeSeries;

/** Time series of REAL4 data, see \ref ss_TimeSeries for more details. */
typedef struct tagREAL4TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    REAL4Vector *data; /**< The sequence of sampled data. */
} REAL4TimeSeries;

/** Time series of REAL8 data, see \ref ss_TimeSeries for more details. */
typedef struct tagREAL8TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    REAL8Vector *data; /**< The sequence of sampled data. */
} REAL8TimeSeries;

/** Time series of REAL16 data, see \ref ss_TimeSeries for more details. */
typedef struct tagREAL16TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    REAL16Vector *data; /**< The sequence of sampled data. */
} REAL16TimeSeries;

typedef struct tagREAL32TimeSeries {
    REAL8 epoch; /**< The start time of the time series. */
    REAL8 deltaT;        /**< The time step between samples of the time series in seconds. */
    REAL32Vector *data; /**< The sequence of sampled data. */
} REAL32TimeSeries;

/** Time series of COMPLEX8 data, see \ref ss_TimeSeries for more details. */
typedef struct tagCOMPLEX8TimeSeries {
    REAL8 epoch;     /**< The start time of the time series. */
    REAL8 deltaT;           /**< The time step between samples of the time series in seconds. */
    COMPLEX8Vector *data; /**< The sequence of sampled data. */
} COMPLEX8TimeSeries;

/** Time series of COMPLEX16 data, see \ref ss_TimeSeries for more details. */
typedef struct tagCOMPLEX16TimeSeries {
    REAL8 epoch;      /**< The start time of the time series. */
    REAL8 deltaT;            /**< The time step between samples of the time series in seconds. */
    COMPLEX16Vector *data; /**< The sequence of sampled data. */
} COMPLEX16TimeSeries;

/** Time series of COMPLEX32 data, see \ref ss_TimeSeries for more details. */
typedef struct tagCOMPLEX32TimeSeries {
    REAL8 epoch;      /**< The start time of the time series. */
    REAL8 deltaT;            /**< The time step between samples of the time series in seconds. */
    COMPLEX32Vector *data; /**< The sequence of sampled data. */
} COMPLEX32TimeSeries;

/** Time series of COMPLEX64 data, see \ref ss_TimeSeries for more details. */
typedef struct tagCOMPLEX64TimeSeries {
    REAL8 epoch;      /**< The start time of the time series. */
    REAL8 deltaT;            /**< The time step between samples of the time series in seconds. */
    COMPLEX64Vector *data; /**< The sequence of sampled data. */
} COMPLEX64TimeSeries;

    /* ---------- TimeVectorSeries types ---------- */

/** Time series of INT2 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagINT2TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    INT2VectorSequence *data; /**< The sequence of sampled data vectors. */
} INT2TimeVectorSeries;

/** Time series of UINT2 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagUINT2TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    UINT2VectorSequence *data; /**< The sequence of sampled data vectors. */
} UINT2TimeVectorSeries;

/** Time series of INT4 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagINT4TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    INT4VectorSequence *data; /**< The sequence of sampled data vectors. */
} INT4TimeVectorSeries;

/** Time series of UINT4 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagUINT4TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    UINT4VectorSequence *data; /**< The sequence of sampled data vectors. */
} UINT4TimeVectorSeries;

/** Time series of INT8 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagINT8TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    INT8VectorSequence *data; /**< The sequence of sampled data vectors. */
} INT8TimeVectorSeries;

/** Time series of UINT8 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagUINT8TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    UINT8VectorSequence *data; /**< The sequence of sampled data vectors. */
} UINT8TimeVectorSeries;

/** Time series of REAL4 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagREAL4TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    REAL4VectorSequence *data; /**< The sequence of sampled data vectors. */
} REAL4TimeVectorSeries;

/** Time series of REAL8 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagREAL8TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    REAL8VectorSequence *data; /**< The sequence of sampled data vectors. */
} REAL8TimeVectorSeries;

/** Time series of REAL16 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagREAL16TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    REAL16VectorSequence *data; /**< The sequence of sampled data vectors. */
} REAL16TimeVectorSeries;

/** Time series of REAL32 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagREAL32TimeVectorSeries {
    CHAR name[TSNameLength];               /**< The name of the time series of vectors. */
    REAL8 epoch;        /**< The start time of the time series of vectors. */
    REAL8 deltaT;              /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;              /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    REAL32VectorSequence *data; /**< The sequence of sampled data vectors. */
} REAL32TimeVectorSeries;

/** Time series of COMPLEX8 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagCOMPLEX8TimeVectorSeries {
    CHAR name[TSNameLength];                   /**< The name of the time series of vectors. */
    REAL8 epoch;            /**< The start time of the time series of vectors. */
    REAL8 deltaT;                  /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;                  /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    COMPLEX8VectorSequence *data; /**< The sequence of sampled data vectors. */
} COMPLEX8TimeVectorSeries;

/** Time series of COMPLEX16 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagCOMPLEX16TimeVectorSeries {
    CHAR name[TSNameLength];                    /**< The name of the time series of vectors. */
    REAL8 epoch;             /**< The start time of the time series of vectors. */
    REAL8 deltaT;                   /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;                   /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    COMPLEX16VectorSequence *data; /**< The sequence of sampled data vectors. */
} COMPLEX16TimeVectorSeries;

/** Time series of COMPLEX32 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagCOMPLEX32TimeVectorSeries {
    CHAR name[TSNameLength];                    /**< The name of the time series of vectors. */
    REAL8 epoch;             /**< The start time of the time series of vectors. */
    REAL8 deltaT;                   /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;                   /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    COMPLEX32VectorSequence *data; /**< The sequence of sampled data vectors. */
} COMPLEX32TimeVectorSeries;

/** Time series of COMPLEX64 vectors, see \ref ss_TimeVectorSeries for more details. */
typedef struct tagCOMPLEX64TimeVectorSeries {
    CHAR name[TSNameLength];                    /**< The name of the time series of vectors. */
    REAL8 epoch;             /**< The start time of the time series of vectors. */
    REAL8 deltaT;                   /**< The time step between samples of the time series of vectors in seconds. */
    REAL8 f0;                   /**< The heterodyning frequency, in Hertz (zero if not heterodyned). */
    COMPLEX64VectorSequence *data; /**< The sequence of sampled data vectors. */
} COMPLEX64TimeVectorSeries;

/* ---------- TimeArraySeries ---------- */

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagINT2TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    INT2ArraySequence *data;
} INT2TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagUINT2TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    UINT2ArraySequence *data;
} UINT2TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagINT4TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    INT4ArraySequence *data;
} INT4TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagUINT4TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    UINT4ArraySequence *data;
} UINT4TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagINT8TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    INT8ArraySequence *data;
} INT8TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagUINT8TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    UINT8ArraySequence *data;
} UINT8TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagREAL4TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    REAL4ArraySequence *data;
} REAL4TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagREAL8TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    REAL8ArraySequence *data;
} REAL8TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagREAL16TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    REAL16ArraySequence *data;
} REAL16TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagREAL32TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    REAL32ArraySequence *data;
} REAL32TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagCOMPLEX8TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    COMPLEX8ArraySequence *data;
} COMPLEX8TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagCOMPLEX16TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    COMPLEX16ArraySequence *data;
} COMPLEX16TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagCOMPLEX32TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    COMPLEX32ArraySequence *data;
} COMPLEX32TimeArraySeries;

/** See \ref ss_TimeArraySeries for documentation */
typedef struct tagCOMPLEX64TimeArraySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 deltaT;
    REAL8 f0;
    COMPLEX64ArraySequence *data;
} COMPLEX64TimeArraySeries;

/* ---------- FrequencySeries types ---------- */

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagINT2FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    INT2Sequence *data;
} INT2FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagUINT2FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    UINT2Sequence *data;
} UINT2FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagINT4FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    INT4Sequence *data;
} INT4FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagUINT4FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    UINT4Sequence *data;
} UINT4FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagINT8FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    INT8Sequence *data;
} INT8FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagUINT8FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    UINT8Sequence *data;
} UINT8FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagREAL4FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    REAL4Sequence *data;
} REAL4FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagREAL8FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    REAL8Sequence *data;
} REAL8FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagCOMPLEX8FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    COMPLEX8Sequence *data;
} COMPLEX8FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagCOMPLEX16FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    COMPLEX16Sequence *data;
} COMPLEX16FrequencySeries;

/** See \ref ss_FrequencySeries for documentation */
typedef struct tagCOMPLEX32FrequencySeries {
    CHAR name[TSNameLength];
    REAL8 epoch;
    REAL8 f0;
    REAL8 deltaF;
    COMPLEX32Sequence *data;
} COMPLEX32FrequencySeries;

typedef struct tagProcStatus {
    INT4 statusCode;                            /**< A numerical code identifying the type of error, or 0 for nominal status; Negative values are reserved for certain standard error types */
    const CHAR *statusDescription;              /**< An explanatory string corresponding to the numerical status code */
    volatile const CHAR *Id;                    /**< A character string identifying the source file and version number of the function being reported on */
    const CHAR *function;                       /**< The name of the function */
    const CHAR *file;                           /**< The name of the source file containing the function code */
    INT4 line;                                  /**< The line number in the source file where the current \c statusCode was set */
    struct tagProcStatus *statusPtr;             /**< Pointer to the next node in the list; \c NULL if this function is not reporting a subroutine error */
    INT4 level;                                 /**< The nested-function level where any error was reported */
} ProcStatus;


#ifdef __cplusplus
}
#endif
#endif

