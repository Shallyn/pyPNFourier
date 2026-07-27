/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_UTILS_FACTORIES__
#define __INCLUDE_UTILS_FACTORIES__

#include "utils_Datatypes.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \name Error Codes */
/** @{ */
#define AVFACTORIESH_ELENGTH 1	/**< Illegal length. */
#define AVFACTORIESH_EVPTR   2	/**< Null vector/array handle. */
#define AVFACTORIESH_EUPTR   4	/**< Non-null vector/array pointer. */
#define AVFACTORIESH_EDPTR   8	/**< Null vector/array data. */
#define AVFACTORIESH_EMALLOC 16	/**< Malloc failure. */
/** @} */
/** @} */

/** \cond DONT_DOXYGEN */
#define AVFACTORIESH_MSGELENGTH  "Illegal length."
#define AVFACTORIESH_MSGEVPTR    "Null vector/array handle."
#define AVFACTORIESH_MSGEUPTR    "Non-null vector/array pointer."
#define AVFACTORIESH_MSGEDPTR    "Null vector/array data."
#define AVFACTORIESH_MSGEMALLOC  "Malloc failure."
/** \endcond */

/** \name INT2 array prototypes */
/** @{ */
INT2Array * CreateINT2ArrayL ( UINT4, ... );
INT2Array * CreateINT2ArrayV ( UINT4, UINT4 * );
INT2Array * CreateINT2Array ( UINT4Vector * );
INT2Array * ResizeINT2ArrayL ( INT2Array *, UINT4, ... );
INT2Array * ResizeINT2ArrayV ( INT2Array *, UINT4, UINT4 * );
INT2Array * ResizeINT2Array ( INT2Array *, UINT4Vector * );
void DestroyINT2Array ( INT2Array * );
void LALI2CreateArray ( ProcStatus *, INT2Array **, UINT4Vector * );
void LALI2ResizeArray ( ProcStatus *, INT2Array **, UINT4Vector * );
void LALI2DestroyArray ( ProcStatus *, INT2Array ** );
/** @} */

/** \name INT4 array prototypes */
/** @{ */
INT4Array * CreateINT4ArrayL ( UINT4, ... );
INT4Array * CreateINT4ArrayV ( UINT4, UINT4 * );
INT4Array * CreateINT4Array ( UINT4Vector * );
INT4Array * ResizeINT4ArrayL ( INT4Array *, UINT4, ... );
INT4Array * ResizeINT4ArrayV ( INT4Array *, UINT4, UINT4 * );
INT4Array * ResizeINT4Array ( INT4Array *, UINT4Vector * );
void DestroyINT4Array ( INT4Array * );
void LALI4CreateArray ( ProcStatus *, INT4Array **, UINT4Vector * );
void LALI4ResizeArray ( ProcStatus *, INT4Array **, UINT4Vector * );
void LALI4DestroyArray ( ProcStatus *, INT4Array ** );
/** @} */

/** \name INT8 array prototypes */
/** @{ */
INT8Array * CreateINT8ArrayL ( UINT4, ... );
INT8Array * CreateINT8ArrayV ( UINT4, UINT4 * );
INT8Array * CreateINT8Array ( UINT4Vector * );
INT8Array * ResizeINT8ArrayL ( INT8Array *, UINT4, ... );
INT8Array * ResizeINT8ArrayV ( INT8Array *, UINT4, UINT4 * );
INT8Array * ResizeINT8Array ( INT8Array *, UINT4Vector * );
void DestroyINT8Array ( INT8Array * );
void LALI8CreateArray ( ProcStatus *, INT8Array **, UINT4Vector * );
void LALI8ResizeArray ( ProcStatus *, INT8Array **, UINT4Vector * );
void LALI8DestroyArray ( ProcStatus *, INT8Array ** );
/** @} */

/** \name UINT2 array prototypes */
/** @{ */
UINT2Array * CreateUINT2ArrayL ( UINT4, ... );
UINT2Array * CreateUINT2ArrayV ( UINT4, UINT4 * );
UINT2Array * CreateUINT2Array ( UINT4Vector * );
UINT2Array * ResizeUINT2ArrayL ( UINT2Array *, UINT4, ... );
UINT2Array * ResizeUINT2ArrayV ( UINT2Array *, UINT4, UINT4 * );
UINT2Array * ResizeUINT2Array ( UINT2Array *, UINT4Vector * );
void DestroyUINT2Array ( UINT2Array * );
void LALU2CreateArray ( ProcStatus *, UINT2Array **, UINT4Vector * );
void LALU2ResizeArray ( ProcStatus *, UINT2Array **, UINT4Vector * );
void LALU2DestroyArray ( ProcStatus *, UINT2Array ** );
/** @} */

/** \name UINT4 array prototypes */
/** @{ */
UINT4Array * CreateUINT4ArrayL ( UINT4, ... );
UINT4Array * CreateUINT4ArrayV ( UINT4, UINT4 * );
UINT4Array * CreateUINT4Array ( UINT4Vector * );
UINT4Array * ResizeUINT4ArrayL ( UINT4Array *, UINT4, ... );
UINT4Array * ResizeUINT4ArrayV ( UINT4Array *, UINT4, UINT4 * );
UINT4Array * ResizeUINT4Array ( UINT4Array *, UINT4Vector * );
void DestroyUINT4Array ( UINT4Array * );
void LALU4CreateArray ( ProcStatus *, UINT4Array **, UINT4Vector * );
void LALU4ResizeArray ( ProcStatus *, UINT4Array **, UINT4Vector * );
void LALU4DestroyArray ( ProcStatus *, UINT4Array ** );
/** @} */

