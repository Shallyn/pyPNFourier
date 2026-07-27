/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/

#ifndef __INCLUDE_PNFOURIER_KPQAB__
#define __INCLUDE_PNFOURIER_KPQAB__
#include "pnFourier_structs.h"
#include <stddef.h>
#include "pnFourier_utils.h"


REAL8 K_pqa0_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 K_pqa0_series_optimized(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 K_pqa0_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, DLaplaceCache2D *dlc, REAL8 atol, REAL8 rtol);
REAL8 K_pqa0_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, DLaplaceCache2D *dlc,
    REAL8 atol, REAL8 rtol);
REAL8 Kpa0_Approx(INT p, INT a, REAL8 e);

#endif
