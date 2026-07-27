/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/
#include "pnFourier_Kpqab.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "core_Kpqab_Approx.h"


#if defined(__STDC_NO_COMPLEX__) || defined(FFTW_NO_COMPLEX)
  #define COMPLEX(r, i)   { (r), (i) }
  #define GETREAL(z)       ((z)[0])
  #define GETIMAG(z)       ((z)[1])
  #define SETCOMP(z,vr,vi)     ((z)[0]=vr,(z)[1]=vi)
#else
  #define COMPLEX(r, i)   ((REAL8 _Complex)((r) + _Complex_I * (i)))
  #define GETREAL(z)       (creal(z))
  #define GETIMAG(z)       (cimag(z))
  #define SETCOMP(z,vr,vi) (z=(vr)+I*(vi))
#endif

static INT choose_N_a(INT p_max_abs, INT q, REAL8 e, REAL8 beta,
                    REAL8 tol, INT a)
{
    /* crude but safe – geometric tail + 12 a guard */
    REAL8 z  = fabs((REAL8)q * e);
    INT   k  = (INT)ceil(log(tol) / log(beta));
    REAL8 term = 1.0;
    while (term > tol) { ++k; term *= z / (2.0 * k); }
    INT N = k + 12 * a;
    if (!(N & 1)) ++N;
    if (N < p_max_abs + 12 * a) N = (p_max_abs + 12 * a) | 1;
    return N;
}