/** \name UINT8 array prototypes */
/** @{ */
UINT8Array * CreateUINT8ArrayL ( UINT4, ... );
UINT8Array * CreateUINT8ArrayV ( UINT4, UINT4 * );
UINT8Array * CreateUINT8Array ( UINT4Vector * );
UINT8Array * ResizeUINT8ArrayL ( UINT8Array *, UINT4, ... );
UINT8Array * ResizeUINT8ArrayV ( UINT8Array *, UINT4, UINT4 * );
UINT8Array * ResizeUINT8Array ( UINT8Array *, UINT4Vector * );
void DestroyUINT8Array ( UINT8Array * );
void LALU8CreateArray ( ProcStatus *, UINT8Array **, UINT4Vector * );
void LALU8ResizeArray ( ProcStatus *, UINT8Array **, UINT4Vector * );
void LALU8DestroyArray ( ProcStatus *, UINT8Array ** );
/** @} */


/** \name REAL4 array prototypes */
/** @{ */
REAL4Array * CreateREAL4ArrayL ( UINT4, ... );
REAL4Array * CreateREAL4ArrayV ( UINT4, UINT4 * );
REAL4Array * CreateREAL4Array ( UINT4Vector * );
REAL4Array * ResizeREAL4ArrayL ( REAL4Array *, UINT4, ... );
REAL4Array * ResizeREAL4ArrayV ( REAL4Array *, UINT4, UINT4 * );
REAL4Array * ResizeREAL4Array ( REAL4Array *, UINT4Vector * );
void DestroyREAL4Array ( REAL4Array * );
void LALSCreateArray ( ProcStatus *, REAL4Array **, UINT4Vector * );
void LALSResizeArray ( ProcStatus *, REAL4Array **, UINT4Vector * );
void LALSDestroyArray ( ProcStatus *, REAL4Array ** );
/** @} */

/** \name REAL4 array prototypes (default name) */
/** @{ */
REAL4Array * CreateArrayL ( UINT4, ... );
REAL4Array * CreateArrayV ( UINT4, UINT4 * );
REAL4Array * CreateArray ( UINT4Vector * );
REAL4Array * ResizeArrayL ( REAL4Array *, UINT4, ... );
REAL4Array * ResizeArrayV ( REAL4Array *, UINT4, UINT4 * );
REAL4Array * ResizeArray ( REAL4Array *, UINT4Vector * );
void LALCreateArray ( ProcStatus *, REAL4Array **, UINT4Vector * );
void LALResizeArray ( ProcStatus *, REAL4Array **, UINT4Vector * );
void LALDestroyArray ( ProcStatus *, REAL4Array ** );
/** @} */

/** \name REAL8 array prototypes */
/** @{ */
REAL8Array * CreateREAL8ArrayL ( UINT4, ... );
REAL8Array * CreateREAL8ArrayV ( UINT4, UINT4 * );
REAL8Array * CreateREAL8Array ( UINT4Vector * );
REAL8Array * ResizeREAL8ArrayL ( REAL8Array *, UINT4, ... );
REAL8Array * ResizeREAL8ArrayV ( REAL8Array *, UINT4, UINT4 * );
REAL8Array * ResizeREAL8Array ( REAL8Array *, UINT4Vector * );
void DestroyREAL8Array ( REAL8Array * );
void LALDCreateArray ( ProcStatus *, REAL8Array **, UINT4Vector * );
void LALDResizeArray ( ProcStatus *, REAL8Array **, UINT4Vector * );
void LALDDestroyArray ( ProcStatus *, REAL8Array ** );
/** @} */

/** @{ */
REAL16Array * CreateREAL16ArrayL ( UINT4, ... );
REAL16Array * CreateREAL16ArrayV ( UINT4, UINT4 * );
REAL16Array * CreateREAL16Array ( UINT4Vector * );
REAL16Array * ResizeREAL16ArrayL ( REAL16Array *, UINT4, ... );
REAL16Array * ResizeREAL16ArrayV ( REAL16Array *, UINT4, UINT4 * );
REAL16Array * ResizeREAL16Array ( REAL16Array *, UINT4Vector * );
void DestroyREAL16Array ( REAL16Array * );
void LALECreateArray ( ProcStatus *, REAL16Array **, UINT4Vector * );
void LALEResizeArray ( ProcStatus *, REAL16Array **, UINT4Vector * );
void LALEDestroyArray ( ProcStatus *, REAL16Array ** );
/** @} */

/** @{ */
REAL32Array * CreateREAL32ArrayL ( UINT4, ... );
REAL32Array * CreateREAL32ArrayV ( UINT4, UINT4 * );
REAL32Array * CreateREAL32Array ( UINT4Vector * );
REAL32Array * ResizeREAL32ArrayL ( REAL32Array *, UINT4, ... );
REAL32Array * ResizeREAL32ArrayV ( REAL32Array *, UINT4, UINT4 * );
REAL32Array * ResizeREAL32Array ( REAL32Array *, UINT4Vector * );
void DestroyREAL32Array ( REAL32Array * );
void LALFCreateArray ( ProcStatus *, REAL32Array **, UINT4Vector * );
void LALFResizeArray ( ProcStatus *, REAL32Array **, UINT4Vector * );
void LALFDestroyArray ( ProcStatus *, REAL32Array ** );
/** @} */


