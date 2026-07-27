/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/
#include "pnFourier.h"

#define USE_OPTIMIZED 1

static INT ExtendPNEllipticCache(size_t need_p, size_t need_q, size_t need_a, PNEllipticCache *cache)
{
    size_t new_abspMax = cache->abspMax ? cache->abspMax : 1;
    size_t new_absqMax = cache->absqMax ? cache->absqMax : 1;
    size_t new_aMax = cache->aMax ? cache->aMax : 1;
    while (new_abspMax < need_p) new_abspMax <<= 1;
    while (new_absqMax < need_q) new_absqMax <<= 1;
    while (new_aMax < need_a) new_aMax <<= 1;

    size_t newSize = (new_abspMax + 1) * (2*new_absqMax + 1) * (new_aMax + 1);
    REAL8 *newBuf = MYMalloc(newSize * sizeof(REAL8));
    if (!newBuf) {
        X_ERROR(X_ENOMEM);
    }
    for (size_t i=0; i<newSize; i++) newBuf[i] = X_REAL8_FAIL_NAN;
    for (size_t ia = 0; ia <= cache->aMax; ia++)
        for (size_t ip = 0; ip <= cache->abspMax; ip++) {
            size_t old_base = ia * cache->stride_a + ip * cache->stride_p;
            size_t new_base = ia * (new_abspMax + 1)*(2*new_absqMax + 1) + ip * (2*new_absqMax + 1);
            memcpy(&newBuf[new_base],
                &(cache->buf[old_base]),
                    (cache->stride_p)*sizeof(REAL8));
        }
    MYFree(cache->buf);
    cache->buf = newBuf;
    cache->aMax = new_aMax;
    cache->abspMax = new_abspMax;
    cache->absqMax = new_absqMax;
    cache->size = newSize;
    cache->stride_a = (cache->abspMax + 1) * (2*cache->absqMax + 1);
    cache->stride_p = 2*cache->absqMax + 1;
    return X_SUCCESS;
}

PNEllipticCache *CreatePNEllipticCache(size_t abspMax, size_t absqMax, size_t aMax, REAL8 e, REAL8 val0)
{
    PNEllipticCache *ret = MYMalloc(sizeof(PNEllipticCache));
    ret->aMax = 0;
    ret->abspMax = 0;
    ret->absqMax = 0;
    ret->stride_a = (ret->abspMax + 1) * (2*ret->absqMax + 1);
    ret->stride_p = 2*ret->absqMax + 1;
    ret->size = (ret->aMax + 1)*(ret->abspMax + 1)*(2*ret->absqMax + 1);
    ret->beta = eval_beta(e);
    ret->e = e;
    ret->buf = MYMalloc( sizeof(REAL8));
    ret->buf[0] = val0; // J[0, 0, 0] = 0

    if (ExtendPNEllipticCache(abspMax, absqMax, aMax, ret)!=X_SUCCESS) {
        X_ERROR_NULL(X_EFAILED);
    }
    return ret;
}