REAL8 K_pqa0_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    REAL8 beta = eval_beta(e);
    if (q==0 && a==0) return p==0 ? log((1.+sqrt(1.-e*e))/2.) : -pow(beta, abs(p)) / abs(p);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT N = choose_N_a(p_abs, q, e, beta, atol, a);

    INT kmin = -N, kmax = N;
    INT M = kmax - kmin + 1;

    /* Bessel array J_k(z) (both signs)                                */
    BesselJCache *jc = CreateBesselJCache(N, z);
    size_t nMax = abs(kmax) + abs(p); // just guess
    LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);
    DLaplaceCache *dlc = CreateDLaplaceCache(nMax, a, beta);
    REAL8 logprefLc = log(1 + beta2);

    /* series summation         */
    REAL8 sum_p = 0.0;
    for (INT k = 0; ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache(k, jc);
        INT n = abs(k + p);             /* |k+p| */
        REAL8 Lnp = get_Laplace_from_LaplaceCache(n, lc);
        REAL8 DLnp = get_DLaplace_from_DLaplaceCache(n, dlc);

        REAL8 term = (k & 1 ? -1.0 : 1.0) * bess * (DLnp + logprefLc*Lnp);
        sum_p += term;
        if (fabs(term) <= rtol * fabs(sum_p)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    REAL8 sum_m = 0.0;
    // negative summation
    for (INT k = 1; ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache(-k, jc);
        INT n = abs(-k + p);             /* |k+p| */
        REAL8 Lnp = get_Laplace_from_LaplaceCache(n, lc);
        REAL8 DLnp = get_DLaplace_from_DLaplaceCache(n, dlc);

        REAL8 term = (k & 1 ? -1.0 : 1.0) * bess * (DLnp + logprefLc*Lnp);
        sum_m += term;
        if (fabs(term) <= rtol * fabs(sum_m)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    STRUCTFREE(jc, BesselJCache);
    STRUCTFREE(lc, LaplaceCache);
    STRUCTFREE(dlc, DLaplaceCache);
    REAL8 factor = pow(1.0 + beta2, a);
    return -factor * (sum_p + sum_m);   /* already includes 1/(2π) via Laplace */
}

/* -------------------------------------------------------------------------
 * Optimized fast path for K^(0) (Eq. 41).
 *
 * On chi=x+i*s, with 0 <= s < acosh(1/e),
 *
 *   |log(1-e*cos(chi))| <= -log(1-e*cosh(s)).
 *
 * Combining this with the exponential and denominator bounds gives a
 * rigorous upper bound for the integral.  If it is already below the
 * requested mixed tolerance, return zero before choose_N_a() makes N grow
 * with |p| and before allocating the Bessel/Laplace/DLaplace tables.
 * Otherwise retain the legacy evaluator unchanged.
 * ------------------------------------------------------------------------- */

static REAL8 K_pqa0_contour_log_bound(REAL8 s, REAL8 p_abs, REAL8 q_abs,
                                       INT a, REAL8 e)
{
    REAL8 den = 1.0 - e * cosh(s);
    if (!(den > 0.0))
        return INFINITY;

    /* Taylor's bound: |log(1-z)| <= -log(1-|z|), |z|<1. */
    REAL8 log_factor_bound = -log(den);
    if (!(log_factor_bound > 0.0))
        return INFINITY;

    return -p_abs * s + q_abs * e * sinh(s)
         - a * log(den) + log(log_factor_bound);
}

static INT K_pqa0_below_absolute_tolerance(INT p, INT q, INT a, REAL8 e,
                                            REAL8 atol, REAL8 rtol)
{
    if (!(atol > 0.0) || !(rtol >= 0.0) || rtol >= 1.0)
        return 0;

    REAL8 p_abs = fabs((REAL8)p);
    REAL8 q_abs = fabs((REAL8)q);
    REAL8 s_max = acosh(1.0 / e);
    REAL8 lo = 0.0;
    REAL8 hi = s_max * (1.0 - 16.0 * DBL_EPSILON);
    if (!(hi > 0.0))
        return 0;

    /* Every sampled value is a valid bound; minimization only tightens it. */
    const REAL8 golden = 0.618033988749894848204586834365638118;
    REAL8 x1 = hi - golden * (hi - lo);
    REAL8 x2 = lo + golden * (hi - lo);
    REAL8 f1 = K_pqa0_contour_log_bound(x1, p_abs, q_abs, a, e);
    REAL8 f2 = K_pqa0_contour_log_bound(x2, p_abs, q_abs, a, e);
    for (INT i = 0; i < 96; ++i) {
        if (f1 > f2) {
            lo = x1;
            x1 = x2;
            f1 = f2;
            x2 = lo + golden * (hi - lo);
            f2 = K_pqa0_contour_log_bound(x2, p_abs, q_abs, a, e);
        } else {
            hi = x2;
            x2 = x1;
            f2 = f1;
            x1 = hi - golden * (hi - lo);
            f1 = K_pqa0_contour_log_bound(x1, p_abs, q_abs, a, e);
        }
    }

    REAL8 log_bound = fmin(f1, f2);
    log_bound = fmin(log_bound,
        K_pqa0_contour_log_bound(0.0, p_abs, q_abs, a, e));
    return log_bound <= log(atol) - log1p(-rtol);
}

REAL8 K_pqa0_series_optimized(INT p, INT q, INT a, REAL8 e,
                              REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX || a < 0 || atol < 0.0 || rtol < 0.0
        || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: invalid input\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    if (p == INT_MIN)
        X_ERROR_REAL8(X_EINVAL);
    if (p < 0) {
        if (q == INT_MIN)
            X_ERROR_REAL8(X_EINVAL);
        p = -p;
        q = -q;
    }

    if (K_pqa0_below_absolute_tolerance(p, q, a, e, atol, rtol))
        return 0.0;
    return K_pqa0_series(p, q, a, e, atol, rtol);
}

REAL8 K_pqa0_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, DLaplaceCache2D *dlc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    REAL8 beta = eval_beta(e);
    if (q==0 && a==0) return p==0 ? log((1.+sqrt(1.-e*e))/2.) : -pow(beta, abs(p)) / abs(p);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT N = choose_N_a(p_abs, q, e, beta, atol, a);

    INT kmin = -N, kmax = N;
    INT M = kmax - kmin + 1;

    /* Bessel array J_k(z) (both signs)                                */
    size_t nMax = abs(kmax) + abs(p); // just guess
    REAL8 logprefLc = log(1 + beta2);

    /* series summation         */
    REAL8 sum_p = 0.0;
    for (INT k = 0; ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache2D(k, q, bc);
        INT n = abs(k + p);             /* |k+p| */
        REAL8 Lnp = get_Laplace_from_LaplaceCache2D(n, a, lc);
        REAL8 DLnp = get_DLaplace_from_DLaplaceCache2D(n, a, dlc);

        REAL8 term = (k & 1 ? -1.0 : 1.0) * bess * (DLnp + logprefLc*Lnp);
        sum_p += term;
        if (fabs(term) <= rtol * fabs(sum_p)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    REAL8 sum_m = 0.0;
    // negative summation
    for (INT k = 1; ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache2D(-k, q, bc);
        INT n = abs(-k + p);             /* |k+p| */
        REAL8 Lnp = get_Laplace_from_LaplaceCache2D(n, a, lc);
        REAL8 DLnp = get_DLaplace_from_DLaplaceCache2D(n, a, dlc);

        REAL8 term = (k & 1 ? -1.0 : 1.0) * bess * (DLnp + logprefLc*Lnp);
        sum_m += term;
        if (fabs(term) <= rtol * fabs(sum_m)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    REAL8 factor = pow(1.0 + beta2, a);
    return -factor * (sum_p + sum_m);   /* already includes 1/(2π) via Laplace */
}

REAL8 K_pqa0_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, DLaplaceCache2D *dlc,
    REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX || a < 0 || !bc || !lc || !dlc
        || atol < 0.0 || rtol < 0.0 || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: invalid input\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    if (p == INT_MIN)
        X_ERROR_REAL8(X_EINVAL);
    if (p < 0) {
        if (q == INT_MIN)
            X_ERROR_REAL8(X_EINVAL);
        p = -p;
        q = -q;
    }

    if (K_pqa0_below_absolute_tolerance(p, q, a, e, atol, rtol))
        return 0.0;
    return K_pqa0_series_cache(p, q, a, e, bc, lc, dlc, atol, rtol);
}


/**
 * 
 *              
 * 
 *                  Analytic Approximation
 * 
 * 
 */
REAL8 Kpa0_Approx(int p, int a, REAL8 e)
{
    if(a < 1 || a>6)
    {
        XPrintError("Error - %s: a = %d must be in [1,6]\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }

    if(e < 0.0 || e >= 1.0)
    {
        XPrintError("Error - %s: e = %f must be in [0,1)\n", __func__, e);
        X_ERROR_REAL8(X_EINVAL);
    }

    int absp = abs(p);
    if (absp < 1 || absp > 200) {
        XPrintError("Error - %s: |p| = %d must > 5 and < 500\n", __func__, absp);
        X_ERROR_REAL8(X_EINVAL);
    }

    REAL8 e2 = e*e;
    REAL8 de2 = 1. - e2;
    REAL8 de = sqrt(de2);
    REAL8 lnde = log(de);
    REAL8 lnPref = absp*log(e) - (2.*a-1.)*lnde;
    size_t indp, inda;
    indp = absp - 1;
    inda = a-1;
    REAL8 eprLog0 = COEFFSK_LogR0[inda]*lnde;
    REAL8 t = 2.*de-1.;
    REAL8 epr_cheby;
    if (absp < 5) {
        REAL8 epr_jhc =
            COEFFSK_jhc_SP[inda][indp][0] + e2*(
                COEFFSK_jhc_SP[inda][indp][1] + e2*(
                    COEFFSK_jhc_SP[inda][indp][2] + e2*(
                        COEFFSK_jhc_SP[inda][indp][3] + e2*(
                            COEFFSK_jhc_SP[inda][indp][4] + e2*(
                                COEFFSK_jhc_SP[inda][indp][5] + e2*(
                                    COEFFSK_jhc_SP[inda][indp][6]
                                )
                            )
                        )
                    )
                )
            );
        epr_cheby = 
            COEFFSK_cheby_SP[inda][indp][0] + t*(
                COEFFSK_cheby_SP[inda][indp][1] + t*(
                    COEFFSK_cheby_SP[inda][indp][2] + t*(
                        COEFFSK_cheby_SP[inda][indp][3] + t*(
                            COEFFSK_cheby_SP[inda][indp][4] + t*(
                                COEFFSK_cheby_SP[inda][indp][5] + t*(
                                    COEFFSK_cheby_SP[inda][indp][6] + t*(
                                            COEFFSK_cheby_SP[inda][indp][7] + t*(
                                                COEFFSK_cheby_SP[inda][indp][8] + t*(
                                                    COEFFSK_cheby_SP[inda][indp][9] + t*(
                                                        COEFFSK_cheby_SP[inda][indp][10]
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                );
        REAL8 corr_extra = e2*e2*e2*e2*e2*e2*e2*( epr_cheby*de + COEFFSK_jhc_SP[inda][indp][7] );
        return exp(lnPref)*(eprLog0-(epr_jhc + corr_extra));
    }
    REAL8 eprL_expForm = 
        COEFFSK_jh[inda][indp][0] + e2*(
            COEFFSK_jh[inda][indp][1] + e2*(
                COEFFSK_jh[inda][indp][2] + e2*(
                    COEFFSK_jh[inda][indp][3] + e2*(
                        COEFFSK_jh[inda][indp][4] + e2*(
                            COEFFSK_jh[inda][indp][5] + e2*(
                                COEFFSK_jh[inda][indp][6]
                            )
                        )
                    )
                )
            )
        );
    REAL8 eprL_corr = 
        COEFFSK_jc[inda][indp][0] + de*(
            COEFFSK_jc[inda][indp][1] + de*(
                COEFFSK_jc[inda][indp][2] + COEFFSK_jc[inda][indp][3]*lnde
            )
        );
    epr_cheby = 
        COEFFSK_cheby[inda][indp][0] + t*(
            COEFFSK_cheby[inda][indp][1] + t*(
                COEFFSK_cheby[inda][indp][2] + t*(
                    COEFFSK_cheby[inda][indp][3] + t*(
                        COEFFSK_cheby[inda][indp][4] + t*(
                            COEFFSK_cheby[inda][indp][5] + t*(
                                COEFFSK_cheby[inda][indp][6] + t*(
                                    COEFFSK_cheby[inda][indp][7] + t*(
                                        COEFFSK_cheby[inda][indp][8] + t*(
                                            COEFFSK_cheby[inda][indp][9] + t*(
                                                COEFFSK_cheby[inda][indp][10]
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        );
    REAL8 corr = 1. + e2*e2*e2*e2*e2*e2*e2*( epr_cheby*de2 + eprL_corr );

    return exp(lnPref)*eprLog0-corr*exp(lnPref+eprL_expForm);

}