/** \name COMPLEX8 array prototypes */
/** @{ */
COMPLEX8Array * CreateCOMPLEX8ArrayL ( UINT4, ... );
COMPLEX8Array * CreateCOMPLEX8ArrayV ( UINT4, UINT4 * );
COMPLEX8Array * CreateCOMPLEX8Array ( UINT4Vector * );
COMPLEX8Array * ResizeCOMPLEX8ArrayL ( COMPLEX8Array *, UINT4, ... );
COMPLEX8Array * ResizeCOMPLEX8ArrayV ( COMPLEX8Array *, UINT4, UINT4 * );
COMPLEX8Array * ResizeCOMPLEX8Array ( COMPLEX8Array *, UINT4Vector * );
void DestroyCOMPLEX8Array ( COMPLEX8Array * );
void LALCCreateArray ( ProcStatus *, COMPLEX8Array **, UINT4Vector * );
void LALCResizeArray ( ProcStatus *, COMPLEX8Array **, UINT4Vector * );
void LALCDestroyArray ( ProcStatus *, COMPLEX8Array ** );
/** @} */

/** \name COMPLEX16 array prototypes */
/** @{ */
COMPLEX16Array * CreateCOMPLEX16ArrayL ( UINT4, ... );
COMPLEX16Array * CreateCOMPLEX16ArrayV ( UINT4, UINT4 * );
COMPLEX16Array * CreateCOMPLEX16Array ( UINT4Vector * );
COMPLEX16Array * ResizeCOMPLEX16ArrayL ( COMPLEX16Array *, UINT4, ... );
COMPLEX16Array * ResizeCOMPLEX16ArrayV ( COMPLEX16Array *, UINT4, UINT4 * );
COMPLEX16Array * ResizeCOMPLEX16Array ( COMPLEX16Array *, UINT4Vector * );
void DestroyCOMPLEX16Array ( COMPLEX16Array * );
void LALZCreateArray ( ProcStatus *, COMPLEX16Array **, UINT4Vector * );
void LALZResizeArray ( ProcStatus *, COMPLEX16Array **, UINT4Vector * );
void LALZDestroyArray ( ProcStatus *, COMPLEX16Array ** );
/** @} */

/** \name COMPLEX32 array prototypes */
/** @{ */
COMPLEX32Array * CreateCOMPLEX32ArrayL ( UINT4, ... );
COMPLEX32Array * CreateCOMPLEX32ArrayV ( UINT4, UINT4 * );
COMPLEX32Array * CreateCOMPLEX32Array ( UINT4Vector * );
COMPLEX32Array * ResizeCOMPLEX32ArrayL ( COMPLEX32Array *, UINT4, ... );
COMPLEX32Array * ResizeCOMPLEX32ArrayV ( COMPLEX32Array *, UINT4, UINT4 * );
COMPLEX32Array * ResizeCOMPLEX32Array ( COMPLEX32Array *, UINT4Vector * );
void DestroyCOMPLEX32Array ( COMPLEX32Array * );
void LALYCreateArray ( ProcStatus *, COMPLEX32Array **, UINT4Vector * );
void LALYResizeArray ( ProcStatus *, COMPLEX32Array **, UINT4Vector * );
void LALYDestroyArray ( ProcStatus *, COMPLEX32Array ** );
/** @} */

/** @{ */
COMPLEX64Array * CreateCOMPLEX64ArrayL ( UINT4, ... );
COMPLEX64Array * CreateCOMPLEX64ArrayV ( UINT4, UINT4 * );
COMPLEX64Array * CreateCOMPLEX64Array ( UINT4Vector * );
COMPLEX64Array * ResizeCOMPLEX64ArrayL ( COMPLEX64Array *, UINT4, ... );
COMPLEX64Array * ResizeCOMPLEX64ArrayV ( COMPLEX64Array *, UINT4, UINT4 * );
COMPLEX64Array * ResizeCOMPLEX64Array ( COMPLEX64Array *, UINT4Vector * );
void DestroyCOMPLEX64Array ( COMPLEX64Array * );
void LALWCreateArray ( ProcStatus *, COMPLEX64Array **, UINT4Vector * );
void LALWResizeArray ( ProcStatus *, COMPLEX64Array **, UINT4Vector * );
void LALWDestroyArray ( ProcStatus *, COMPLEX64Array ** );
/** @} */

/** @} */
/* ---------- end: ArrayFactories_c ---------- */


/**
 * \defgroup VectorFactories_c Module VectorFactories.c
 * \ingroup AVFactories_h
 *
 * \brief Create/destroy \<datatype\>%Vector objects.
 *
 * ### Description ###
 *
 * The \c CreateVector family of functions create a \<datatype\>%Vector of the appropriate dimensions.
 *
 * The \c ResizeVector family of functions changes the amount of storage allocated by the \c CreateVector functions.
 *
 * The \c DestroyVector family of functions return the storage allocated by the \c CreateVector functions to the system.
 *
 */
/** @{ */

/** \name CHAR vector prototypes */
/** @{ */
CHARVector * CreateCHARVector ( UINT4 length );
CHARVector * ResizeCHARVector ( CHARVector * vector, UINT4 length );
CHARVector * CutCHARVector( CHARVector *vector, size_t first, size_t length);
CHARVector * CopyCHARVector (CHARVector *vector);
void DestroyCHARVector ( CHARVector * vector );
void LALCHARCreateVector ( ProcStatus *, CHARVector **, UINT4 );
void LALCHARResizeVector ( ProcStatus *, CHARVector **, UINT4 );
void LALCHARDestroyVector ( ProcStatus *, CHARVector ** );
/** @} */

/** \name INT2 vector prototypes */
/** @{ */
INT2Vector * CreateINT2Vector ( UINT4 length );
INT2Vector * ResizeINT2Vector ( INT2Vector * vector, UINT4 length );
INT2Vector * CutINT2Vector( INT2Vector *vector, size_t first, size_t length);
INT2Vector * CopyINT2Vector (INT2Vector *vector);
void DestroyINT2Vector ( INT2Vector * vector );
void LALI2CreateVector ( ProcStatus *, INT2Vector **, UINT4 );
void LALI2ResizeVector ( ProcStatus *, INT2Vector **, UINT4 );
void LALI2DestroyVector ( ProcStatus *, INT2Vector ** );
/** @} */