void DestroyPNEllipticCache(PNEllipticCache *cache)
{
    if (!cache)
        return;
    if (cache->buf) {
        MYFree(cache->buf);
        cache->buf = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^0_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
REAL8 get_Jpqa0_from_PNEllipticCache(INT p, INT q, INT a, PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) q = -q;
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa0_series_optimized(absp, q, a, cache->e, atol, rtol);
#else
        ret = J_pqa0_series(absp, q, a, cache->e, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}

REAL8 get_Jpqa0_from_cache(INT p, INT q, INT a, 
    BesselJCache2D *jc, LaplaceCache2D *lc, 
    PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) q = -q;
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa0_series_cache_optimized(absp, q, a, cache->e, jc, lc, atol, rtol);
#else
        ret = J_pqa0_series_cache(absp, q, a, cache->e, jc, lc, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^1_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
REAL8 get_Jpqa1_from_PNEllipticCache(INT p, INT q, INT a, PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    REAL8 pref = 1.0;
    if (p<0) {
        q = -q;
        pref = -1.0;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa1_series_optimized(absp, q, a, cache->e, atol, rtol);
#else
        ret = J_pqa1_series(absp, q, a, cache->e, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return pref*ret;
}

REAL8 get_Jpqa1_from_cache(INT p, INT q, INT a, 
    BesselJCache2D *jc, LaplaceCache2D *lc, 
    PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    REAL8 pref = 1.0;
    if (p<0) {
        q = -q;
        pref = -1.0;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa1_series_cache_optimized(absp, q, a, cache->e, jc, lc, atol, rtol);
#else
        ret = J_pqa1_series_cache(absp, q, a, cache->e, jc, lc, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return pref*ret;
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^2_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
REAL8 get_Jpqa2_from_PNEllipticCache(INT p, INT q, INT a, PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) {
        q = -q;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa2_series_optimized(absp, q, a, cache->e, atol, rtol);
#else
        ret = J_pqa2_series(absp, q, a, cache->e, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}

REAL8 get_Jpqa2_from_cache(INT p, INT q, INT a, 
    BesselJCache2D *jc, LaplaceCache2D *lc, 
    PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) {
        q = -q;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa2_series_cache_optimized(absp, q, a, cache->e, jc, lc, atol, rtol);
#else
        ret = J_pqa2_series_cache(absp, q, a, cache->e, jc, lc, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}


/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^3_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
REAL8 get_Jpqa3_from_PNEllipticCache(INT p, INT q, INT a, PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) {
        q = -q;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa3_series_optimized(absp, q, a, cache->e, atol, rtol);
#else
        ret = J_pqa3_series(absp, q, a, cache->e, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}

REAL8 get_Jpqa3_from_cache(INT p, INT q, INT a, 
    BesselJCache2D *jc, LaplaceCache2D *lc, 
    PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    REAL8 pref = 1.0;
    if (p<0) {
        q = -q;
        pref = -1.0;
    }
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = J_pqa3_series_cache_optimized(absp, q, a, cache->e, jc, lc, atol, rtol);
#else
        ret = J_pqa3_series_cache(absp, q, a, cache->e, jc, lc, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return pref*ret;
}


/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                              K_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
REAL8 get_Kpqa0_from_PNEllipticCache(INT p, INT q, INT a, PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) q = -q;
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
#if USE_OPTIMIZED
        ret = K_pqa0_series_optimized(absp, q, a, cache->e, atol, rtol);
#else
        ret = K_pqa0_series(absp, q, a, cache->e, atol, rtol);
#endif
        cache->buf[idx] = ret;
    }
    return ret;
}

REAL8 get_Kpqa0_from_cache(INT p, INT q, INT a, 
    BesselJCache2D *jc, LaplaceCache2D *lc, DLaplaceCache2D *dlc,
    PNEllipticCache *cache, REAL8 atol, REAL8 rtol)
{
    size_t absp = (p < 0 ? -p : p);
    size_t absq = (q < 0 ? -q : q);
    if (absp > cache->abspMax || absq > cache->absqMax || a > cache->aMax) {
        if (ExtendPNEllipticCache(absp, absq, a, cache) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    if (p<0) q = -q;
    size_t idx = (size_t)a * cache->stride_a + (size_t)absp * cache->stride_p + cache->absqMax + q;
    REAL8 ret = cache->buf[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
        ret = K_pqa0_series_cache(absp, q, a, cache->e, jc, lc, dlc, atol, rtol);
        cache->buf[idx] = ret;
    }
    return ret;
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                           Evaluator                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
PNEllipticEvaluator* CreatePNEllipticEvaluator(REAL8 e, REAL8 atol, REAL8 rtol)
{
    PNEllipticEvaluator* ret = (PNEllipticEvaluator*)MYMalloc(sizeof(PNEllipticEvaluator));
    ret->is_zeroe = (e <= 1e-16);
    ret->beta = eval_beta(e);
    ret->e = e;
    ret->atol = atol;
    ret->rtol = rtol;
    ret->cacheL = CreateLaplaceCache2D(2, 2, ret->beta);
    ret->cacheBJ = CreateBesselJCache2D(2, 2, ret->e);
    ret->cacheDL = CreateDLaplaceCache2D(2, 2, ret->beta);

    ret->cacheJ0 = CreatePNEllipticCache(2, 2, 2, ret->e, 1.0);
    ret->cacheJ1 = CreatePNEllipticCache(2, 2, 2, ret->e, 0.0);
    ret->cacheJ2 = CreatePNEllipticCache(2, 2, 2, ret->e, 0.0);
    ret->cacheJ3 = CreatePNEllipticCache(2, 2, 2, ret->e, 0.0);
    ret->cacheK0 = CreatePNEllipticCache(2, 2, 2, ret->e, log((1. + sqrt(1. - e*e)) / 2.));
    return ret;
}

void DestroyPNEllipticEvaluator(PNEllipticEvaluator *cache)
{
    if (!cache) return;
    STRUCTFREE(cache->cacheBJ, BesselJCache2D);
    STRUCTFREE(cache->cacheL,  LaplaceCache2D);
    STRUCTFREE(cache->cacheDL, DLaplaceCache2D);

    STRUCTFREE(cache->cacheJ0, PNEllipticCache);
    STRUCTFREE(cache->cacheJ1, PNEllipticCache);
    STRUCTFREE(cache->cacheJ2, PNEllipticCache);
    STRUCTFREE(cache->cacheJ3, PNEllipticCache);
    STRUCTFREE(cache->cacheK0, PNEllipticCache);

    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 evaluate_Jpqab(INT p, INT q, size_t a, size_t b, PNEllipticEvaluator* cache)
{
    if (cache->is_zeroe) {
        return (p==0 && b==0 ? 1.0 : 0.0);
    }
    switch (b)
    {
        case 0:
            return get_Jpqa0_from_cache(p, q, a, cache->cacheBJ, cache->cacheL, cache->cacheJ0, cache->atol, cache->rtol);
        case 1:
            return get_Jpqa1_from_cache(p, q, a, cache->cacheBJ, cache->cacheL, cache->cacheJ1, cache->atol, cache->rtol);
        case 2:
            return get_Jpqa2_from_cache(p, q, a, cache->cacheBJ, cache->cacheL, cache->cacheJ2, cache->atol, cache->rtol);
        case 3:
            return get_Jpqa3_from_cache(p, q, a, cache->cacheBJ, cache->cacheL, cache->cacheJ3, cache->atol, cache->rtol);
        default:
            X_PRINT_INFO("Unsupported J[%d,%d,%zu,%zu]", p, q, a, b);
            X_ERROR_REAL8(X_EINVAL);
    }
    return X_REAL8_FAIL_NAN;
}

REAL8 evaluate_Kpqa0(INT p, INT q, size_t a, PNEllipticEvaluator* cache)
{
    if (cache->is_zeroe) return 0.0;
    return get_Kpqa0_from_cache(p, q, a, cache->cacheBJ, cache->cacheL, cache->cacheDL, cache->cacheK0, cache->atol, cache->rtol);
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            Numeric                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
INT evaluate_JKIntegrals_Numeric(int p, REAL8 e, PNEllipticCacheV2 *ret,
        REAL8 atol, REAL8 rtol)
{
    if (!ret) X_ERROR(X_EINVAL);
    *ret = (PNEllipticCacheV2){0};
    PNEllipticEvaluator *PNJKICache = CreatePNEllipticEvaluator(e, atol, rtol);
    // 
    int absp = p<0 ? -p : p;
    ret->p = p;
    ret->e = e;
    REAL8 de = sqrt(1. - e*e);
    // Bessel Functions
    ret->BesselJ = jn(p, e*p);
    ret->dBesselJ = 0.5*( jn(p-1, e*p) - jn(p+1, e*p) );

    if (p!=0) {
        // dJpa0
        // ret->dJp10 = dJpa0_Approx(p, 1, e);
        ret->dJp10 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,0,PNJKICache) - evaluate_Jpqab(p+1,p,1,0,PNJKICache) ) + evaluate_Jpqab(p-1,p,2,0,PNJKICache)+evaluate_Jpqab(p+1,p,2,0,PNJKICache) );
        // dJpa1
        // ret->dJp11 = dJpa1_Approx(p, 1, e);
        ret->dJp11 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,1,PNJKICache) - evaluate_Jpqab(p+1,p,1,1,PNJKICache) ) + 
            evaluate_Jpqab(p-1,p,2,1,PNJKICache)+evaluate_Jpqab(p+1,p,2,1,PNJKICache) + 
            ( evaluate_Jpqab(p+1,p,2,0,PNJKICache) - evaluate_Jpqab(p-1,p,2,0,PNJKICache) )/de );
        
        // dJpa2
        // ret->dJp12 = dJpa2_Approx(p, 1, e);
        ret->dJp12 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,2,PNJKICache) - evaluate_Jpqab(p+1,p,1,2,PNJKICache) ) + 
            evaluate_Jpqab(p-1,p,2,2,PNJKICache)+evaluate_Jpqab(p+1,p,2,2,PNJKICache) + 
            2.*( evaluate_Jpqab(p+1,p,2,1,PNJKICache) - evaluate_Jpqab(p-1,p,2,1,PNJKICache) )/de );

        // Jpa0 positive a
        for (int a = 1; a<15; a++)
            ret->Jpa0Vec_aPos[a-1] = evaluate_Jpqab(p, p, a, 0, PNJKICache);
        
        // Kpa0
        for (int a = 1; a<7; a++)
            ret->Kpa0Vec[a-1] = evaluate_Kpqa0(p, p, a, PNJKICache);
        
        // Jpa1 positive a
        for (int a = 0; a<11; a++)
            ret->Jpa1Vec_aPos[a] = evaluate_Jpqab(p, p, a, 1, PNJKICache);

        // Jpa1 negative a
        REAL8 J0, Jm1, Jp1, Jm2, Jp2, Jm3, Jp3, Jm4, Jp4;
        REAL8 e2 = e*e;
        REAL8 e3 = e2*e;
        REAL8 e4 = e3*e;
        J0 = evaluate_Jpqab(p, p, 0, 1, PNJKICache);
        Jm1 = evaluate_Jpqab(p-1, p, 0, 1, PNJKICache);
        Jp1 = evaluate_Jpqab(p+1, p, 0, 1, PNJKICache);
        Jm2 = evaluate_Jpqab(p-2, p, 0, 1, PNJKICache);
        Jp2 = evaluate_Jpqab(p+2, p, 0, 1, PNJKICache);
        Jm3 = evaluate_Jpqab(p-3, p, 0, 1, PNJKICache);
        Jp3 = evaluate_Jpqab(p+3, p, 0, 1, PNJKICache);
        Jm4 = evaluate_Jpqab(p-4, p, 0, 1, PNJKICache);
        Jp4 = evaluate_Jpqab(p+4, p, 0, 1, PNJKICache);
        ret->Jpa1Vec_aNeg[0] = (J0 + 3*e2*J0 + (3*e4*J0)/8. - 2*e*Jm1 - (3*e3*Jm1)/2. + (3*e2*Jm2)/2. + (e4*Jm2)/4. - (e3*Jm3)/2. + (e4*Jm4)/16. - 2*e*Jp1 - (3*e3*Jp1)/2. + (3*e2*Jp2)/2. + (e4*Jp2)/4. - (e3*Jp3)/2. + (e4*Jp4)/16.);
        ret->Jpa1Vec_aNeg[1] = (J0 + (3*e2*J0)/2 - (3*e*Jm1)/2 - (3*e3*Jm1)/8 + (3*e2*Jm2)/4 - (e3*Jm3)/8 - (3*e*Jp1)/2 - (3*e3*Jp1)/8 + (3*e2*Jp2)/4 - (e3*Jp3)/8);
        ret->Jpa1Vec_aNeg[2] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
        ret->Jpa1Vec_aNeg[3] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;


        // Jpa2 positive a
        for (int a = 0; a<7; a++) {
            ret->Jpa2Vec_aPos[a] = evaluate_Jpqab(p, p, a, 2, PNJKICache);
            // ret->Jpa2Vec_aPos[a] = Jpa2_Approx(p, a, e);
        }
        // Jpa2 negative a
        // for (int a = -3; a<0; a++) {
        //     ret->Jpa2Vec_aNeg[a+3] = Jpa2_Approx(p, a, e);
        // }
        J0 = evaluate_Jpqab(p, p, 0, 2, PNJKICache);
        Jm1 = evaluate_Jpqab(p-1, p, 0, 2, PNJKICache);
        Jp1 = evaluate_Jpqab(p+1, p, 0, 2, PNJKICache);
        Jm2 = evaluate_Jpqab(p-2, p, 0, 2, PNJKICache);
        Jp2 = evaluate_Jpqab(p+2, p, 0, 2, PNJKICache);
        Jm3 = evaluate_Jpqab(p-3, p, 0, 2, PNJKICache);
        Jp3 = evaluate_Jpqab(p+3, p, 0, 2, PNJKICache);
        ret->Jpa2Vec_aNeg[0] = (J0 + (3*e2*J0)/2 - (3*e*Jm1)/2 - (3*e3*Jm1)/8 + (3*e2*Jm2)/4 - (e3*Jm3)/8 - (3*e*Jp1)/2 - (3*e3*Jp1)/8 + (3*e2*Jp2)/4 - (e3*Jp3)/8);
        ret->Jpa2Vec_aNeg[1] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
        ret->Jpa2Vec_aNeg[2] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;

        // Jpa3 positive a
        for (int a = 0; a<3; a++)
            ret->Jpa3Vec_aPos[a] = evaluate_Jpqab(p, p, a, 3, PNJKICache);
        // Jpa3 negative a
        J0 = evaluate_Jpqab(p, p, 0, 3, PNJKICache);
        Jm1 = evaluate_Jpqab(p-1, p, 0, 3, PNJKICache);
        Jp1 = evaluate_Jpqab(p+1, p, 0, 3, PNJKICache);
        Jm2 = evaluate_Jpqab(p-2, p, 0, 3, PNJKICache);
        Jp2 = evaluate_Jpqab(p+2, p, 0, 3, PNJKICache);
        ret->Jpa3Vec_aNeg[0] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
        ret->Jpa3Vec_aNeg[1] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;
    } else {
        for (int a = 0; a<11; a++)
            ret->dJSIntVec_b1[a+2] = 0.5*(evaluate_Jpqab(1, 0, a, 1, PNJKICache) - evaluate_Jpqab(-1, 0, a, 1, PNJKICache));
        REAL8 J0, J1, J2, J3;
        REAL8 e2 = e*e;
        J1 = evaluate_Jpqab(1, 0, 0, 1, PNJKICache);
        J2 = evaluate_Jpqab(2, 0, 0, 1, PNJKICache);
        J3 = evaluate_Jpqab(2, 0, 0, 1, PNJKICache);
        // a = -1
        ret->dJSIntVec_b1[1] = J1 - 0.5*e*J2;
        // a = -2
        ret->dJSIntVec_b1[0] = 0.25*( (e2+4.)*J1 + e*(-4.*J2 + e*J3) );

        for (int a = 0; a<7; a++)
            ret->dJCIntVec_b2[a+2] = evaluate_Jpqab(0, 0, a, 2, PNJKICache);
        J0 = evaluate_Jpqab(0, 0, 0, 2, PNJKICache);
        J1 = evaluate_Jpqab(1, 0, 0, 2, PNJKICache);
        J2 = evaluate_Jpqab(2, 0, 0, 2, PNJKICache);
        // a = -1
        ret->dJCIntVec_b2[1] = J0 - e*J1;
        // a = -2
        ret->dJCIntVec_b2[0] = 0.5*( (2.+e2)*J0 + e*(-4.*J1 + e*J2) );

        for (int a = 0; a<3; a++)
            ret->dJSIntVec_b3[a] = 0.5*(evaluate_Jpqab(1, 0, a, 3, PNJKICache) - evaluate_Jpqab(-1, 0, a, 3, PNJKICache));
    }
    STRUCTFREE(PNJKICache, PNEllipticEvaluator);
    return X_SUCCESS;
}


/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                         Approximation                            */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */
#define IS_DEBUG 0
INT evaluate_JKIntegrals_Approx(int p, REAL8 e, PNEllipticCacheV2 *ret)
{
    PNEllipticEvaluator *PNJKICache = CreatePNEllipticEvaluator(e, 1e-16, 1e-16);
    if (!ret || p==0 || p>200) X_ERROR_REAL8(X_EINVAL);
    // 
    int absp = p<0 ? -p : p;
    ret->p = p;
    ret->e = e;
    REAL8 de = sqrt(1. - e*e);
    // Bessel Functions
    ret->BesselJ = jn(p, e*p);
    ret->dBesselJ = 0.5*( jn(p-1, e*p) - jn(p+1, e*p) );
#if IS_DEBUG
    // dJpa0
    // ret->dJp10 = dJpa0_Approx(p, 1, e);
    ret->dJp10 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,0,PNJKICache) - evaluate_Jpqab(p+1,p,1,0,PNJKICache) ) + evaluate_Jpqab(p-1,p,2,0,PNJKICache)+evaluate_Jpqab(p+1,p,2,0,PNJKICache) );
    // dJpa1
    // ret->dJp11 = dJpa1_Approx(p, 1, e);
    ret->dJp11 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,1,PNJKICache) - evaluate_Jpqab(p+1,p,1,1,PNJKICache) ) + 
        evaluate_Jpqab(p-1,p,2,1,PNJKICache)+evaluate_Jpqab(p+1,p,2,1,PNJKICache) + 
        ( evaluate_Jpqab(p+1,p,2,0,PNJKICache) - evaluate_Jpqab(p-1,p,2,0,PNJKICache) )/de );

    // dJpa2
    // ret->dJp12 = dJpa2_Approx(p, 1, e);
    ret->dJp12 = 0.5*( p*( evaluate_Jpqab(p-1,p,1,2,PNJKICache) - evaluate_Jpqab(p+1,p,1,2,PNJKICache) ) + 
        evaluate_Jpqab(p-1,p,2,2,PNJKICache)+evaluate_Jpqab(p+1,p,2,2,PNJKICache) + 
        2.*( evaluate_Jpqab(p+1,p,2,1,PNJKICache) - evaluate_Jpqab(p-1,p,2,1,PNJKICache) )/de );

    // Jpa0 positive a
    for (int a = 1; a<15; a++) {
        // ret->Jpa0Vec_aPos[a-1] = evaluate_Jpqab(p, p, a, 0, PNJKICache);
        ret->Jpa0Vec_aPos[a-1] = Jpa0_Approx(p, a, e);
    }
    // Kpa0
    for (int a = 1; a<7; a++) {
        // ret->Kpa0Vec[a-1] = evaluate_Kpqa0(p, p, a, PNJKICache);
        ret->Kpa0Vec[a-1] = Kpa0_Approx(p, a, e);
    }


    // Jpa1 positive a
    for (int a = 0; a<11; a++) {
        // ret->Jpa1Vec_aPos[a] = evaluate_Jpqab(p, p, a, 1, PNJKICache);
        ret->Jpa1Vec_aPos[a] = Jpa1_Approx(p, a, e);
    }

    // Jpa1 negative a
    for (int a = -4; a<0; a++) {
        ret->Jpa1Vec_aNeg[a+4] = Jpa1_Approx(p, a, e);
    }
    REAL8 J0, Jm1, Jp1, Jm2, Jp2, Jm3, Jp3, Jm4, Jp4;
    REAL8 e2 = e*e;
    REAL8 e3 = e2*e;
    REAL8 e4 = e3*e;
    // J0 = evaluate_Jpqab(p, p, 0, 1, PNJKICache);
    // Jm1 = evaluate_Jpqab(p-1, p, 0, 1, PNJKICache);
    // Jp1 = evaluate_Jpqab(p+1, p, 0, 1, PNJKICache);
    // Jm2 = evaluate_Jpqab(p-2, p, 0, 1, PNJKICache);
    // Jp2 = evaluate_Jpqab(p+2, p, 0, 1, PNJKICache);
    // Jm3 = evaluate_Jpqab(p-3, p, 0, 1, PNJKICache);
    // Jp3 = evaluate_Jpqab(p+3, p, 0, 1, PNJKICache);
    // Jm4 = evaluate_Jpqab(p-4, p, 0, 1, PNJKICache);
    // Jp4 = evaluate_Jpqab(p+4, p, 0, 1, PNJKICache);
    // ret->Jpa1Vec_aNeg[0] = (J0 + 3*e2*J0 + (3*e4*J0)/8. - 2*e*Jm1 - (3*e3*Jm1)/2. + (3*e2*Jm2)/2. + (e4*Jm2)/4. - (e3*Jm3)/2. + (e4*Jm4)/16. - 2*e*Jp1 - (3*e3*Jp1)/2. + (3*e2*Jp2)/2. + (e4*Jp2)/4. - (e3*Jp3)/2. + (e4*Jp4)/16.);
    // ret->Jpa1Vec_aNeg[1] = (J0 + (3*e2*J0)/2 - (3*e*Jm1)/2 - (3*e3*Jm1)/8 + (3*e2*Jm2)/4 - (e3*Jm3)/8 - (3*e*Jp1)/2 - (3*e3*Jp1)/8 + (3*e2*Jp2)/4 - (e3*Jp3)/8);
    // ret->Jpa1Vec_aNeg[2] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
    // ret->Jpa1Vec_aNeg[3] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;


    // Jpa2 positive a
    for (int a = 0; a<7; a++) {
        // ret->Jpa2Vec_aPos[a] = evaluate_Jpqab(p, p, a, 2, PNJKICache);
        ret->Jpa2Vec_aPos[a] = Jpa2_Approx(p, a, e);
    }
    // Jpa2 negative a
    for (int a = -3; a<0; a++) {
        ret->Jpa2Vec_aNeg[a+3] = Jpa2_Approx(p, a, e);
    }
    // J0 = evaluate_Jpqab(p, p, 0, 2, PNJKICache);
    // Jm1 = evaluate_Jpqab(p-1, p, 0, 2, PNJKICache);
    // Jp1 = evaluate_Jpqab(p+1, p, 0, 2, PNJKICache);
    // Jm2 = evaluate_Jpqab(p-2, p, 0, 2, PNJKICache);
    // Jp2 = evaluate_Jpqab(p+2, p, 0, 2, PNJKICache);
    // Jm3 = evaluate_Jpqab(p-3, p, 0, 2, PNJKICache);
    // Jp3 = evaluate_Jpqab(p+3, p, 0, 2, PNJKICache);
    // ret->Jpa2Vec_aNeg[0] = (J0 + (3*e2*J0)/2 - (3*e*Jm1)/2 - (3*e3*Jm1)/8 + (3*e2*Jm2)/4 - (e3*Jm3)/8 - (3*e*Jp1)/2 - (3*e3*Jp1)/8 + (3*e2*Jp2)/4 - (e3*Jp3)/8);
    // ret->Jpa2Vec_aNeg[1] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
    // ret->Jpa2Vec_aNeg[2] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;

    // Jpa3 positive a
    for (int a = 0; a<3; a++) {
        ret->Jpa3Vec_aPos[a] = evaluate_Jpqab(p, p, a, 3, PNJKICache);
        // ret->Jpa3Vec_aPos[a] = Jpa3_Approx(p, a, e);
    }
    // Jpa3 negative a
    // for (int a = -2; a<0; a++) {
    //     ret->Jpa3Vec_aNeg[a+2] = Jpa3_Approx(p, a, e);
    // }
    J0 = evaluate_Jpqab(p, p, 0, 3, PNJKICache);
    Jm1 = evaluate_Jpqab(p-1, p, 0, 3, PNJKICache);
    Jp1 = evaluate_Jpqab(p+1, p, 0, 3, PNJKICache);
    Jm2 = evaluate_Jpqab(p-2, p, 0, 3, PNJKICache);
    Jp2 = evaluate_Jpqab(p+2, p, 0, 3, PNJKICache);
    ret->Jpa3Vec_aNeg[0] = (J0 + (e2*J0)/2. - e*Jm1 + (e2*Jm2)/4. - e*Jp1 + (e2*Jp2)/4.);
    ret->Jpa3Vec_aNeg[1] = J0 - (e*Jm1)/2. - (e*Jp1)/2.;

    STRUCTFREE(PNJKICache, PNEllipticEvaluator);
#else
    ret->dJp10 = dJpa0_Approx(p, 1, e);
    ret->dJp11 = dJpa1_Approx(p, 1, e);
    ret->dJp12 = dJpa2_Approx(p, 1, e);
    for (int a = 1; a<15; a++) {
        ret->Jpa0Vec_aPos[a-1] = Jpa0_Approx(p, a, e);
    }
    for (int a = 1; a<7; a++) {
        ret->Kpa0Vec[a-1] = Kpa0_Approx(p, a, e);
    }
    for (int a = 0; a<11; a++) {
        ret->Jpa1Vec_aPos[a] = Jpa1_Approx(p, a, e);
    }
    for (int a = -4; a<0; a++) {
        ret->Jpa1Vec_aNeg[a+4] = Jpa1_Approx(p, a, e);
    }
    for (int a = 0; a<7; a++) {
        ret->Jpa2Vec_aPos[a] = Jpa2_Approx(p, a, e);
    }
    for (int a = -3; a<0; a++) {
        ret->Jpa2Vec_aNeg[a+3] = Jpa2_Approx(p, a, e);
    }
    for (int a = 0; a<3; a++) {
        ret->Jpa3Vec_aPos[a] = Jpa3_Approx(p, a, e);
    }
    for (int a = -2; a<0; a++) {
        ret->Jpa3Vec_aNeg[a+2] = Jpa3_Approx(p, a, e);
    }
#endif
    return X_SUCCESS;
}


