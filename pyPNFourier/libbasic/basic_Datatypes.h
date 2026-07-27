/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#ifndef __INCLUDE_BASIC_DATATYPES__
#define __INCLUDE_BASIC_DATATYPES__

#include <inttypes.h>
#include <stdint.h>
#include "basic_Alloc.h"
// #include <quadmath.h>

/* macros for certain keywords */
#if __STDC_VERSION__ >= 199901L
# define _RESTRICT_ restrict
# define _INLINE_ inline
#elif defined __GNUC__
# define _RESTRICT_ __restrict__
# define _INLINE_ __inline__
#else
# define _RESTRICT_
# define _INLINE_
#endif

/* macros for compiler-specific attributes */
#if defined(__GNUC__)
# define _GCC_PRINTF_FORMAT_(NFMT, NARG) __attribute__ ((format (printf, NFMT, NARG)))
# define _GCC_VPRINTF_FORMAT_(NFMT) __attribute__ ((format (printf, NFMT, 0)))
#else
# define _GCC_PRINTF_FORMAT_(NFMT, NARG)
# define _GCC_VPRINTF_FORMAT_(NFMT)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// undef everything
#undef CHAR
#undef UCHAR
#undef INT
#undef INT2
#undef INT4
#undef INT8
#undef UINT
#undef UINT2
#undef UINT4
#undef UINT8
#undef REAL4
#undef REAL8
#undef REAL16
#undef REAL32
#undef COMPLEX8
#undef COMPLEX16
#undef COMPLEX32
#undef COMPLEX64

typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char BOOLEAN;	/**< Boolean logical type, see \ref Datatypes for more details */

/* Integer types */
typedef int16_t  INT2;		/**< Two-byte signed integer */
typedef int32_t  INT4;		/**< Four-byte signed integer. */
typedef uint16_t UINT2;		/**< Two-byte unsigned integer. */
typedef uint32_t UINT4;		/**< Four-byte unsigned integer. */
typedef int64_t  INT8;		/**< Eight-byte signed integer; on some platforms this is equivalent to <tt>long int</tt> instead. */
typedef uint64_t UINT8;		/**< Eight-byte unsigned integer; on some platforms this is equivalent to <tt>unsigned long int</tt> instead. */
typedef int INT;
typedef unsigned int UINT;
/** MACRO to initialize arbitrary variable 'x' to zero */
#define INIT_MEM(x) memset(&(x), 0, sizeof((x)))

/** C99 MACRO to declare and zero-initialize a variable".
    To declare and zero-initialize an array variable, use as "type INIT_DECL(var, [n]);" */
#define INIT_DECL(var, ...) var __VA_ARGS__; INIT_MEM(var)

/** MACRO which gives the number of elements in a fixed-size array */
#define ARR_NUM_ELEM(x) ( sizeof((x)) / sizeof((x)[0]) )
/** MACRO to access the last element in a fixed-size array */
#define ARR_LAST_ELEM(x) (x)[ NUM_ELEM(x) - 1 ]

#define MY_INT8_C(c) INT64_C(c)
#define MY_UINT8_C(c) UINT64_C(c)

/* Real types */
typedef float REAL4;            /**< Single precision real floating-point number (4 bytes). */
typedef double REAL8;           /**< Double precision real floating-point number (8 bytes). */
typedef long double REAL16;     /**< Double precision real floating-point number (12 bytes). */
#ifdef QUADMATH_H
typedef __float128 REAL32;      /**< Quadupole precision real floating-point number (16 bytes). */
#else
typedef long double REAL32;
#endif
/* Complex types */

/* Use C99 complex numbers where available: C99, gcc with non-ANSI extensions */
#if !defined(__cplusplus)
# if __STDC_VERSION__ >= 199901L || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#  define _C99_COMPLEX_NUMBERS_
# endif
#endif

#ifdef _C99_COMPLEX_NUMBERS_

#include <complex.h>

typedef float  complex COMPLEX8;	/**< Single-precision floating-point complex number (8 bytes total) */
typedef double complex COMPLEX16;	/**< Double-precision floating-point complex number (16 bytes total) */
typedef long double complex COMPLEX32; /**< LDouble-precision floating-point complex number (16 bytes total) */
#ifdef QUADMATH_H
typedef __complex128 COMPLEX64; /**< Quad-Double-precision floating-point complex number (16 bytes total) */
#else 
typedef long double complex COMPLEX64;
#endif

#define crectf(re, im) (((REAL4)(re)) + _Complex_I*((REAL4)(im)))	/**< Construct a COMPLEX8 from real and imaginary parts */
#define crect(re, im)  (((REAL8)(re)) + _Complex_I*((REAL8)(im)))	/**< Construct a COMPLEX16 from real and imaginary parts */
#define crectl(re, im)  (((REAL16)(re)) + _Complex_I*((REAL16)(im)))	/**< Construct a COMPLEX16 from real and imaginary parts */
#define crectq(re, im)  (((REAL32)(re)) + _Complex_I*((REAL32)(im)))	/**< Construct a COMPLEX16 from real and imaginary parts */

#define cpolarf(r, th) (((REAL4)(r)) * cexpf(crectf(0, th)))		/**< Construct a COMPLEX8 from polar modulus and argument */
#define cpolar(r, th)  (((REAL8)(r)) * cexp(crect(0, th)))		/**< Construct a COMPLEX16 from polar modulus and argument */
#define cpolarl(r, th)  (((REAL16)(r)) * cexpl(crectl(0, th)))		/**< Construct a COMPLEX16 from polar modulus and argument */
#define cpolarq(r, th)  (((REAL32)(r)) * cexpl(crectl(0, th)))		/**< Construct a COMPLEX16 from polar modulus and argument */