/** \name INT4 vector prototypes */
/** @{ */
INT4Vector * CreateINT4Vector ( UINT4 length );
INT4Vector * ResizeINT4Vector ( INT4Vector * vector, UINT4 length );
INT4Vector * CutINT4Vector( INT4Vector *vector, size_t first, size_t length);
INT4Vector * CopyINT4Vector (INT4Vector *vector);
void DestroyINT4Vector ( INT4Vector * vector );
void LALI4CreateVector ( ProcStatus *, INT4Vector **, UINT4 );
void LALI4ResizeVector ( ProcStatus *, INT4Vector **, UINT4 );
void LALI4DestroyVector ( ProcStatus *, INT4Vector ** );
/** @} */


/** \name INT8 vector prototypes */
/** @{ */
INT8Vector * CreateINT8Vector ( UINT4 length );
INT8Vector * ResizeINT8Vector ( INT8Vector * vector, UINT4 length );
INT8Vector * CutINT8Vector( INT8Vector *vector, size_t first, size_t length);
INT8Vector * CopyINT8Vector (INT8Vector *vector);
void DestroyINT8Vector ( INT8Vector * vector );
void LALI8CreateVector ( ProcStatus *, INT8Vector **, UINT4 );
void LALI8ResizeVector ( ProcStatus *, INT8Vector **, UINT4 );
void LALI8DestroyVector ( ProcStatus *, INT8Vector ** );
/** @} */

/** \name UINT2 vector prototypes */
/** @{ */
UINT2Vector * CreateUINT2Vector ( UINT4 length );
UINT2Vector * ResizeUINT2Vector ( UINT2Vector * vector, UINT4 length );
UINT2Vector * CutUINT2Vector( UINT2Vector *vector, size_t first, size_t length);
UINT2Vector * CopyUINT2Vector (UINT2Vector *vector);
void DestroyUINT2Vector ( UINT2Vector * vector );
void LALU2CreateVector ( ProcStatus *, UINT2Vector **, UINT4 );
void LALU2ResizeVector ( ProcStatus *, UINT2Vector **, UINT4 );
void LALU2DestroyVector ( ProcStatus *, UINT2Vector ** );
/** @} */

/** \name UINT4 vector prototypes */
/** @{ */
UINT4Vector * CreateUINT4Vector ( UINT4 length );
UINT4Vector * ResizeUINT4Vector ( UINT4Vector * vector, UINT4 length );
UINT4Vector * CutUINT4Vector( UINT4Vector *vector, size_t first, size_t length);
UINT4Vector * CopyUINT4Vector (UINT4Vector *vector);
void DestroyUINT4Vector ( UINT4Vector * vector );
void LALU4CreateVector ( ProcStatus *, UINT4Vector **, UINT4 );
void LALU4ResizeVector ( ProcStatus *, UINT4Vector **, UINT4 );
void LALU4DestroyVector ( ProcStatus *, UINT4Vector ** );
/** @} */

/** \name UINT8 vector prototypes */
/** @{ */
UINT8Vector * CreateUINT8Vector ( UINT4 length );
UINT8Vector * ResizeUINT8Vector ( UINT8Vector * vector, UINT4 length );
UINT8Vector * CutUINT8Vector( UINT8Vector *vector, size_t first, size_t length);
UINT8Vector * CopyUINT8Vector (UINT8Vector *vector);
void DestroyUINT8Vector ( UINT8Vector * vector );
void LALU8CreateVector ( ProcStatus *, UINT8Vector **, UINT4 );
void LALU8ResizeVector ( ProcStatus *, UINT8Vector **, UINT4 );
void LALU8DestroyVector ( ProcStatus *, UINT8Vector ** );
/** @} */

/** \name REAL4 vector prototypes */
/** @{ */
REAL4Vector * CreateREAL4Vector ( UINT4 length );
REAL4Vector * ResizeREAL4Vector ( REAL4Vector * vector, UINT4 length );
REAL4Vector * CutREAL4Vector( REAL4Vector *vector, size_t first, size_t length);
REAL4Vector * CopyREAL4Vector (REAL4Vector *vector);
void DestroyREAL4Vector ( REAL4Vector * vector );
void LALSCreateVector ( ProcStatus *, REAL4Vector **, UINT4 );
void LALSResizeVector ( ProcStatus *, REAL4Vector **, UINT4 );
void LALSDestroyVector ( ProcStatus *, REAL4Vector ** );
/** @} */

/** \name REAL4 vector prototypes (default name) */
/** @{ */
REAL4Vector * CreateVector ( UINT4 length );
REAL4Vector * ResizeVector ( REAL4Vector * vector, UINT4 length );
void LALCreateVector ( ProcStatus *, REAL4Vector **, UINT4 );
void LALResizeVector ( ProcStatus *, REAL4Vector **, UINT4 );
void LALDestroyVector ( ProcStatus *, REAL4Vector ** );
/** @} */

/** \name REAL8 vector prototypes */
/** @{ */
REAL8Vector * CreateREAL8Vector ( UINT4 length );
REAL8Vector * ResizeREAL8Vector ( REAL8Vector * vector, UINT4 length );
REAL8Vector * CutREAL8Vector( REAL8Vector *vector, size_t first, size_t length);
REAL8Vector * CopyREAL8Vector (REAL8Vector *vector);
void DestroyREAL8Vector ( REAL8Vector * vector );
void LALDCreateVector ( ProcStatus *, REAL8Vector **, UINT4 );
void LALDResizeVector ( ProcStatus *, REAL8Vector **, UINT4 );
void LALDDestroyVector ( ProcStatus *, REAL8Vector ** );
/** @} */

