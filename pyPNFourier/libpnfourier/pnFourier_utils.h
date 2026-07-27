/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/

#ifndef __INCLUDE_PNFOURIER_UTILS__
#define __INCLUDE_PNFOURIER_UTILS__
#include "pnFourier_structs.h"

typedef struct {
    INT     a;
    REAL8   beta;
    INT     nMax;
    REAL8   *tbl;   /* tbl[m]  (0<=m<=mMax) */
} LaplaceCache;

// Laplace[a, n]
typedef struct {
    REAL8   beta;
    size_t  size;
    size_t  stride;
    size_t  aMax;
    size_t  nMax;   /* len = aMax * nMax */
    REAL8   *tbl;   /* L[a, n] = tbl[n + a*nMax] */
} LaplaceCache2D;

typedef struct {
    INT     a;
    REAL8   beta;
    INT     nMax;
    REAL8   *tbl;   /* tbl[m]  (0<=m<=mMax) */
} DLaplaceCache;

// Laplace[a, n]
typedef struct {
    REAL8   beta;
    size_t  size;
    size_t  stride;
    size_t  aMax;
    size_t  nMax;   /* len = aMax * nMax */
    REAL8   *tbl;   /* DL[a, n] = tbl[n + a*nMax] */
} DLaplaceCache2D;


typedef struct {
    REAL8     z;
    INT     kMax;
    REAL8   *Jbl;   /* Jbl[m]  (0<=k<=kMax) */
} BesselJCache;

// BesselJ[k, q*e]
typedef struct {
    REAL8   e;
    size_t  size;
    size_t  stride; // absqMax + 1
    size_t  abskMax;
    size_t  absqMax;   /* len = (kMax+1) * (absqMax+1) */
    REAL8   *tbl;   /* BesselJ[k, q*e] = tbl[k + q*kMax] */
} BesselJCache2D;

typedef struct {
  INT     jmax;
  REAL8   *Hbl;
} HarmNumberCache;

REAL8 eval_beta(REAL8 e);
REAL8 BesselJ(INT n, REAL8 x);
REAL8 laplace_na(INT n, INT a, REAL8 beta);
REAL8 loglaplace_na(INT n, INT a, REAL8 beta);
REAL8 Dlaplace_na(INT n, INT a, REAL8 beta);


LaplaceCache *CreateLaplaceCache(size_t nMax, INT a, REAL8 beta);
void DestroyLaplaceCache(LaplaceCache *cache);
REAL8 get_Laplace_from_LaplaceCache(INT m, LaplaceCache *C);

LaplaceCache2D *CreateLaplaceCache2D(size_t nMax, size_t aMax, REAL8 beta);
void DestroyLaplaceCache2D(LaplaceCache2D *cache);
REAL8 get_Laplace_from_LaplaceCache2D(INT n, INT a, LaplaceCache2D *C);



DLaplaceCache *CreateDLaplaceCache(size_t nMax, INT a, REAL8 beta);
void DestroyDLaplaceCache(DLaplaceCache *cache);
REAL8 get_DLaplace_from_DLaplaceCache(INT m, DLaplaceCache *C);

DLaplaceCache2D *CreateDLaplaceCache2D(size_t nMax, size_t aMax, REAL8 beta);
void DestroyDLaplaceCache2D(DLaplaceCache2D *cache);
REAL8 get_DLaplace_from_DLaplaceCache2D(INT n, INT a, DLaplaceCache2D *C);


BesselJCache *CreateBesselJCache(size_t kMax, REAL8 z);
void DestroyBesselJCache(BesselJCache *cache);
REAL8 get_BesselJ_from_BesselJCache(INT m, BesselJCache *C);

BesselJCache2D *CreateBesselJCache2D(size_t abskMax, size_t absqMax, REAL8 e);
void DestroyBesselJCache2D(BesselJCache2D *cache);
REAL8 get_BesselJ_from_BesselJCache2D(INT k, INT q, BesselJCache2D *C);

#endif