INT debug_test_PNEllipticApproxCache(int p, REAL8 e)
{
    PNEllipticCacheV2 cache;
    evaluate_JKIntegrals_Numeric(p, e, &cache, 1e-16, 1e-16);
    if (p!=0) {
        print_debug("p = %d, e = %16e:\n", p, e);
        print_err("\tJ[p] = %.16e\n", cache.BesselJ);
        print_err("\tdJ[p] = %.16e\n", cache.dBesselJ);
        print_debug("dJp10 = %.16e\n", cache.dJp10);
        print_debug("dJp11 = %.16e\n", cache.dJp11);
        print_debug("dJp12 = %.16e\n", cache.dJp12);
        print_debug("Jpa0:\n");
        for (int a = 1; a<15; a++) {
            print_err("\tJ[p,%d,0] = %.16e\n", a, cache.Jpa0Vec_aPos[a-1]);
        }
        print_debug("Jpa1:\n");
        for (int a = -4; a<0; a++) {
            print_err("\tJ[p,%d,1] = %.16e\n", a, cache.Jpa1Vec_aNeg[a+4]);
        }
        for (int a = 0; a<11; a++) {
            print_err("\tJ[p,%d,1] = %.16e\n", a, cache.Jpa1Vec_aPos[a]);
        }
        print_debug("Jpa2:\n");
        for (int a = -3; a<0; a++) {
            print_err("\tJ[p,%d,2] = %.16e\n", a, cache.Jpa2Vec_aNeg[a+3]);
        }
        for (int a = 0; a<7; a++) {
            print_err("\tJ[p,%d,2] = %.16e\n", a, cache.Jpa2Vec_aPos[a]);
        }
        print_debug("Jpa3:\n");
        for (int a = -2; a<0; a++) {
            print_err("\tJ[p,%d,3] = %.16e\n", a, cache.Jpa3Vec_aNeg[a+2]);
        }
        for (int a = 0; a<3; a++) {
            print_err("\tJ[p,%d,3] = %.16e\n", a, cache.Jpa3Vec_aPos[a]);
        }
        print_debug("Kpa0:\n");
        for (int a = 1; a<7; a++) {
            print_err("\tK[p,%d,0] = %.16e\n", a, cache.Kpa0Vec[a-1]);
        }
    } else {
        print_debug("p = %d, e = %16e:\n", p, e);
        for (int a = -2; a<11; a++)
            print_err("\tdJSb1[a=%d] = %.16e\n", a, cache.dJSIntVec_b1[a+2]);
        for (int a = -2; a<7; a++)
            print_err("\tdJCb2[a=%d] = %.16e\n", a, cache.dJCIntVec_b2[a+2]);
        for (int a = 0; a<3; a++)
            print_err("\tdJSb3[a=%d] = %.16e\n", a, cache.dJSIntVec_b3[a]);

    }
    return X_SUCCESS;
}