/** \name REAL16 vector prototypes */
/** @{ */
REAL16Vector * CreateREAL16Vector ( UINT4 length );
REAL16Vector * ResizeREAL16Vector ( REAL16Vector * vector, UINT4 length );
REAL16Vector * CutREAL16Vector( REAL16Vector *vector, size_t first, size_t length);
REAL16Vector * CopyREAL16Vector (REAL16Vector *vector);
void DestroyREAL16Vector ( REAL16Vector * vector );
void LALECreateVector ( ProcStatus *, REAL16Vector **, UINT4 );
void LALEResizeVector ( ProcStatus *, REAL16Vector **, UINT4 );
void LALEDestroyVector ( ProcStatus *, REAL16Vector ** );
/** @} */

/** @{ */
REAL32Vector * CreateREAL32Vector ( UINT4 length );
REAL32Vector * ResizeREAL32Vector ( REAL32Vector * vector, UINT4 length );
REAL32Vector * CutREAL32Vector( REAL32Vector *vector, size_t first, size_t length);
REAL32Vector * CopyREAL32Vector (REAL32Vector *vector);
void DestroyREAL32Vector ( REAL32Vector * vector );
void LALFCreateVector ( ProcStatus *, REAL32Vector **, UINT4 );
void LALFResizeVector ( ProcStatus *, REAL32Vector **, UINT4 );
void LALFDestroyVector ( ProcStatus *, REAL32Vector ** );
/** @} */

/** \name COMPLEX8 vector prototypes */
/** @{ */
COMPLEX8Vector * CreateCOMPLEX8Vector ( UINT4 length );
COMPLEX8Vector * ResizeCOMPLEX8Vector ( COMPLEX8Vector * vector, UINT4 length );
COMPLEX8Vector * CutCOMPLEX8Vector( COMPLEX8Vector *vector, size_t first, size_t length);
COMPLEX8Vector * CopyCOMPLEX8Vector (COMPLEX8Vector *vector);
void DestroyCOMPLEX8Vector ( COMPLEX8Vector * vector );
void LALCCreateVector ( ProcStatus *, COMPLEX8Vector **, UINT4 );
void LALCResizeVector ( ProcStatus *, COMPLEX8Vector **, UINT4 );
void LALCDestroyVector ( ProcStatus *, COMPLEX8Vector ** );
/** @} */

/** \name COMPLEX16 vector prototypes */
/** @{ */
COMPLEX16Vector * CreateCOMPLEX16Vector ( UINT4 length );
COMPLEX16Vector * ResizeCOMPLEX16Vector ( COMPLEX16Vector * vector, UINT4 length );
COMPLEX16Vector * CutCOMPLEX16Vector( COMPLEX16Vector *vector, size_t first, size_t length);
COMPLEX16Vector * CopyCOMPLEX16Vector (COMPLEX16Vector *vector);
void DestroyCOMPLEX16Vector ( COMPLEX16Vector * vector );
void LALZCreateVector ( ProcStatus *, COMPLEX16Vector **, UINT4 );
void LALZResizeVector ( ProcStatus *, COMPLEX16Vector **, UINT4 );
void LALZDestroyVector ( ProcStatus *, COMPLEX16Vector ** );
/** @} */

/** \name COMPLEX32 vector prototypes */
/** @{ */
COMPLEX32Vector * CreateCOMPLEX32Vector ( UINT4 length );
COMPLEX32Vector * ResizeCOMPLEX32Vector ( COMPLEX32Vector * vector, UINT4 length );
COMPLEX32Vector * CutCOMPLEX32Vector( COMPLEX32Vector *vector, size_t first, size_t length);
COMPLEX32Vector * CopyCOMPLEX32Vector (COMPLEX32Vector *vector);
void DestroyCOMPLEX32Vector ( COMPLEX32Vector * vector );
void LALYCreateVector ( ProcStatus *, COMPLEX32Vector **, UINT4 );
void LALYResizeVector ( ProcStatus *, COMPLEX32Vector **, UINT4 );
void LALYDestroyVector ( ProcStatus *, COMPLEX32Vector ** );
/** @} */

/** @{ */
COMPLEX64Vector * CreateCOMPLEX64Vector ( UINT4 length );
COMPLEX64Vector * ResizeCOMPLEX64Vector ( COMPLEX64Vector * vector, UINT4 length );
COMPLEX64Vector * CutCOMPLEX64Vector( COMPLEX64Vector *vector, size_t first, size_t length);
COMPLEX64Vector * CopyCOMPLEX64Vector (COMPLEX64Vector *vector);
void DestroyCOMPLEX64Vector ( COMPLEX64Vector * vector );
void LALWCreateVector ( ProcStatus *, COMPLEX64Vector **, UINT4 );
void LALWResizeVector ( ProcStatus *, COMPLEX64Vector **, UINT4 );
void LALWDestroyVector ( ProcStatus *, COMPLEX64Vector ** );
/** @} */

/** @} */
/**\name Error Codes */
/** @{ */
#define SEQFACTORIESH_ESLENGTH  1	/**< Illegal sequence length. */
#define SEQFACTORIESH_EVLENGTH  2	/**< Illegal vector length. */
#define SEQFACTORIESH_EALENGTH  4	/**< Illegal array dimension. */
#define SEQFACTORIESH_EVPTR     8	/**< Null sequence handle. */
#define SEQFACTORIESH_EUPTR    16	/**< Non-null sequence pointer. */
#define SEQFACTORIESH_EDPTR    32	/**< Null sequence data. */
#define SEQFACTORIESH_EINPTR   64	/**< Null input pointer. */
#define SEQFACTORIESH_EMALLOC 128	/**< Malloc failure. */
/** @} */

