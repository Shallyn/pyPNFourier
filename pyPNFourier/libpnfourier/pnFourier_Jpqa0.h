/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/

#ifndef __INCLUDE_PNFOURIER_JPQA0__
#define __INCLUDE_PNFOURIER_JPQA0__
#include "pnFourier_structs.h"
#include "pnFourier_utils.h"


INT Jpq10_fft(INT p_min, INT p_max, INT q, REAL8 e,
                   REAL8 tol, REAL8 *out);
INT Jpqa_fft(INT p_min, INT p_max, INT q, INT a, REAL8 e,
                 REAL8 tol, REAL8 *out);

REAL8 J_pq10_series(INT p, INT q, REAL8 e, REAL8 atol);
REAL8 J_pqa0_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa0_series_optimized(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol);
REAL8 J_pqa0_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *jc, LaplaceCache2D *lc,
    REAL8 atol, REAL8 rtol);
REAL8 J_pqa0_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *jc, LaplaceCache2D *lc,
    REAL8 atol, REAL8 rtol);



REAL8 Jpa0_Approx(INT p, INT a, REAL8 e);
REAL8 dJpa0_Approx(INT p, INT a, REAL8 e);


#endif
