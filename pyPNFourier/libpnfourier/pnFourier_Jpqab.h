/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/

#ifndef __INCLUDE_PNFOURIER_JPQAb__
#define __INCLUDE_PNFOURIER_JPQAb__
#include "pnFourier_structs.h"
#include "pnFourier_utils.h"


REAL8 J_pqa1_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa1_series_optimized(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa1_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc ,REAL8 atol, REAL8 rtol);
REAL8 J_pqa1_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol);

REAL8 J_pqa2_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa2_series_optimized(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa2_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol);
REAL8 J_pqa2_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol);

REAL8 J_pqa3_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa3_series_optimized(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa3_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol);
REAL8 J_pqa3_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol);

REAL8 Jpa1_Approx(INT p, INT a, REAL8 e);
REAL8 Jpa2_Approx(INT p, INT a, REAL8 e);
REAL8 Jpa3_Approx(INT p, INT a, REAL8 e);
REAL8 dJpa1_Approx(INT p, INT a, REAL8 e);
REAL8 dJpa2_Approx(INT p, INT a, REAL8 e);

#endif