/** \cond DONT_DOXYGEN */
#define SEQFACTORIESH_MSGESLENGTH "Illegal sequence length."
#define SEQFACTORIESH_MSGEVLENGTH "Illegal vector length."
#define SEQFACTORIESH_MSGEALENGTH "Illegal array dimension."
#define SEQFACTORIESH_MSGEVPTR    "Null sequence handle."
#define SEQFACTORIESH_MSGEUPTR    "Non-null sequence pointer."
#define SEQFACTORIESH_MSGEDPTR    "Null sequence data."
#define SEQFACTORIESH_MSGEINPTR   "Null input pointer."
#define SEQFACTORIESH_MSGEMALLOC  "Malloc failure."
/** \endcond */

typedef struct tagCreateVectorSequenceIn {
  UINT4 length; 	/**< The sequence length */
  UINT4 vectorLength;	/**< The length of each vector in the sequence */
} CreateVectorSequenceIn;

typedef struct tagCreateArraySequenceIn {
  UINT4 length;			/**< The sequence length */
  UINT4Vector *dimLength;	/**< The dimensions of each array index (the same for every array in the sequence) */
} CreateArraySequenceIn;

/** \name REAL4 prototypes (default name) */
/** @{ */
void LALCreateArraySequence(ProcStatus *, REAL4ArraySequence **,
                            CreateArraySequenceIn *);
void LALDestroyArraySequence(ProcStatus *, REAL4ArraySequence **);
/** @} */