#else /* !_C99_COMPLEX_NUMBERS_ */
/** \cond DONT_DOXYGEN */

/****************************************************************************/
/* Fall back to GSL complex number types if C99 complex numbers are not     */
/* available.  GSL complex numbers are implemented as a struct containing   */
/* an array of 2 elements of the corresponding real type.  The C99 standard */
/* should guarantee that GSL complex numbers are binary-compatible with C99 */
/* complex numbers; 6.2.5 point 13 of the standard states that C99 complex  */
/* numbers are equivalent to an array of 2 elements of the corresponding    */
/* real type, and 6.7.2.1 point 13 states that padding is never added to    */
/* the beginning of a struct.                                               */
/****************************************************************************/

#include <math.h>
#include <gsl/gsl_complex.h>

typedef gsl_complex_float COMPLEX8;
typedef gsl_complex       COMPLEX16;
typedef gsl_complex_long_double  COMPLEX32
typedef gsl_complex_long_double  COMPLEX64

static _INLINE_ COMPLEX8 crectf(const REAL4 re, const REAL4 im);
static _INLINE_ COMPLEX8 crectf(const REAL4 re, const REAL4 im) {
  COMPLEX8 z; GSL_SET_COMPLEX(&z, re, im); return z;
}
static _INLINE_ COMPLEX16 crect(const REAL8 re, const REAL8 im);
static _INLINE_ COMPLEX16 crect(const REAL8 re, const REAL8 im) {
  COMPLEX16 z; GSL_SET_COMPLEX(&z, re, im); return z;
}

static _INLINE_ COMPLEX32 crectl(const REAL16 re, const REAL16 im);
static _INLINE_ COMPLEX32 crectl(const REAL16 re, const REAL16 im) {
  COMPLEX32 z; GSL_SET_COMPLEX(&z, re, im); return z;
}

static _INLINE_ COMPLEX8 cpolarf(const REAL4 r, const REAL4 th);
static _INLINE_ COMPLEX8 cpolarf(const REAL4 r, const REAL4 th) {
  COMPLEX8 z; GSL_SET_COMPLEX(&z, r*cos(th), r*sin(th)); return z;
}
static _INLINE_ COMPLEX16 cpolar(const REAL8 r, const REAL8 th);
static _INLINE_ COMPLEX16 cpolar(const REAL8 r, const REAL8 th) {
  COMPLEX16 z; GSL_SET_COMPLEX(&z, r*cos(th), r*sin(th)); return z;
}

#define crealf(z) GSL_REAL(z)
#define cimagf(z) GSL_IMAG(z)
#define creal(z)  GSL_REAL(z)
#define cimag(z)  GSL_IMAG(z)
#define creall(z)  GSL_REAL(z)
#define cimagl(z)  GSL_IMAG(z)

/** \endcond */
#endif /* _C99_COMPLEX_NUMBERS_ */

/**
 * \name Format macros for printing (U)INT2
 */
/** @{ */
#define INT2_PRId PRId16
#define INT2_PRIi PRIi16
#define INT2_PRIo PRIo16
#define INT2_PRIu PRIu16
#define INT2_PRIx PRIx16
#define INT2_PRIX PRIX16
/** @} */

/**
 * \name Format macros for printing (U)INT4
 */
/** @{ */
#define INT4_PRId PRId32
#define INT4_PRIi PRIi32
#define INT4_PRIo PRIo32
#define INT4_PRIu PRIu32
#define INT4_PRIx PRIx32
#define INT4_PRIX PRIX32
/** @} */

/**
 * \name Format macros for printing (U)INT8
 */
/** @{ */
#define INT8_PRId PRId64
#define INT8_PRIi PRIi64
#define INT8_PRIo PRIo64
#define INT8_PRIu PRIu64
#define INT8_PRIx PRIx64
#define INT8_PRIX PRIX64
/** @} */

/**
 * \name Format macros for scanning (U)INT2
 */
/** @{ */
#define INT2_SCNd SCNd16
#define INT2_SCNi SCNi16
#define INT2_SCNo SCNo16
#define INT2_SCNu SCNu16
#define INT2_SCNx SCNx16
/** @} */

/**
 * \name Format macros for scanning (U)INT4
 */
/** @{ */
#define INT4_SCNd SCNd32
#define INT4_SCNi SCNi32
#define INT4_SCNo SCNo32
#define INT4_SCNu SCNu32
#define INT4_SCNx SCNx32
/** @} */

/**
 * \name Format macros for scanning (U)INT8
 */
/** @{ */
#define INT8_SCNd SCNd64
#define INT8_SCNi SCNi64
#define INT8_SCNo SCNo64
#define INT8_SCNu SCNu64
#define INT8_SCNx SCNx64
/** @} */

/**
 * \name Format macros for printing and scanning (U)INT* (decimal only) and REAL*
 */
/** @{ */
#define INT2_FORMAT  INT2_SCNd
#define INT4_FORMAT  INT4_SCNd
#define INT8_FORMAT  INT8_SCNd
#define UINT2_FORMAT INT2_SCNu
#define UINT4_FORMAT INT4_SCNu
#define UINT8_FORMAT INT8_SCNu
#define REAL4_FORMAT "g"
#define REAL8_FORMAT "lg"
#define REAL16_FORMAT "Lg"


#if defined(__cplusplus)
}
#endif
#endif