/** \name INT2 prototypes */
/** @{ */
void LALI2CreateArraySequence ( ProcStatus *status,
          INT2ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALI2DestroyArraySequence ( ProcStatus *status,
          INT2ArraySequence **arraySeqence);
/** @} */


/** \name INT4 prototypes */
/** @{ */
void LALI4CreateArraySequence ( ProcStatus *status,
          INT4ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALI4DestroyArraySequence ( ProcStatus *status,
          INT4ArraySequence **arraySeqence);
/** @} */

/** \name INT8 prototypes */
/** @{ */
void LALI8CreateArraySequence ( ProcStatus *status,
          INT8ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALI8DestroyArraySequence ( ProcStatus *status,
          INT8ArraySequence **arraySeqence);
/** @} */

/** \name UINT2 prototypes */
/** @{ */
void LALU2CreateArraySequence ( ProcStatus *status,
          UINT2ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALU2DestroyArraySequence ( ProcStatus *status,
          UINT2ArraySequence **arraySeqence);
/** @} */

/** \name UINT4 prototypes */
/** @{ */
void LALU4CreateArraySequence ( ProcStatus *status,
          UINT4ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALU4DestroyArraySequence ( ProcStatus *status,
          UINT4ArraySequence **arraySeqence);
/** @} */

/** \name UINT8 prototypes */
/** @{ */
void LALU8CreateArraySequence ( ProcStatus *status,
          UINT8ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALU8DestroyArraySequence ( ProcStatus *status,
          UINT8ArraySequence **arraySeqence);
/** @} */

/** \name REAL4 prototypes */
/** @{ */
void LALSCreateArraySequence ( ProcStatus *status,
          REAL4ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALSDestroyArraySequence ( ProcStatus *status,
          REAL4ArraySequence **arraySeqence);
/** @} */

/** \name REAL8 prototypes */
/** @{ */
void LALDCreateArraySequence ( ProcStatus *status,
          REAL8ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALDDestroyArraySequence ( ProcStatus *status,
          REAL8ArraySequence **arraySeqence);
/** @} */

/** \name REAL16 prototypes */
/** @{ */
void LALECreateArraySequence ( ProcStatus *status,
          REAL16ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALEDestroyArraySequence ( ProcStatus *status,
          REAL16ArraySequence **arraySeqence);
/** @} */

/** @{ */
void LALFCreateArraySequence ( ProcStatus *status,
          REAL32ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALFDestroyArraySequence ( ProcStatus *status,
          REAL32ArraySequence **arraySeqence);
/** @} */

/** \name COMPLEX8 prototypes */
/** @{ */
void LALCCreateArraySequence ( ProcStatus *status,
          COMPLEX8ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALCDestroyArraySequence ( ProcStatus *status,
          COMPLEX8ArraySequence **arraySeqence);
/** @} */

/** \name COMPLEX16 prototypes */
/** @{ */
void LALZCreateArraySequence ( ProcStatus *status,
          COMPLEX16ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALZDestroyArraySequence ( ProcStatus *status,
          COMPLEX16ArraySequence **arraySeqence);
/** @} */

/** \name COMPLEX32 prototypes */
/** @{ */
void LALYCreateArraySequence ( ProcStatus *status,
          COMPLEX32ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALYDestroyArraySequence ( ProcStatus *status,
          COMPLEX32ArraySequence **arraySeqence);
/** @} */

/** \name COMPLEX64 prototypes */
/** @{ */
void LALWCreateArraySequence ( ProcStatus *status,
          COMPLEX64ArraySequence **arraySequence,
          CreateArraySequenceIn *aSeqParams);

void LALWDestroyArraySequence ( ProcStatus *status,
          COMPLEX64ArraySequence **arraySeqence);
/** @} */


/** @} */
/* ---------- end: ArraySequenceFactories_c ---------- */

/**
 * \defgroup VectorSequenceFactories_c Module VectorSequenceFactories.c
 * \ingroup SeqFactories_h
 * \brief Create/destroy \<datatype\>VectorSequence objects.
 *
 * ### Description ###
 *
 * The \c CreateVectorSequence family of functions create a \<datatype\>VectorSequence of the appropriate dimensions.
 *
 * The \c DestroyVectorSequence family of functions return the storage allocated by the \c CreateVectorSequence functions to the system.
 */
/** @{ */

/** \name REAL4 prototypes (default name) */
/** @{ */
REAL4VectorSequence * CreateVectorSequence ( UINT4 length, UINT4 veclen );
#ifndef SWIG   /* exclude from SWIG interface */
void DestroyVectorSequence ( REAL4VectorSequence * vecseq );
#endif   /* SWIG */


void LALCreateVectorSequence(ProcStatus *, REAL4VectorSequence **,
                             CreateVectorSequenceIn *);
void LALDestroyVectorSequence(ProcStatus *, REAL4VectorSequence**);
/** @} */

/** \name CHAR prototypes */
/** @{ */
CHARVectorSequence * CreateCHARVectorSequence ( UINT4 length, UINT4 veclen );
void DestroyCHARVectorSequence ( CHARVectorSequence * vecseq );

void LALCHARCreateVectorSequence ( ProcStatus *status,
          CHARVectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALCHARDestroyVectorSequence ( ProcStatus *status,
          CHARVectorSequence **vectorSequence);
/** @} */

/** \name INT2 prototypes */
/** @{ */
INT2VectorSequence * CreateINT2VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyINT2VectorSequence ( INT2VectorSequence * vecseq );

void LALI2CreateVectorSequence ( ProcStatus *status,
          INT2VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALI2DestroyVectorSequence ( ProcStatus *status,
          INT2VectorSequence **vectorSequence);
/** @} */

/** \name INT4 prototypes */
/** @{ */
INT4VectorSequence * CreateINT4VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyINT4VectorSequence ( INT4VectorSequence * vecseq );

void LALI4CreateVectorSequence ( ProcStatus *status,
          INT4VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALI4DestroyVectorSequence ( ProcStatus *status,
          INT4VectorSequence **vectorSequence);
/** @} */

/** \name INT8 prototypes */
/** @{ */
INT8VectorSequence * CreateINT8VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyINT8VectorSequence ( INT8VectorSequence * vecseq );

void LALI8CreateVectorSequence ( ProcStatus *status,
          INT8VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALI8DestroyVectorSequence ( ProcStatus *status,
          INT8VectorSequence **vectorSequence);
/** @} */

/** \name UINT2 prototypes */
/** @{ */
UINT2VectorSequence * CreateUINT2VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyUINT2VectorSequence ( UINT2VectorSequence * vecseq );

void LALU2CreateVectorSequence ( ProcStatus *status,
          UINT2VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALU2DestroyVectorSequence ( ProcStatus *status,
          UINT2VectorSequence **vectorSequence);
/** @} */

/** \name UINT4 prototypes */
/** @{ */
UINT4VectorSequence * CreateUINT4VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyUINT4VectorSequence ( UINT4VectorSequence * vecseq );

void LALU4CreateVectorSequence ( ProcStatus *status,
          UINT4VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALU4DestroyVectorSequence ( ProcStatus *status,
          UINT4VectorSequence **vectorSequence);
/** @} */

/** \name UINT8 prototypes */
/** @{ */
UINT8VectorSequence * CreateUINT8VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyUINT8VectorSequence ( UINT8VectorSequence * vecseq );

void LALU8CreateVectorSequence ( ProcStatus *status,
          UINT8VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALU8DestroyVectorSequence ( ProcStatus *status,
          UINT8VectorSequence **vectorSequence);
/** @} */

/** \name REAL4 prototypes */
/** @{ */
REAL4VectorSequence * CreateREAL4VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyREAL4VectorSequence ( REAL4VectorSequence * vecseq );

void LALSCreateVectorSequence ( ProcStatus *status,
          REAL4VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALSDestroyVectorSequence ( ProcStatus *status,
          REAL4VectorSequence **vectorSequence);
/** @} */

/** \name REAL8 prototypes */
/** @{ */
REAL8VectorSequence * CreateREAL8VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyREAL8VectorSequence ( REAL8VectorSequence * vecseq );

void LALDCreateVectorSequence ( ProcStatus *status,
          REAL8VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALDDestroyVectorSequence ( ProcStatus *status,
          REAL8VectorSequence **vectorSequence);
/** @} */

/** \name REAL16 prototypes */
/** @{ */
REAL16VectorSequence * CreateREAL16VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyREAL16VectorSequence ( REAL16VectorSequence * vecseq );

void LALECreateVectorSequence ( ProcStatus *status,
          REAL16VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALEDestroyVectorSequence ( ProcStatus *status,
          REAL16VectorSequence **vectorSequence);
/** @} */

/** \name REAL32 prototypes */
/** @{ */
REAL32VectorSequence * CreateREAL32VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyREAL32VectorSequence ( REAL32VectorSequence * vecseq );

void LALFCreateVectorSequence ( ProcStatus *status,
          REAL32VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALFDestroyVectorSequence ( ProcStatus *status,
          REAL32VectorSequence **vectorSequence);
/** @} */

/** \name COMPLEX8 prototypes */
/** @{ */
COMPLEX8VectorSequence * CreateCOMPLEX8VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyCOMPLEX8VectorSequence ( COMPLEX8VectorSequence * vecseq );

void LALCCreateVectorSequence ( ProcStatus *status,
          COMPLEX8VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALCDestroyVectorSequence ( ProcStatus *status,
          COMPLEX8VectorSequence **vectorSequence);
/** @} */

/** \name COMPLEX16 prototypes */
/** @{ */
COMPLEX16VectorSequence * CreateCOMPLEX16VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyCOMPLEX16VectorSequence ( COMPLEX16VectorSequence * vecseq );

void LALZCreateVectorSequence ( ProcStatus *status,
          COMPLEX16VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALZDestroyVectorSequence ( ProcStatus *status,
          COMPLEX16VectorSequence **vectorSequence);
/** @} */

/** \name COMPLEX32 prototypes */
/** @{ */
COMPLEX32VectorSequence * CreateCOMPLEX32VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyCOMPLEX32VectorSequence ( COMPLEX32VectorSequence * vecseq );

void LALYCreateVectorSequence ( ProcStatus *status,
          COMPLEX32VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALYDestroyVectorSequence ( ProcStatus *status,
          COMPLEX32VectorSequence **vectorSequence);
/** @} */

/** \name COMPLEX64 prototypes */
/** @{ */
COMPLEX64VectorSequence * CreateCOMPLEX64VectorSequence ( UINT4 length, UINT4 veclen );
void DestroyCOMPLEX64VectorSequence ( COMPLEX64VectorSequence * vecseq );

void LALWCreateVectorSequence ( ProcStatus *status,
          COMPLEX64VectorSequence **vectorSequence,
          CreateVectorSequenceIn *vSeqParams);

void LALWDestroyVectorSequence ( ProcStatus *status,
          COMPLEX64VectorSequence **vectorSequence);
/** @} */

/** @} */
/* ---------- end: VectorSequenceFactories_c ---------- */

COMPLEX8TimeSeries *CreateCOMPLEX8TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
COMPLEX16TimeSeries *CreateCOMPLEX16TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
REAL4TimeSeries *CreateREAL4TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
REAL8TimeSeries *CreateREAL8TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
INT2TimeSeries *CreateINT2TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
INT4TimeSeries *CreateINT4TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
INT8TimeSeries *CreateINT8TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
UINT2TimeSeries *CreateUINT2TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
UINT4TimeSeries *CreateUINT4TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );
UINT8TimeSeries *CreateUINT8TimeSeries ( const REAL8 epoch, REAL8 deltaT,  size_t length );

void DestroyCOMPLEX8TimeSeries ( COMPLEX8TimeSeries *series );
void DestroyCOMPLEX16TimeSeries ( COMPLEX16TimeSeries *series );
void DestroyREAL4TimeSeries ( REAL4TimeSeries *series );
void DestroyREAL8TimeSeries ( REAL8TimeSeries *series );
void DestroyINT2TimeSeries ( INT2TimeSeries *series );
void DestroyINT4TimeSeries ( INT4TimeSeries *series );
void DestroyINT8TimeSeries ( INT8TimeSeries *series );
void DestroyUINT2TimeSeries ( UINT2TimeSeries *series );
void DestroyUINT4TimeSeries ( UINT4TimeSeries *series );
void DestroyUINT8TimeSeries ( UINT8TimeSeries *series );

COMPLEX8TimeSeries *CutCOMPLEX8TimeSeries ( const COMPLEX8TimeSeries *series, size_t first, size_t length );
COMPLEX16TimeSeries *CutCOMPLEX16TimeSeries ( const COMPLEX16TimeSeries *series, size_t first, size_t length );
REAL4TimeSeries *CutREAL4TimeSeries ( const REAL4TimeSeries *series, size_t first, size_t length );
REAL8TimeSeries *CutREAL8TimeSeries ( const REAL8TimeSeries *series, size_t first, size_t length );
INT2TimeSeries *CutINT2TimeSeries ( const INT2TimeSeries *series, size_t first, size_t length );
INT4TimeSeries *CutINT4TimeSeries ( const INT4TimeSeries *series, size_t first, size_t length );
INT8TimeSeries *CutINT8TimeSeries ( const INT8TimeSeries *series, size_t first, size_t length );
UINT2TimeSeries *CutUINT2TimeSeries ( const UINT2TimeSeries *series, size_t first, size_t length );
UINT4TimeSeries *CutUINT4TimeSeries ( const UINT4TimeSeries *series, size_t first, size_t length );
UINT8TimeSeries *CutUINT8TimeSeries ( const UINT8TimeSeries *series, size_t first, size_t length );

// COMPLEX8TimeSeries *ResizeCOMPLEX8TimeSeries ( COMPLEX8TimeSeries *series, int first, size_t length );
// COMPLEX16TimeSeries *ResizeCOMPLEX16TimeSeries ( COMPLEX16TimeSeries *series, int first, size_t length );
// REAL4TimeSeries *ResizeREAL4TimeSeries ( REAL4TimeSeries *series, int first, size_t length );
// REAL8TimeSeries *ResizeREAL8TimeSeries ( REAL8TimeSeries *series, int first, size_t length );
// INT2TimeSeries *ResizeINT2TimeSeries ( INT2TimeSeries *series, int first, size_t length );
// INT4TimeSeries *ResizeINT4TimeSeries ( INT4TimeSeries *series, int first, size_t length );
// INT8TimeSeries *ResizeINT8TimeSeries ( INT8TimeSeries *series, int first, size_t length );
// UINT2TimeSeries *ResizeUINT2TimeSeries ( UINT2TimeSeries *series, int first, size_t length );
// UINT4TimeSeries *ResizeUINT4TimeSeries ( UINT4TimeSeries *series, int first, size_t length );
// UINT8TimeSeries *ResizeUINT8TimeSeries ( UINT8TimeSeries *series, int first, size_t length );

#ifdef __cplusplus
}
#endif
#endif

