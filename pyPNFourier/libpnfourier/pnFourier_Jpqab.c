/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/
#include "pnFourier_Jpqab.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

#define KMIN 5
#define NMIN 5
/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^1_pqa                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */



/* ----------  main  ------------ */
REAL8 J_0q01_series(INT q, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT min_eord = 2;
    INT kMax = (INT)(log(atol) / log(e)) - min_eord;
    kMax = (kMax < KMIN ? KMIN : kMax);
    BesselJCache *jc = CreateBesselJCache(kMax, q*e);
    REAL8 sum = 0.0;
    REAL8 betaPow = beta;
    for (INT k = 0;  ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache(2*k+1, jc);
        REAL8 term = bess * betaPow / (REAL8)(2*k+1);
        betaPow *= beta2;
        sum += term;
        if (fabs(term) <= rtol * fabs(sum)) {   /* early exit */
            if (k > min_eord) break;            /* tail symmetric */
        }
    }
    STRUCTFREE(jc, BesselJCache);
    return 2.*sum;
}

REAL8 J_pq01_series(INT p, INT q, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT min_eord = (p_abs==0 ? 2 : p_abs);
    INT kMax = (INT)(log(atol) / log(e)) - min_eord;
    kMax = (kMax < KMIN ? KMIN : kMax);

    /* Bessel array J_k(z) (both signs)                                */
    BesselJCache *jc = CreateBesselJCache(kMax, q*e);

    /* series summation                                                */
    REAL8 sumP = 0.0;
    REAL8 betaPow = beta;
    for (INT k = 1;  ; ++k) {
        REAL8 bessp = get_BesselJ_from_BesselJCache(k+p, jc);
        REAL8 term = bessp * betaPow / (REAL8)k;
        betaPow *= beta;
        sumP += term;
        if (fabs(term) <= rtol * fabs(sumP)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    REAL8 sumM = 0.0;
    betaPow = beta;
    for (INT k = 1;  ; ++k) {
        REAL8 bessm = get_BesselJ_from_BesselJCache(p-k, jc);
        REAL8 term = -bessm * betaPow / (REAL8)k;
        betaPow *= beta;
        sumM += term;
        if (fabs(term) <= rtol * fabs(sumM)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }
    STRUCTFREE(jc, BesselJCache);
    return sumP + sumM;   /* already includes 1/(2π) via Laplace */
}


REAL8 J_pqa1_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    if (p==0 && q==0) return 0.0;
    if (a==0 && p==0) return J_0q01_series(q, e, atol, rtol);
    if (a==0) return J_pq01_series(p, q, e, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    INT kMax = (INT)(fabs(xArg) + 8.0*sqrt(fabs(xArg)+1.0) + 25.0);
    INT kMin = KMIN;
    BesselJCache *bc = CreateBesselJCache(kMax, xArg);
    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;
    LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);

    /* ------------ init ------------ */
    INT ksum_cum = 0;
    INT ksum_cum_stop = 2;
    REAL8 tol_L = atol;
    REAL8 sum_p, sum_m;
    sum_p = 0.0;
    for (INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta;
        for (INT n = 1; ; n++)
        {
            REAL8 Lterm =  betaPow*(get_Laplace_from_LaplaceCache(m0+n, lc) - get_Laplace_from_LaplaceCache(m0-n, lc))/ (REAL8)n;
            sumL += Lterm;
            betaPow *= beta;
            if (fabs(Lterm) <= tol_L * fabs(sumL)) {
                if (n>2) break;
            }
        } /* n loop p */
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(k, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            ksum_cum++;
            if (k>2 && ksum_cum > ksum_cum_stop) break;
        } else
            ksum_cum = 0;
    } /* k loop */

    sum_m = 0;
    ksum_cum = 0;
    for (INT k = 1;  ; k++)
    {
        INT m0 = p-k;

        REAL8 sumL = 0.0;
        REAL8 betaPow = beta;
        for (INT n = 1; ; n++)
        {
            REAL8 Lterm =  betaPow*(get_Laplace_from_LaplaceCache(m0+n, lc) - get_Laplace_from_LaplaceCache(m0-n, lc))/ (REAL8)n;
            sumL += Lterm;
            betaPow *= beta;
            if (fabs(Lterm) <= tol_L * fabs(sumL)) {
                if (n>2) break;
            }
        } /* n loop */


        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(-k, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            ksum_cum++;
            if (k>2 && ksum_cum > ksum_cum_stop) break;
        } else 
            ksum_cum = 0;
    } /* k loop */

    STRUCTFREE(bc, BesselJCache);
    STRUCTFREE(lc, LaplaceCache);
    return pref * (sum_p + sum_m);
}

/* -------------------------------------------------------------------------
 * Optimized common fast path for J^(1), J^(2), and J^(3).
 *
 * Shift chi to x+i*s.  In addition to the exponential and denominator bound
 * used for J^(0), the Fourier series
 *
 *   i*delta_chi = sum_{n>=1} beta^n (exp(i*n*chi)-exp(-i*n*chi))/n
 *
 * gives a computable bound for |delta_chi| in the analytic strip.  If the
 * resulting rigorous integral bound is already below atol/(1-rtol), return
 * zero before allocating the p-sized Laplace table.  Otherwise use the
 * legacy evaluator unchanged: its nested beta convolutions in Eqs. (38)-(40)
 * are not equivalent to the single convolution optimized in J^(0).
 * ------------------------------------------------------------------------- */

static REAL8 J_pqab_contour_log_bound(REAL8 s, REAL8 p_abs, REAL8 q_abs,
                                      INT a, INT b, REAL8 e, REAL8 beta)
{
    REAL8 u_plus = beta * exp(s);
    REAL8 u_minus = beta * exp(-s);
    REAL8 den = 1.0 - e * cosh(s);
    if (!(u_plus < 1.0) || !(den > 0.0))
        return INFINITY;

    /* |i delta chi| <= sum beta^n(e^(ns)+e^(-ns))/n. */
    REAL8 delta_bound = -log1p(-u_plus) - log1p(-u_minus);
    if (!(delta_bound > 0.0))
        return INFINITY;
    return -p_abs * s + q_abs * e * sinh(s)
         - a * log(den) + b * log(delta_bound);
}

static INT J_pqab_below_absolute_tolerance(INT p, INT q, INT a, INT b,
                                            REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (!(atol > 0.0) || !(rtol >= 0.0) || rtol >= 1.0)
        return 0;

    REAL8 beta = eval_beta(e);
    REAL8 p_abs = fabs((REAL8)p);
    REAL8 q_abs = fabs((REAL8)q);
    REAL8 s_max = acosh(1.0 / e);
    REAL8 lo = 0.0;
    REAL8 hi = s_max * (1.0 - 16.0 * DBL_EPSILON);
    if (!(hi > 0.0))
        return 0;

    /* Golden-section minimization; retaining the s=0 endpoint is important. */
    const REAL8 golden = 0.618033988749894848204586834365638118;
    REAL8 x1 = hi - golden * (hi - lo);
    REAL8 x2 = lo + golden * (hi - lo);
    REAL8 f1 = J_pqab_contour_log_bound(x1, p_abs, q_abs, a, b, e, beta);
    REAL8 f2 = J_pqab_contour_log_bound(x2, p_abs, q_abs, a, b, e, beta);
    for (INT i = 0; i < 96; ++i) {
        if (f1 > f2) {
            lo = x1;
            x1 = x2;
            f1 = f2;
            x2 = lo + golden * (hi - lo);
            f2 = J_pqab_contour_log_bound(x2, p_abs, q_abs, a, b, e, beta);
        } else {
            hi = x2;
            x2 = x1;
            f2 = f1;
            x1 = hi - golden * (hi - lo);
            f1 = J_pqab_contour_log_bound(x1, p_abs, q_abs, a, b, e, beta);
        }
    }
    REAL8 log_bound = fmin(f1, f2);
    log_bound = fmin(log_bound,
        J_pqab_contour_log_bound(0.0, p_abs, q_abs, a, b, e, beta));
    return log_bound <= log(atol) - log1p(-rtol);
}

static REAL8 J_pqab_legacy(INT p, INT q, INT a, INT b, REAL8 e,
                           REAL8 atol, REAL8 rtol)
{
    if (b == 1) return J_pqa1_series(p, q, a, e, atol, rtol);
    if (b == 2) return J_pqa2_series(p, q, a, e, atol, rtol);
    return J_pqa3_series(p, q, a, e, atol, rtol);
}

static REAL8 J_pqab_legacy_cache(INT p, INT q, INT a, INT b, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (b == 1) return J_pqa1_series_cache(p, q, a, e, bc, lc, atol, rtol);
    if (b == 2) return J_pqa2_series_cache(p, q, a, e, bc, lc, atol, rtol);
    return J_pqa3_series_cache(p, q, a, e, bc, lc, atol, rtol);
}

static REAL8 J_pqab_series_optimized_impl(INT p, INT q, INT a, INT b,
                                           REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX || a < 0 || b < 1 || b > 3
        || atol < 0.0 || rtol < 0.0 || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: invalid input\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    REAL8 symmetry = 1.0;
    if (p < 0) {
        if (p == INT_MIN || q == INT_MIN)
            X_ERROR_REAL8(X_EINVAL);
        p = -p;
        q = -q;
        if (b & 1) symmetry = -1.0;
    }
    if ((b & 1) && p == 0 && q == 0)
        return 0.0;
    if (J_pqab_below_absolute_tolerance(p, q, a, b, e, atol, rtol))
        return 0.0;
    return symmetry * J_pqab_legacy(p, q, a, b, e, atol, rtol);
}

static REAL8 J_pqab_series_cache_optimized_impl(INT p, INT q, INT a, INT b,
    REAL8 e, BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX || a < 0 || b < 1 || b > 3 || !bc || !lc
        || atol < 0.0 || rtol < 0.0 || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: invalid input\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    REAL8 symmetry = 1.0;
    if (p < 0) {
        if (p == INT_MIN || q == INT_MIN)
            X_ERROR_REAL8(X_EINVAL);
        p = -p;
        q = -q;
        if (b & 1) symmetry = -1.0;
    }
    if ((b & 1) && p == 0 && q == 0)
        return 0.0;
    if (J_pqab_below_absolute_tolerance(p, q, a, b, e, atol, rtol))
        return 0.0;
    return symmetry * J_pqab_legacy_cache(p, q, a, b, e, bc, lc, atol, rtol);
}

REAL8 J_pqa1_series_optimized(INT p, INT q, INT a, REAL8 e,
                              REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_optimized_impl(p, q, a, 1, e, atol, rtol);
}

// with cache
REAL8 J_0q01_series_cache(INT q, REAL8 e, BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT min_eord = 2;
    INT kMax = (INT)(log(atol) / log(e)) - min_eord;
    kMax = (kMax < KMIN ? KMIN : kMax);
    REAL8 sum = 0.0;
    REAL8 betaPow = beta;
    for (INT k = 0;  ; ++k) {
        REAL8 bess = get_BesselJ_from_BesselJCache2D(2*k+1, q, bc);
        REAL8 term = bess * betaPow / (REAL8)(2*k+1);
        betaPow *= beta2;
        sum += term;
        if (fabs(term) <= rtol * fabs(sum)) {   /* early exit */
            if (k > min_eord) break;            /* tail symmetric */
        }
    }
    return 2.*sum;
}

REAL8 J_pq01_series_cache(INT p, INT q, REAL8 e, BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT min_eord = (p_abs==0 ? 2 : p_abs);
    INT kMax = (INT)(log(atol) / log(e)) - min_eord;
    kMax = (kMax < KMIN ? KMIN : kMax);

    /* Bessel array J_k(z) (both signs)                                */

    /* series summation                                                */
    REAL8 sumP = 0.0;
    REAL8 betaPow = beta;
    for (INT k = 1;  ; ++k) {
        REAL8 bessp = get_BesselJ_from_BesselJCache2D(k+p, q, bc);
        REAL8 term = bessp * betaPow / (REAL8)k;
        betaPow *= beta;
        sumP += term;
        if (fabs(term) <= rtol * fabs(sumP)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }

    REAL8 sumM = 0.0;
    betaPow = beta;
    for (INT k = 1;  ; ++k) {
        REAL8 bessm = get_BesselJ_from_BesselJCache2D(p-k, q, bc);
        REAL8 term = -bessm * betaPow / (REAL8)k;
        betaPow *= beta;
        sumM += term;
        if (fabs(term) <= rtol * fabs(sumM)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }
    return sumP + sumM;   /* already includes 1/(2π) via Laplace */
}


REAL8 J_pqa1_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    if (p==0 && q==0) return 0.0;
    if (a==0 && p==0) return J_0q01_series_cache(q, e, bc, lc, atol, rtol);
    if (a==0) return J_pq01_series_cache(p, q, e, bc, lc, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    INT kMax = (INT)(fabs(xArg) + 8.0*sqrt(fabs(xArg)+1.0) + 25.0);
    INT kMin = KMIN;
    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;

    /* ------------ init ------------ */
    INT ksum_cum = 0;
    INT ksum_cum_stop = 2;
    REAL8 tol_L = atol;
    REAL8 sum_p, sum_m;
    sum_p = 0.0;
    for (INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta;
        for (INT n = 1; ; n++)
        {
            REAL8 Lterm =  betaPow*(get_Laplace_from_LaplaceCache2D(m0+n, a, lc) - get_Laplace_from_LaplaceCache2D(m0-n, a, lc))/ (REAL8)n;
            sumL += Lterm;
            betaPow *= beta;
            if (fabs(Lterm) <= tol_L * fabs(sumL)) {
                if (n>2) break;
            }
        } /* n loop p */
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(k, q, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            ksum_cum++;
            if (k>2 && ksum_cum > ksum_cum_stop) break;
        } else
            ksum_cum = 0;
    } /* k loop */

    sum_m = 0;
    ksum_cum = 0;
    for (INT k = 1;  ; k++)
    {
        INT m0 = p-k;

        REAL8 sumL = 0.0;
        REAL8 betaPow = beta;
        for (INT n = 1; ; n++)
        {
            REAL8 Lterm =  betaPow*(get_Laplace_from_LaplaceCache2D(m0+n, a, lc) - get_Laplace_from_LaplaceCache2D(m0-n, a, lc))/ (REAL8)n;
            sumL += Lterm;
            betaPow *= beta;
            if (fabs(Lterm) <= tol_L * fabs(sumL)) {
                if (n>2) break;
            }
        } /* n loop */


        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(-k, q, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            ksum_cum++;
            if (k>2 && ksum_cum > ksum_cum_stop) break;
        } else 
            ksum_cum = 0;
    } /* k loop */

    return pref * (sum_p + sum_m);
}

REAL8 J_pqa1_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_cache_optimized_impl(p, q, a, 1, e, bc, lc, atol, rtol);
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

/* ----------  main  ------------ */

REAL8 J_pq02_series(INT p, INT q, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    const REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta*beta;
    const REAL8 xArg = q * e;

    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    BesselJCache *bc = CreateBesselJCache(kMax, xArg);

    REAL8 sum_p = 0.0;
    REAL8 betaPow = beta2;
    INT ncum = 0, ncumMax = 2;
    INT nMin = 2;
    for (INT n=0;; n++) {
        REAL8 term = 0.0;
        for (INT s=0; s<=n; s++) {
            term += (get_BesselJ_from_BesselJCache(p+n-2*s, bc) + get_BesselJ_from_BesselJCache(p-n+2*s, bc)
             - get_BesselJ_from_BesselJCache(p+n+2, bc) - get_BesselJ_from_BesselJCache(p-n-2, bc)) / ((s+1.)*(n-s+1.));
        }
        term *= betaPow;
        sum_p += term;
        betaPow *= beta;
        if (fabs(term) <= rtol * fabs(sum_p)) {
            ncum++;
            if (n > nMin && ncum > ncumMax) break;
        } else
            ncum = 0;
    }
    STRUCTFREE(bc, BesselJCache);
    return -(sum_p);
}

REAL8 J_pqa2_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a==0) return J_pq02_series(p, q, e, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    const INT kMin = KMIN;
    BesselJCache *bc = CreateBesselJCache(kMax, xArg);

    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;
    LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);
    /* ------------ n upper ------------ */
    INT kcum_max = 2;
    REAL8 beta2 = beta*beta;
    REAL8 tolL = atol*0.1;
    REAL8 sum_p = 0.0, sum_m = 0.0;
    INT kcum = 0;
    for ( INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta2;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s=0; s<=n ; s++) {
                term += (
                    get_Laplace_from_LaplaceCache(m0 + (n-2*s), lc) + 
                    get_Laplace_from_LaplaceCache(m0 - (n-2*s), lc) - 
                    get_Laplace_from_LaplaceCache(m0 + (n+2), lc) - 
                    get_Laplace_from_LaplaceCache(m0 - (n+2), lc) 
                ) / ((s+1.)*(n-s+1.));
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(k, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            kcum++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */

    // negative k
    kcum = 0;
    for ( INT k = 1;  ; k++)
    {
        INT m0 = -k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta2;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s=0; s<=n ; s++) {
                term += (
                    get_Laplace_from_LaplaceCache(m0 + (n-2*s), lc) + 
                    get_Laplace_from_LaplaceCache(m0 - (n-2*s), lc) - 
                    get_Laplace_from_LaplaceCache(m0 + (n+2), lc) - 
                    get_Laplace_from_LaplaceCache(m0 - (n+2), lc) 
                ) / ((s+1.)*(n-s+1.));
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(-k, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            kcum ++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */


    STRUCTFREE(bc, BesselJCache);
    STRUCTFREE(lc, LaplaceCache);
    return -pref * (sum_m + sum_p);
}

REAL8 J_pqa2_series_optimized(INT p, INT q, INT a, REAL8 e,
                              REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_optimized_impl(p, q, a, 2, e, atol, rtol);
}

// with cache
REAL8 J_pq02_series_cache(INT p, INT q, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    const REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta*beta;
    const REAL8 xArg = q * e;

    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);

    REAL8 sum_p = 0.0;
    REAL8 betaPow = beta2;
    INT ncum = 0, ncumMax = 2;
    INT nMin = 2;
    for (INT n=0;; n++) {
        REAL8 term = 0.0;
        for (INT s=0; s<=n; s++) {
            term += (get_BesselJ_from_BesselJCache2D(p+n-2*s, q, bc) + get_BesselJ_from_BesselJCache2D(p-n+2*s, q, bc)
             - get_BesselJ_from_BesselJCache2D(p+n+2, q, bc) - get_BesselJ_from_BesselJCache2D(p-n-2, q, bc)) / ((s+1.)*(n-s+1.));
        }
        term *= betaPow;
        sum_p += term;
        betaPow *= beta;
        if (fabs(term) <= rtol * fabs(sum_p)) {
            ncum++;
            if (n > nMin && ncum > ncumMax) break;
        } else
            ncum = 0;
    }
    return -(sum_p);
}

REAL8 J_pqa2_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a==0) return J_pq02_series_cache(p, q, e, bc, lc, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    const INT kMin = KMIN;

    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;
    /* ------------ n upper ------------ */
    INT kcum_max = 2;
    REAL8 beta2 = beta*beta;
    REAL8 tolL = atol*0.1;
    REAL8 sum_p = 0.0, sum_m = 0.0;
    INT kcum = 0;
    for ( INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta2;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s=0; s<=n ; s++) {
                term += (
                    get_Laplace_from_LaplaceCache2D(m0 + (n-2*s), a, lc) + 
                    get_Laplace_from_LaplaceCache2D(m0 - (n-2*s), a, lc) - 
                    get_Laplace_from_LaplaceCache2D(m0 + (n+2), a, lc) - 
                    get_Laplace_from_LaplaceCache2D(m0 - (n+2), a, lc) 
                ) / ((s+1.)*(n-s+1.));
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(k, q, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            kcum++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */

    // negative k
    kcum = 0;
    for ( INT k = 1;  ; k++)
    {
        INT m0 = -k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta2;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s=0; s<=n ; s++) {
                term += (
                    get_Laplace_from_LaplaceCache2D(m0 + (n-2*s), a, lc) + 
                    get_Laplace_from_LaplaceCache2D(m0 - (n-2*s), a, lc) - 
                    get_Laplace_from_LaplaceCache2D(m0 + (n+2), a, lc) - 
                    get_Laplace_from_LaplaceCache2D(m0 - (n+2), a, lc) 
                ) / ((s+1.)*(n-s+1.));
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(-k, q, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            kcum ++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */
    return -pref * (sum_m + sum_p);
}

REAL8 J_pqa2_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_cache_optimized_impl(p, q, a, 2, e, bc, lc, atol, rtol);
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

REAL8 J_pq03_series(INT p, INT q, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    const REAL8 beta = eval_beta(e);
    REAL8 beta3 = beta*beta*beta;
    const REAL8 xArg = q * e;

    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    BesselJCache *bc = CreateBesselJCache(kMax, xArg);

    REAL8 sum_p = 0.0;
    REAL8 betaPow = beta3;
    INT ncum = 0, ncumMax = 2;
    INT nMin = 2;
    for (INT n=0;; n++) {
        REAL8 term = 0.0;
        for (INT s1=0; s1<=n; s1++) {
            for (INT s2=0; s2<=s1; s2++){
                term += (
                    get_BesselJ_from_BesselJCache(p+(1+n-2*s2), bc) - 
                    get_BesselJ_from_BesselJCache(p-(1+n-2*s2), bc) +
                    get_BesselJ_from_BesselJCache(p+(1+n-2*s1+2*s2), bc) -
                    get_BesselJ_from_BesselJCache(p-(1+n-2*s1+2*s2), bc) -
                    get_BesselJ_from_BesselJCache(p+(n+3), bc) + 
                    get_BesselJ_from_BesselJCache(p-(n+3), bc) - 
                    get_BesselJ_from_BesselJCache(p+(n-1-2*s1), bc) +
                    get_BesselJ_from_BesselJCache(p-(n-1-2*s1), bc)
                ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
            }
        }
        term *= betaPow;
        sum_p += term;
        betaPow *= beta;
        if (fabs(term) <= rtol * fabs(sum_p)) {
            ncum++;
            if (n > nMin && ncum > ncumMax) break;
        } else
            ncum = 0;
    }
    STRUCTFREE(bc, BesselJCache);
    return -(sum_p);
}

REAL8 J_pq03_series_cache(INT p, INT q, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    const REAL8 beta = eval_beta(e);
    REAL8 beta3 = beta*beta*beta;
    const REAL8 xArg = q * e;

    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);

    REAL8 sum_p = 0.0;
    REAL8 betaPow = beta3;
    INT ncum = 0, ncumMax = 2;
    INT nMin = 2;
    for (INT n=0;; n++) {
        REAL8 term = 0.0;
        for (INT s1=0; s1<=n; s1++) {
            for (INT s2=0; s2<=s1; s2++){
                term += (
                    get_BesselJ_from_BesselJCache2D(p+(1+n-2*s2), q, bc) - 
                    get_BesselJ_from_BesselJCache2D(p-(1+n-2*s2), q, bc) +
                    get_BesselJ_from_BesselJCache2D(p+(1+n-2*s1+2*s2),q , bc) -
                    get_BesselJ_from_BesselJCache2D(p-(1+n-2*s1+2*s2), q, bc) -
                    get_BesselJ_from_BesselJCache2D(p+(n+3), q, bc) + 
                    get_BesselJ_from_BesselJCache2D(p-(n+3), q, bc) - 
                    get_BesselJ_from_BesselJCache2D(p+(n-1-2*s1), q, bc) +
                    get_BesselJ_from_BesselJCache2D(p-(n-1-2*s1), q, bc)
                ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
            }
        }
        term *= betaPow;
        sum_p += term;
        betaPow *= beta;
        if (fabs(term) <= rtol * fabs(sum_p)) {
            ncum++;
            if (n > nMin && ncum > ncumMax) break;
        } else
            ncum = 0;
    }
    return -(sum_p);
}

/* ----------  main  ------------ */

REAL8 J_pqa3_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }
    if (p==0 && q==0) return 0.0;
    if (a==0) return J_pq03_series(p, q, e, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    const INT kMin = KMIN;
    BesselJCache *bc = CreateBesselJCache(kMax, xArg);

    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;
    LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);
    /* ------------ n upper ------------ */
    INT kcum_max = 2;
    REAL8 beta3 = beta*beta*beta;
    REAL8 tolL = atol*0.1;
    REAL8 sum_p = 0.0, sum_m = 0.0;
    INT kcum = 0;
    for ( INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta3;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;

            for (INT s1=0; s1<=n; s1++) {
                for (INT s2=0; s2<=s1; s2++){
                    term += (
                        get_Laplace_from_LaplaceCache(m0+(1+n-2*s2), lc) - 
                        get_Laplace_from_LaplaceCache(m0-(1+n-2*s2), lc) +
                        get_Laplace_from_LaplaceCache(m0+(1+n-2*s1+2*s2), lc) -
                        get_Laplace_from_LaplaceCache(m0-(1+n-2*s1+2*s2), lc) -
                        get_Laplace_from_LaplaceCache(m0+(n+3), lc) + 
                        get_Laplace_from_LaplaceCache(m0-(n+3), lc) - 
                        get_Laplace_from_LaplaceCache(m0+(n-1-2*s1), lc) +
                        get_Laplace_from_LaplaceCache(m0-(n-1-2*s1), lc)
                    ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
                }
            }
                
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(k, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            kcum++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */

    // negative k
    kcum = 0;
    for ( INT k = 1;  ; k++)
    {
        INT m0 = -k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta3;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s1=0; s1<=n; s1++) {
                for (INT s2=0; s2<=s1; s2++){
                    term += (
                        get_Laplace_from_LaplaceCache(m0+(1+n-2*s2), lc) - 
                        get_Laplace_from_LaplaceCache(m0-(1+n-2*s2), lc) +
                        get_Laplace_from_LaplaceCache(m0+(1+n-2*s1+2*s2), lc) -
                        get_Laplace_from_LaplaceCache(m0-(1+n-2*s1+2*s2), lc) -
                        get_Laplace_from_LaplaceCache(m0+(n+3), lc) + 
                        get_Laplace_from_LaplaceCache(m0-(n+3), lc) - 
                        get_Laplace_from_LaplaceCache(m0+(n-1-2*s1), lc) +
                        get_Laplace_from_LaplaceCache(m0-(n-1-2*s1), lc)
                    ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
                }
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache(-k, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            kcum ++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */


    STRUCTFREE(bc, BesselJCache);
    STRUCTFREE(lc, LaplaceCache);
    return -pref * (sum_m + sum_p);
}

REAL8 J_pqa3_series_optimized(INT p, INT q, INT a, REAL8 e,
                              REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_optimized_impl(p, q, a, 3, e, atol, rtol);
}

// with cache

REAL8 J_pqa3_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }
    if (p==0 && q==0) return 0.0;
    if (a==0) return J_pq03_series_cache(p, q, e, bc, lc, atol, rtol);
    const REAL8 beta = eval_beta(e);
    const REAL8 pref = pow(1.0 + beta*beta, a);
    const REAL8 xArg = q * e;

    /* ------------ k upper ------------ */
    REAL8 lambda = beta * fabs(xArg) / 2.0;
    const INT kMax = (int)ceil(lambda + sqrt(6.0*lambda*log(1.0/(atol))) + 3.0);
    const INT kMin = KMIN;

    size_t nMax = abs(p) + 16; // just guess
    size_t nMin = NMIN;
    /* ------------ n upper ------------ */
    INT kcum_max = 2;
    REAL8 beta3 = beta*beta*beta;
    REAL8 tolL = atol*0.1;
    REAL8 sum_p = 0.0, sum_m = 0.0;
    INT kcum = 0;
    for ( INT k = 0;  ; k++)
    {
        INT m0 = k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta3;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;

            for (INT s1=0; s1<=n; s1++) {
                for (INT s2=0; s2<=s1; s2++){
                    term += (
                        get_Laplace_from_LaplaceCache2D(m0+(1+n-2*s2), a, lc) - 
                        get_Laplace_from_LaplaceCache2D(m0-(1+n-2*s2), a, lc) +
                        get_Laplace_from_LaplaceCache2D(m0+(1+n-2*s1+2*s2), a, lc) -
                        get_Laplace_from_LaplaceCache2D(m0-(1+n-2*s1+2*s2), a, lc) -
                        get_Laplace_from_LaplaceCache2D(m0+(n+3), a, lc) + 
                        get_Laplace_from_LaplaceCache2D(m0-(n+3), a, lc) - 
                        get_Laplace_from_LaplaceCache2D(m0+(n-1-2*s1), a, lc) +
                        get_Laplace_from_LaplaceCache2D(m0-(n-1-2*s1), a, lc)
                    ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
                }
            }
                
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(k, q, bc) * (sumL);
        sum_p += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_p)) {
            kcum++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */

    // negative k
    kcum = 0;
    for ( INT k = 1;  ; k++)
    {
        INT m0 = -k+p;
        REAL8 sumL = 0.0;
        REAL8 betaPow = beta3;
        for ( INT n = 0; ; n++ ) {
            REAL8 term = 0.0;
            for (INT s1=0; s1<=n; s1++) {
                for (INT s2=0; s2<=s1; s2++){
                    term += (
                        get_Laplace_from_LaplaceCache2D(m0+(1+n-2*s2), a, lc) - 
                        get_Laplace_from_LaplaceCache2D(m0-(1+n-2*s2), a, lc) +
                        get_Laplace_from_LaplaceCache2D(m0+(1+n-2*s1+2*s2), a, lc) -
                        get_Laplace_from_LaplaceCache2D(m0-(1+n-2*s1+2*s2), a, lc) -
                        get_Laplace_from_LaplaceCache2D(m0+(n+3), a, lc) + 
                        get_Laplace_from_LaplaceCache2D(m0-(n+3), a, lc) - 
                        get_Laplace_from_LaplaceCache2D(m0+(n-1-2*s1), a, lc) +
                        get_Laplace_from_LaplaceCache2D(m0-(n-1-2*s1), a, lc)
                    ) / ((s1-s2+1.)*(n-s1+1.)*(s2+1));
                }
            }
            term *= betaPow;
            sumL += term;
            betaPow *= beta;
            if (fabs(term) <= tolL * fabs(sumL)) {
                if (n>nMin) break;
            }
        }
        REAL8 termJ = (k&1 ? -1.0 : 1.0) * get_BesselJ_from_BesselJCache2D(-k, q, bc) * (sumL);
        sum_m += termJ;
        if (fabs(termJ) <= rtol * fabs(sum_m)) {
            kcum ++;
            if (k>kMin && kcum > kcum_max) break;
        } else 
            kcum = 0;
    } /* n loop */


    return -pref * (sum_m + sum_p);
}

REAL8 J_pqa3_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *bc, LaplaceCache2D *lc, REAL8 atol, REAL8 rtol)
{
    return J_pqab_series_cache_optimized_impl(p, q, a, 3, e, bc, lc, atol, rtol);
}


/**
 * 
 *              
 * 
 *                  Analytic Approximation
 * 
 * 
 */
#include "core_Jpqa1_Approx.h"
REAL8 Jpa1_Approx(int p, int a, REAL8 e)
{
    if(a < -4 || a>10)
    {
        XPrintError("Error - %s: a = %d must be in [1,6]\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }

    if(e < 0.0 || e >= 1.0)
    {
        XPrintError("Error - %s: e = %f must be in [0,1)\n", __func__, e);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 sign = p<0 ? -1. : 1.;
    int absp = abs(p);
    if (absp < 1 || absp > 200) {
        XPrintError("Error - %s: |p| = %d must > 5 and < 500\n", __func__, absp);
        X_ERROR_REAL8(X_EINVAL);
    }
    size_t indp, inda;
    indp = absp - 1;
    inda = a+1;

    REAL8 e2 = e*e;
    REAL8 de2 = 1. - e2;
    REAL8 de = sqrt(de2);
    REAL8 lnde = log(de);
    REAL8 lnPref = absp*log(e) - ( a>2 ? (2.*a-4.) : 0.0 )*lnde;
    REAL8 t = 2.*de-1.;
    REAL8 epr_cheby;
    // case0: a = -4,-3,-2
    if (a<-1) {
        // case: a=-4
        if (a==-4) {
            if (absp >= 35) { // 35 - 200
                REAL8 lneprL = COEFFSJb1_am4_jhc[absp-35][0] + e2*(
                    COEFFSJb1_am4_jhc[absp-35][1] + e2*(
                        COEFFSJb1_am4_jhc[absp-35][2]
                    )
                );
                epr_cheby = COEFFSJb1_ma4_cheby_V2[absp-35][0] + t*(
                    COEFFSJb1_ma4_cheby_V2[absp-35][1] + t*(
                        COEFFSJb1_ma4_cheby_V2[absp-35][2] + t*(
                            COEFFSJb1_ma4_cheby_V2[absp-35][3] + t*(
                                COEFFSJb1_ma4_cheby_V2[absp-35][4] + t*(
                                    COEFFSJb1_ma4_cheby_V2[absp-35][5] + t*(
                                        COEFFSJb1_ma4_cheby_V2[absp-35][6] + t*(
                                            COEFFSJb1_ma4_cheby_V2[absp-35][7] + t*(
                                                COEFFSJb1_ma4_cheby_V2[absp-35][8] + t*(
                                                    COEFFSJb1_ma4_cheby_V2[absp-35][9] + t*(
                                                        COEFFSJb1_ma4_cheby_V2[absp-35][10] + t*(
                                                            COEFFSJb1_ma4_cheby_V2[absp-35][11]
                                                        )
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
                return sign*(-exp(lnPref + lneprL)*(1. + e2*e2*e2*( de*epr_cheby + COEFFSJb1_am4_jhc[absp-35][3] )));
            } else if (absp>24) { // 25 - 35
                REAL8 lneprL = COEFFSJb1_am4_jhc_MP[absp-25][0] + e2*(
                    COEFFSJb1_am4_jhc_MP[absp-25][1] + e2*(
                        COEFFSJb1_am4_jhc_MP[absp-25][2]
                    )
                );
                epr_cheby = COEFFSJb1_ma4_cheby_MP[absp-25][0] + t*(
                    COEFFSJb1_ma4_cheby_MP[absp-25][1] + t*(
                        COEFFSJb1_ma4_cheby_MP[absp-25][2] + t*(
                            COEFFSJb1_ma4_cheby_MP[absp-25][3] + t*(
                                COEFFSJb1_ma4_cheby_MP[absp-25][4] + t*(
                                    COEFFSJb1_ma4_cheby_MP[absp-25][5] + t*(
                                        COEFFSJb1_ma4_cheby_MP[absp-25][6] + t*(
                                            COEFFSJb1_ma4_cheby_MP[absp-25][7] + t*(
                                                COEFFSJb1_ma4_cheby_MP[absp-25][8] + t*(
                                                    COEFFSJb1_ma4_cheby_MP[absp-25][9] + t*(
                                                        COEFFSJb1_ma4_cheby_MP[absp-25][10] + t*(
                                                            COEFFSJb1_ma4_cheby_MP[absp-25][11] + t*(
                                                                COEFFSJb1_ma4_cheby_MP[absp-25][12] + t*(
                                                                    COEFFSJb1_ma4_cheby_MP[absp-25][13] + t*(
                                                                        COEFFSJb1_ma4_cheby_MP[absp-25][14] + t*(
                                                                            COEFFSJb1_ma4_cheby_MP[absp-25][15] + t*COEFFSJb1_ma4_cheby_MP[absp-25][16]
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
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
                return sign*(-exp(lnPref)*(exp(lneprL) + e2*e2*e2*( de*epr_cheby + COEFFSJb1_am4_jhc_MP[absp-25][3] )));
            } else if (absp > 20) { // 21 - 24
                REAL8 eprLeft = 
                    COEFFSJb1_am4_jhc_SPMV3[absp-21][0] + e2*(
                        COEFFSJb1_am4_jhc_SPMV3[absp-21][1] + e2*(
                            COEFFSJb1_am4_jhc_SPMV3[absp-21][2] + e2*(
                                COEFFSJb1_am4_jhc_SPMV3[absp-21][3] + e2*(
                                    COEFFSJb1_am4_jhc_SPMV3[absp-21][4] + e2*(
                                        COEFFSJb1_am4_jhc_SPMV3[absp-21][5]
                                        )
                                    )
                                )
                            )
                        );
                epr_cheby = 
                    COEFFSJb1_ma4_cheby_SPMV3[absp-21][0] + t*(
                        COEFFSJb1_ma4_cheby_SPMV3[absp-21][1] + t*(
                            COEFFSJb1_ma4_cheby_SPMV3[absp-21][2] + t*(
                                COEFFSJb1_ma4_cheby_SPMV3[absp-21][3] + t*(
                                    COEFFSJb1_ma4_cheby_SPMV3[absp-21][4] + t*(
                                        COEFFSJb1_ma4_cheby_SPMV3[absp-21][5] + t*(
                                            COEFFSJb1_ma4_cheby_SPMV3[absp-21][6] + t*(
                                                COEFFSJb1_ma4_cheby_SPMV3[absp-21][7] + t*(
                                                    COEFFSJb1_ma4_cheby_SPMV3[absp-21][8] + t*COEFFSJb1_ma4_cheby_SPMV3[absp-21][9]
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    );
                REAL8 corr_SP = e2*e2*e2*e2*e2*e2*(  COEFFSJb1_am4_jhc_SPMV3[absp-21][6] + de*(
                    COEFFSJb1_am4_jhc_SPMV3[absp-21][7] + de*epr_cheby
                ) );
                return -sign*exp(lnPref)*( eprLeft + corr_SP );
            } else { // 1 - 20
                REAL8 eprLeft = 
                    COEFFSJb1_am4_jhc_SP_V3[indp][0] + e2*(
                        COEFFSJb1_am4_jhc_SP_V3[indp][1] + e2*(
                            COEFFSJb1_am4_jhc_SP_V3[indp][2] + e2*(
                                COEFFSJb1_am4_jhc_SP_V3[indp][3] + e2*(
                                    COEFFSJb1_am4_jhc_SP_V3[indp][4] + e2*(
                                        COEFFSJb1_am4_jhc_SP_V3[indp][5] + e2*(
                                            COEFFSJb1_am4_jhc_SP_V3[indp][6]
                                        )
                                    )
                                )
                            )
                        )
                    );
                epr_cheby = 
                    COEFFSJb1_ma4_cheby_SP_V3[indp][0] + t*(
                        COEFFSJb1_ma4_cheby_SP_V3[indp][1] + t*(
                            COEFFSJb1_ma4_cheby_SP_V3[indp][2] + t*(
                                COEFFSJb1_ma4_cheby_SP_V3[indp][3] + t*(
                                    COEFFSJb1_ma4_cheby_SP_V3[indp][4] + t*(
                                        COEFFSJb1_ma4_cheby_SP_V3[indp][5] + t*(
                                            COEFFSJb1_ma4_cheby_SP_V3[indp][6] + t*(
                                                COEFFSJb1_ma4_cheby_SP_V3[indp][7] + t*(
                                                    COEFFSJb1_ma4_cheby_SP_V3[indp][8]
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    );
                REAL8 corr_SP = e2*e2*e2*e2*e2*e2*e2*(  COEFFSJb1_am4_jhc_SP_V3[indp][7] + de*(
                    COEFFSJb1_am4_jhc_SP_V3[indp][8] + de*epr_cheby
                ) );
                return -sign*exp(lnPref)*( eprLeft + corr_SP );
            }
        } // a = -4


        // case0.1: p>=21 a = -3 & -2
        if (absp>=21) {
            REAL8 lneprR = 
                COEFFSJb1_majc[a+3][absp-10][0] + e2*(
                    COEFFSJb1_majc[a+3][absp-10][1] + e2*(
                        COEFFSJb1_majc[a+3][absp-10][2]
                    )
                );
            epr_cheby = 
                COEFFSJb1_macheby[a+3][absp-10][0] + t*(
                    COEFFSJb1_macheby[a+3][absp-10][1] + t*(
                        COEFFSJb1_macheby[a+3][absp-10][2] + t*(
                            COEFFSJb1_macheby[a+3][absp-10][3] + t*(
                                COEFFSJb1_macheby[a+3][absp-10][4] + t*(
                                    COEFFSJb1_macheby[a+3][absp-10][5] + t*(
                                        COEFFSJb1_macheby[a+3][absp-10][6] + t*(
                                            COEFFSJb1_macheby[a+3][absp-10][7] + t*(
                                                COEFFSJb1_macheby[a+3][absp-10][8] + t*(
                                                    COEFFSJb1_macheby[a+3][absp-10][9] + t*(
                                                        COEFFSJb1_macheby[a+3][absp-10][10] + t*(
                                                            COEFFSJb1_macheby[a+3][absp-10][11] + t*COEFFSJb1_macheby[a+3][absp-10][12]
                                                        )
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
            return exp(lneprR+lnPref)*(1. + e2*e2*e2*( de*epr_cheby + COEFFSJb1_majc[a+3][absp-10][3]) );
        } else {
            REAL8 eprLeft = 
                COEFFSJb1_majc_SP[a+3][indp][0] + e2*(
                    COEFFSJb1_majc_SP[a+3][indp][1] + e2*(
                        COEFFSJb1_majc_SP[a+3][indp][2] + e2*(
                            COEFFSJb1_majc_SP[a+3][indp][3] + e2*(
                                COEFFSJb1_majc_SP[a+3][indp][4] + e2*(
                                    COEFFSJb1_majc_SP[a+3][indp][5] + e2*(
                                        COEFFSJb1_majc_SP[a+3][indp][6]
                                    )
                                )
                            )
                        )
                    )
                );
            REAL8 eprRCorr = COEFFSJb1_majc_SP[a+3][indp][7] + COEFFSJb1_majc_SP[a+3][indp][8]*de;
            epr_cheby = 
                COEFFSJb1_macheby_SP[a+3][indp][0] + t*(
                    COEFFSJb1_macheby_SP[a+3][indp][1] + t*(
                        COEFFSJb1_macheby_SP[a+3][indp][2] + t*(
                            COEFFSJb1_macheby_SP[a+3][indp][3] + t*(
                                COEFFSJb1_macheby_SP[a+3][indp][4] + t*(
                                    COEFFSJb1_macheby_SP[a+3][indp][5] + t*(
                                        COEFFSJb1_macheby_SP[a+3][indp][6] + t*(
                                            COEFFSJb1_macheby_SP[a+3][indp][7] + t*(
                                                COEFFSJb1_macheby_SP[a+3][indp][8]
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                );
            REAL8 corr_SP = e2*e2*e2*e2*e2*e2*e2*( de2*epr_cheby + eprRCorr );
            return sign*exp(lnPref)*( eprLeft + corr_SP );
        }
    }

    // case1: p=1, a=8 or 9
    if (absp==1) {
        if (a==9) {
            REAL8 eprL = 
                0.500000000000000 + e2*(
                    1.43750000000000 + e2*(
                        0.648437500000000 + e2*(
                            0.176812065972222+ e2*(
                                0.0896491156684028 + e2*(
                                    0.0573513398347078 + e2*0.0416825086603720
                                )
                            )
                        )
                    )
                );
            REAL8 eprR = 0.420079746650010-1.80468750000000*de;
            epr_cheby = 
                2.26933922991211 + t*(
                    -1.33551295697196 + t*(
                        0.733048799802907 + t*(
                            -0.405359954266009 + t*(
                                0.263391744058399 + t*(
                                    -0.111820537734589)
                                )
                            )
                        )
                    );
            REAL8 corr_sp = e2*e2*e2*e2*e2*e2*e2*(de2 * epr_cheby + eprR);
            return sign*( -exp(lnPref)*( eprL + corr_sp ) );
        } else if (a==10) {
            REAL8 eprL = 
                0.500000000000000 + e2*(
                    2.12500000000000 + e2*(
                        1.51822916666667 + e2*(
                            0.405924479166667 + e2*(
                                0.172681342230903 + e2*(
                                    0.105573108814381 + e2*0.0745658757110542
                                )
                            )
                        )
                    )
                );
            REAL8 eprR = 0.709824349169588-2.97916666666667*de;
            epr_cheby = 
                3.71190896192818 + t*(
                    -2.16294089158575 + t*(
                        1.17613118885686 + t*(
                            -0.643675344288202 + t*(
                                0.412017891258697 + t*(
                                    -0.172859762081057)
                                )
                            )
                        )
                    );
            REAL8 corr_sp = e2*e2*e2*e2*e2*e2*e2*(de2 * epr_cheby + eprR);
            return sign*( -exp(lnPref)*( eprL + corr_sp ) );
        }
    }
    REAL8 lnmeprL = 
        COEFFSJb1_jh[inda][indp][0] + e2*(
            COEFFSJb1_jh[inda][indp][1] + e2*(
                COEFFSJb1_jh[inda][indp][2] + e2*(
                    COEFFSJb1_jh[inda][indp][3] + e2*(
                        COEFFSJb1_jh[inda][indp][4] + e2*(
                            COEFFSJb1_jh[inda][indp][5] + e2*(
                                COEFFSJb1_jh[inda][indp][6]
                            )
                        )
                    )
                )
            )
        );
    REAL8 explnmeprL_corr0 = COEFFSJb1_jh[inda][indp][7] + COEFFSJb1_jh[inda][indp][8]*lnde;
    epr_cheby = 
        COEFFSJb1_cheby[inda][indp][0] + t*(
            COEFFSJb1_cheby[inda][indp][1] + t*(
                COEFFSJb1_cheby[inda][indp][2] + t*(
                    COEFFSJb1_cheby[inda][indp][3] + t*(
                        COEFFSJb1_cheby[inda][indp][4] + t*(
                            COEFFSJb1_cheby[inda][indp][5] + t*(
                                COEFFSJb1_cheby[inda][indp][6] + t*(
                                    COEFFSJb1_cheby[inda][indp][7] + t*(
                                        COEFFSJb1_cheby[inda][indp][8] + t*COEFFSJb1_cheby[inda][indp][9]
                                    )
                                )
                            )
                        )
                    )
                )
            )
        );
    REAL8 corr = 1. + e2*e2*e2*e2*e2*e2*e2*( epr_cheby*de + explnmeprL_corr0 );

    return sign*( -corr*exp(lnPref + lnmeprL) );

}


#include "core_Jpqa2_Approx.h"
REAL8 Jpa2_Approx(int p, int a, REAL8 e)
{
    if(a < -3 || a>10)
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
    REAL8 lnPref = absp*log(e) - ( a>0 ? (2.*a-1.) : 0.0 )*lnde;
    REAL8 t = 2.*de-1.;
    REAL8 epr_cheby;
    // case 0: a==-2
    if (a==-2) {
        if (absp>=90) {
            REAL8 lneprR = 
                COEFFSJb2_am2_jhc_LP[absp-90][0] + e2*(
                    COEFFSJb2_am2_jhc_LP[absp-90][1] + e2*(
                        COEFFSJb2_am2_jhc_LP[absp-90][2]
                    )
                );
            epr_cheby = 
                COEFFSJb2_ma2_cheby_LP[absp-90][0] + t*(
                    COEFFSJb2_ma2_cheby_LP[absp-90][1] + t*(
                        COEFFSJb2_ma2_cheby_LP[absp-90][2] + t*(
                            COEFFSJb2_ma2_cheby_LP[absp-90][3] + t*(
                                COEFFSJb2_ma2_cheby_LP[absp-90][4] + t*(
                                    COEFFSJb2_ma2_cheby_LP[absp-90][5] + t*(
                                        COEFFSJb2_ma2_cheby_LP[absp-90][6] + t*(
                                            COEFFSJb2_ma2_cheby_LP[absp-90][7] + t*(
                                                COEFFSJb2_ma2_cheby_LP[absp-90][8] + t*(
                                                    COEFFSJb2_ma2_cheby_LP[absp-90][9] + t*(
                                                        COEFFSJb2_ma2_cheby_LP[absp-90][10]
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
            return -exp(lneprR+lnPref)*(1. + e2*e2*e2*( de*epr_cheby + COEFFSJb2_am2_jhc_LP[absp-90][3] ));
        } else if (absp>=51) { // 51 - 89
            REAL8 lneprL = (COEFFSJb2_am2_jhc_MP1[absp-51][0] + e2*COEFFSJb2_am2_jhc_MP1[absp-51][1]);
            epr_cheby = 
                COEFFSJb2_ma2_cheby_MP1[absp-51][0] + t*(
                    COEFFSJb2_ma2_cheby_MP1[absp-51][1] + t*(
                        COEFFSJb2_ma2_cheby_MP1[absp-51][2] + t*(
                            COEFFSJb2_ma2_cheby_MP1[absp-51][3] + t*(
                                COEFFSJb2_ma2_cheby_MP1[absp-51][4] + t*(
                                    COEFFSJb2_ma2_cheby_MP1[absp-51][5] + t*(
                                        COEFFSJb2_ma2_cheby_MP1[absp-51][6] + t*(
                                            COEFFSJb2_ma2_cheby_MP1[absp-51][7] + t*(
                                                COEFFSJb2_ma2_cheby_MP1[absp-51][8] + t*(
                                                    COEFFSJb2_ma2_cheby_MP1[absp-51][9] + t*(
                                                        COEFFSJb2_ma2_cheby_MP1[absp-51][10] + t*(
                                                            COEFFSJb2_ma2_cheby_MP1[absp-51][11] + t*(
                                                                COEFFSJb2_ma2_cheby_MP1[absp-51][12] + t*(
                                                                    COEFFSJb2_ma2_cheby_MP1[absp-51][13] + t*(
                                                                        COEFFSJb2_ma2_cheby_MP1[absp-51][14]
                                                                    )
                                                                )
                                                            )
                                                        )
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
            return -exp(lnPref + lneprL)*(1. + e2*e2*(de*epr_cheby + COEFFSJb2_am2_jhc_MP1[absp-51][2]));
        } else if ( absp>=40 ) { // 40 - 50
            REAL8 eprFit = COEFFSJb2_am2_jhc_MP2[absp-40][0] + de*(
                    COEFFSJb2_am2_jhc_MP2[absp-40][1] + de*(
                        COEFFSJb2_am2_jhc_MP2[absp-40][2] + de*(
                            COEFFSJb2_am2_jhc_MP2[absp-40][3] + de*(
                                COEFFSJb2_am2_jhc_MP2[absp-40][4]
                            )
                        )
                    )
                );
            REAL8 tt = 2*e-1;
            epr_cheby = COEFFSJb2_ma2_cheby_MP2[absp-40][0] + tt*(
                    COEFFSJb2_ma2_cheby_MP2[absp-40][1] + tt*(
                        COEFFSJb2_ma2_cheby_MP2[absp-40][2] + tt*(
                            COEFFSJb2_ma2_cheby_MP2[absp-40][3] + tt*(
                                COEFFSJb2_ma2_cheby_MP2[absp-40][4] + tt*(
                                    COEFFSJb2_ma2_cheby_MP2[absp-40][5] + tt*(
                                        COEFFSJb2_ma2_cheby_MP2[absp-40][6] + tt*(
                                            COEFFSJb2_ma2_cheby_MP2[absp-40][7] + tt*(
                                                COEFFSJb2_ma2_cheby_MP2[absp-40][8] + tt*(
                                                    COEFFSJb2_ma2_cheby_MP2[absp-40][9] + tt*(
                                                        COEFFSJb2_ma2_cheby_MP2[absp-40][10] + tt*(
                                                            COEFFSJb2_ma2_cheby_MP2[absp-40][11] + tt*(
                                                                COEFFSJb2_ma2_cheby_MP2[absp-40][12] + tt*(
                                                                    COEFFSJb2_ma2_cheby_MP2[absp-40][13] + tt*(
                                                                        COEFFSJb2_ma2_cheby_MP2[absp-40][14] + tt*(
                                                                            COEFFSJb2_ma2_cheby_MP2[absp-40][15] + tt*(
                                                                                COEFFSJb2_ma2_cheby_MP2[absp-40][16] + tt*(
                                                                                    COEFFSJb2_ma2_cheby_MP2[absp-40][17] + tt*(
                                                                                        COEFFSJb2_ma2_cheby_MP2[absp-40][18] + tt*(
                                                                                            COEFFSJb2_ma2_cheby_MP2[absp-40][19]
                                                                                        )
                                                                                    )
                                                                                )
                                                                            )
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
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
            return exp(lnPref)*(eprFit + epr_cheby);
        } else if (absp>=26) { // 26 - 39
            REAL8 eprFit = COEFFSJb2_am2_jhc_SP2[absp-26][0] + de*(
                    COEFFSJb2_am2_jhc_SP2[absp-26][1] + de*(
                        COEFFSJb2_am2_jhc_SP2[absp-26][2] + de*COEFFSJb2_am2_jhc_SP2[absp-26][3]
                    )
                );
            epr_cheby = 
                COEFFSJb2_ma2_cheby_SP2[absp-26][0] + t*(
                    COEFFSJb2_ma2_cheby_SP2[absp-26][1] + t*(
                        COEFFSJb2_ma2_cheby_SP2[absp-26][2] + t*(
                            COEFFSJb2_ma2_cheby_SP2[absp-26][3] + t*(
                                COEFFSJb2_ma2_cheby_SP2[absp-26][4] + t*(
                                    COEFFSJb2_ma2_cheby_SP2[absp-26][5] + t*(
                                        COEFFSJb2_ma2_cheby_SP2[absp-26][6] + t*(
                                            COEFFSJb2_ma2_cheby_SP2[absp-26][7] + t*(
                                                COEFFSJb2_ma2_cheby_SP2[absp-26][8] + t*(
                                                    COEFFSJb2_ma2_cheby_SP2[absp-26][9] + t*(
                                                        COEFFSJb2_ma2_cheby_SP2[absp-26][10] + t*(
                                                            COEFFSJb2_ma2_cheby_SP2[absp-26][11] + t*(
                                                                COEFFSJb2_ma2_cheby_SP2[absp-26][12] + t*(
                                                                    COEFFSJb2_ma2_cheby_SP2[absp-26][13]
                                                                )
                                                            )
                                                        )
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
            return exp(lnPref + eprFit + e2*e2*de*( epr_cheby ));
        } else { // 1 - 25
            REAL8 taylorL = 
                COEFFSJb2_am2_jhc_SP[absp-1][0] + e2*(
                    COEFFSJb2_am2_jhc_SP[absp-1][1] + e2*(
                        COEFFSJb2_am2_jhc_SP[absp-1][2] + e2*(
                            COEFFSJb2_am2_jhc_SP[absp-1][3] + e2*(
                                COEFFSJb2_am2_jhc_SP[absp-1][4] + e2*(
                                    COEFFSJb2_am2_jhc_SP[absp-1][5] + e2*(
                                        COEFFSJb2_am2_jhc_SP[absp-1][6]
                                    )
                                )
                            )
                        )
                    )
                );
            epr_cheby = 
                COEFFSJb2_ma2_cheby_SP[absp-1][0] + t*(
                    COEFFSJb2_ma2_cheby_SP[absp-1][1] + t*(
                        COEFFSJb2_ma2_cheby_SP[absp-1][2] + t*(
                            COEFFSJb2_ma2_cheby_SP[absp-1][3] + t*(
                                COEFFSJb2_ma2_cheby_SP[absp-1][4] + t*(
                                    COEFFSJb2_ma2_cheby_SP[absp-1][5] + t*(
                                        COEFFSJb2_ma2_cheby_SP[absp-1][6] + t*(
                                            COEFFSJb2_ma2_cheby_SP[absp-1][7] + t*(
                                                COEFFSJb2_ma2_cheby_SP[absp-1][8] + t*(
                                                    COEFFSJb2_ma2_cheby_SP[absp-1][9] + t*(
                                                        COEFFSJb2_ma2_cheby_SP[absp-1][10]
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
            return exp(lnPref)*(taylorL + e2*e2*e2*e2*e2*e2*e2*( de*epr_cheby + COEFFSJb2_am2_jhc_SP[absp-1][7]));
        }
    }
    // case 1: a>=-1 or a=-3
    int inda = a==-3 ? 0 : a+2;
    if (absp >= 10) {
        REAL8 sign = a==-3 ? -1.0 : 1.0;
        REAL8 lneprL = COEFFSJb2_jh[inda][absp-10][0] + e2*(
            COEFFSJb2_jh[inda][absp-10][1] + e2*COEFFSJb2_jh[inda][absp-10][2]
        );
        if (absp >= 50)
            epr_cheby = 
                COEFFSJb2_chebyV2LP[inda][absp-50][0] + t*(
                    COEFFSJb2_chebyV2LP[inda][absp-50][1] + t*(
                        COEFFSJb2_chebyV2LP[inda][absp-50][2] + t*(
                            COEFFSJb2_chebyV2LP[inda][absp-50][3] + t*(
                                COEFFSJb2_chebyV2LP[inda][absp-50][4] + t*(
                                    COEFFSJb2_chebyV2LP[inda][absp-50][5] + t*(
                                        COEFFSJb2_chebyV2LP[inda][absp-50][6] + t*(
                                            COEFFSJb2_chebyV2LP[inda][absp-50][7] + t*((
                                                COEFFSJb2_chebyV2LP[inda][absp-50][8] + t*(
                                                    COEFFSJb2_chebyV2LP[inda][absp-50][9] + t*(
                                                        COEFFSJb2_chebyV2LP[inda][absp-50][10] + t*(
                                                            COEFFSJb2_chebyV2LP[inda][absp-50][11] + t*(
                                                                COEFFSJb2_chebyV2LP[inda][absp-50][12] + t*(
                                                                    COEFFSJb2_chebyV2LP[inda][absp-50][13] + t*(
                                                                        COEFFSJb2_chebyV2LP[inda][absp-50][14] + t*(
                                                                            COEFFSJb2_chebyV2LP[inda][absp-50][15] + t*(
                                                                                COEFFSJb2_chebyV2LP[inda][absp-50][16] + t*COEFFSJb2_chebyV2LP[inda][absp-50][17] 
                                                                            ) 
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ));
        else
            epr_cheby = 
                COEFFSJb2_cheby[inda][absp-10][0] + t*(
                    COEFFSJb2_cheby[inda][absp-10][1] + t*(
                        COEFFSJb2_cheby[inda][absp-10][2] + t*(
                            COEFFSJb2_cheby[inda][absp-10][3] + t*(
                                COEFFSJb2_cheby[inda][absp-10][4] + t*(
                                    COEFFSJb2_cheby[inda][absp-10][5] + t*(
                                        COEFFSJb2_cheby[inda][absp-10][6] + t*(
                                            COEFFSJb2_cheby[inda][absp-10][7] + t*((
                                                COEFFSJb2_cheby[inda][absp-10][8] + t*(
                                                    COEFFSJb2_cheby[inda][absp-10][9] + t*(
                                                        COEFFSJb2_cheby[inda][absp-10][10] + t*(
                                                            COEFFSJb2_cheby[inda][absp-10][11] + t*(
                                                                COEFFSJb2_cheby[inda][absp-10][12] + t*(
                                                                    COEFFSJb2_cheby[inda][absp-10][13]
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ));
        return sign*exp(lnPref + lneprL)*(1. + e2*e2*e2*epr_cheby);
    } else {
        // p<10
        epr_cheby = 
            COEFFSJb2_cheby_SP[inda][absp-1][0] + t*(
                COEFFSJb2_cheby_SP[inda][absp-1][1] + t*(
                    COEFFSJb2_cheby_SP[inda][absp-1][2] + t*(
                        COEFFSJb2_cheby_SP[inda][absp-1][3] + t*(
                            COEFFSJb2_cheby_SP[inda][absp-1][4] + t*(
                                COEFFSJb2_cheby_SP[inda][absp-1][5] + t*(
                                    COEFFSJb2_cheby_SP[inda][absp-1][6] + t*(
                                        COEFFSJb2_cheby_SP[inda][absp-1][7] + t*(
                                            COEFFSJb2_cheby_SP[inda][absp-1][8]
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 eprR = 
            COEFFSJb2_jhc_SP[inda][absp-1][7] + de*( COEFFSJb2_jhc_SP[inda][absp-1][8] + lnde*COEFFSJb2_jhc_SP[inda][absp-1][9]);
        REAL8 eprL = 
            COEFFSJb2_jhc_SP[inda][absp-1][0] + e2*(
                COEFFSJb2_jhc_SP[inda][absp-1][1] + e2*(
                    COEFFSJb2_jhc_SP[inda][absp-1][2] + e2*(
                        COEFFSJb2_jhc_SP[inda][absp-1][3] + e2*(
                            COEFFSJb2_jhc_SP[inda][absp-1][4] + e2*(
                                COEFFSJb2_jhc_SP[inda][absp-1][5] + e2*(
                                    COEFFSJb2_jhc_SP[inda][absp-1][6]
                                )
                            )
                        )
                    )
                )
            );
        return exp(lnPref)*(eprL + e2*e2*e2*e2*e2*e2*e2*( de2*epr_cheby + eprR ) );
    }

    return 0.0;
}



#include "core_Jpqa3_Approx.h"
REAL8 Jpa3_Approx(int p, int a, REAL8 e)
{
    if(a < -2 || a>2 )
    {
        XPrintError("Error - %s: a = %d must be in [1,6]\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }

    if(e < 0.0 || e >= 1.0)
    {
        XPrintError("Error - %s: e = %f must be in [0,1)\n", __func__, e);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 sign = p<0 ? -1. : 1.;
    int absp = abs(p);
    if (absp < 1 || absp > 200) {
        XPrintError("Error - %s: |p| = %d must > 5 and < 500\n", __func__, absp);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 e2 = e*e;
    REAL8 de2 = 1. - e2;
    REAL8 de = sqrt(de2);
    REAL8 lnde = log(de);
    REAL8 lnPref = absp*log(e);
    REAL8 t = 2.*de-1.;
    REAL8 epr_cheby;
    if (a==2) {
        REAL8 lnTerm = (2.*absp*CST_PISQ/3. + 8.*absp*de)*lnde;
        if (absp >19) {
            int ip = absp-20;
            REAL8 lneprL = 
                COEFFSJb3_a2_jhc[ip][0] + e2*(
                    COEFFSJb3_a2_jhc[ip][1] + e2*(
                        COEFFSJb3_a2_jhc[ip][2]
                    )
                );
            epr_cheby = COEFFSJb3_a2_cheby[ip][0] + t*(
                    COEFFSJb3_a2_cheby[ip][1] + t*(
                        COEFFSJb3_a2_cheby[ip][2] + t*(
                            COEFFSJb3_a2_cheby[ip][3] + t*(
                                COEFFSJb3_a2_cheby[ip][4] + t*(
                                    COEFFSJb3_a2_cheby[ip][5] + t*(
                                        COEFFSJb3_a2_cheby[ip][6] + t*(
                                            COEFFSJb3_a2_cheby[ip][7] + t*(
                                                COEFFSJb3_a2_cheby[ip][8] + t*(
                                                    COEFFSJb3_a2_cheby[ip][9] + t*(
                                                        COEFFSJb3_a2_cheby[ip][10] + t*(
                                                            COEFFSJb3_a2_cheby[ip][11] + t*(
                                                                COEFFSJb3_a2_cheby[ip][12] + t*(
                                                                    COEFFSJb3_a2_cheby[ip][13] + t*(
                                                                        COEFFSJb3_a2_cheby[ip][14]
                                                                    )
                                                                )
                                                            )
                                                        )
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
            
            return sign*( -exp(lneprL + lnPref)*(1. + e2*e2*e2*(de * epr_cheby + COEFFSJb3_a2_jhc[ip][3])) - exp(lnPref)*lnTerm );
        } else {
            int ip = absp-1;
            REAL8 eprLTaylor = 
                COEFFSJb3_a2_jhc_SP[ip][0] + e2*(
                    COEFFSJb3_a2_jhc_SP[ip][1] + e2*(
                        COEFFSJb3_a2_jhc_SP[ip][2] + e2*(
                            COEFFSJb3_a2_jhc_SP[ip][3] + e2*(
                                COEFFSJb3_a2_jhc_SP[ip][4] + e2*(
                                    COEFFSJb3_a2_jhc_SP[ip][5] + e2*(
                                        COEFFSJb3_a2_jhc_SP[ip][6]
                                    )
                                )
                            )
                        )
                    )
                );
            if (absp>2)
                epr_cheby = COEFFSJb3_a2_cheby_SP[ip][0] + t*(
                        COEFFSJb3_a2_cheby_SP[ip][1] + t*(
                            COEFFSJb3_a2_cheby_SP[ip][2] + t*(
                                COEFFSJb3_a2_cheby_SP[ip][3] + t*(
                                    COEFFSJb3_a2_cheby_SP[ip][4] + t*(
                                        COEFFSJb3_a2_cheby_SP[ip][5] + t*(
                                            COEFFSJb3_a2_cheby_SP[ip][6] + t*(
                                                COEFFSJb3_a2_cheby_SP[ip][7] + t*(
                                                    COEFFSJb3_a2_cheby_SP[ip][8]
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    );
                else 
                    epr_cheby = COEFFSJb3_a2_cheby_HSP[ip][0] + t*(
                            COEFFSJb3_a2_cheby_HSP[ip][1] + t*(
                                COEFFSJb3_a2_cheby_HSP[ip][2] + t*(
                                    COEFFSJb3_a2_cheby_HSP[ip][3] + t*(
                                        COEFFSJb3_a2_cheby_HSP[ip][4] + t*(
                                            COEFFSJb3_a2_cheby_HSP[ip][5] + t*(
                                                COEFFSJb3_a2_cheby_HSP[ip][6] + t*(
                                                    COEFFSJb3_a2_cheby_HSP[ip][7] + t*(
                                                        COEFFSJb3_a2_cheby_HSP[ip][8] + t*(
                                                            COEFFSJb3_a2_cheby_HSP[ip][9] + t*(
                                                                COEFFSJb3_a2_cheby_HSP[ip][10]
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
            return sign* ( -exp(lnPref)*( eprLTaylor + e2*e2*e2*e2*e2*e2*e2*( epr_cheby*de + COEFFSJb3_a2_jhc_SP[ip][7] ) + lnTerm ) );
        }
    }
    // a!=2
    if (absp >= 15) {
        int ia = a+2;
        int ip = absp-15;
        REAL8 lneprL = 
            COEFFSJb3_jh[ia][ip][0] + e2*(
                COEFFSJb3_jh[ia][ip][1] + e2*(
                    COEFFSJb3_jh[ia][ip][2]
                )
            );
        epr_cheby = 
            COEFFSJb3_cheby[ia][ip][0] + t*(
                COEFFSJb3_cheby[ia][ip][1] + t*(
                    COEFFSJb3_cheby[ia][ip][2] + t*(
                        COEFFSJb3_cheby[ia][ip][3] + t*(
                            COEFFSJb3_cheby[ia][ip][4] + t*(
                                COEFFSJb3_cheby[ia][ip][5] + t*(
                                    COEFFSJb3_cheby[ia][ip][6] + t*(
                                        COEFFSJb3_cheby[ia][ip][7] + t*(
                                            COEFFSJb3_cheby[ia][ip][8] + t*(
                                                COEFFSJb3_cheby[ia][ip][9] + t*(
                                                    COEFFSJb3_cheby[ia][ip][10] + t*COEFFSJb3_cheby[ia][ip][11]
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
        return sign* ( -exp(lnPref+lneprL)*(1. + e2*e2*e2*( de*epr_cheby + COEFFSJb3_jh[ia][ip][3] )) );
    } else {
        int ia = a+2;
        int ip = absp-1;
        REAL8 eprL = 
            COEFFSJb3_jh_SP[ia][ip][0] + e2*(
                COEFFSJb3_jh_SP[ia][ip][1] + e2*(
                    COEFFSJb3_jh_SP[ia][ip][2] + e2*(
                        COEFFSJb3_jh_SP[ia][ip][3] + e2*(
                            COEFFSJb3_jh_SP[ia][ip][4] + e2*(
                                COEFFSJb3_jh_SP[ia][ip][5] + e2*(
                                    COEFFSJb3_jh_SP[ia][ip][6]
                                )
                            )
                        )
                    )
                )
            );
        if (absp>2)
            epr_cheby = 
                COEFFSJb3_cheby_SP[ia][ip][0] + t*(
                    COEFFSJb3_cheby_SP[ia][ip][1] + t*(
                        COEFFSJb3_cheby_SP[ia][ip][2] + t*(
                            COEFFSJb3_cheby_SP[ia][ip][3] + t*(
                                COEFFSJb3_cheby_SP[ia][ip][4] + t*(
                                    COEFFSJb3_cheby_SP[ia][ip][5] + t*(
                                        COEFFSJb3_cheby_SP[ia][ip][6] + t*(
                                            COEFFSJb3_cheby_SP[ia][ip][7] + t*(
                                                COEFFSJb3_cheby_SP[ia][ip][8]
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                );
        else
            epr_cheby = 
                COEFFSJb3_cheby_HSP[ia][ip][0] + t*(
                    COEFFSJb3_cheby_HSP[ia][ip][1] + t*(
                        COEFFSJb3_cheby_HSP[ia][ip][2] + t*(
                            COEFFSJb3_cheby_HSP[ia][ip][3] + t*(
                                COEFFSJb3_cheby_HSP[ia][ip][4] + t*(
                                    COEFFSJb3_cheby_HSP[ia][ip][5] + t*(
                                        COEFFSJb3_cheby_HSP[ia][ip][6] + t*(
                                            COEFFSJb3_cheby_HSP[ia][ip][7] + t*(
                                                COEFFSJb3_cheby_HSP[ia][ip][8] + t*(
                                                    COEFFSJb3_cheby_HSP[ia][ip][9] + t*(
                                                        COEFFSJb3_cheby_HSP[ia][ip][10] + t*(
                                                            COEFFSJb3_cheby_HSP[ia][ip][11] + t*(
                                                                COEFFSJb3_cheby_HSP[ia][ip][12] + t*(
                                                                    COEFFSJb3_cheby_HSP[ia][ip][13]
                                                                )
                                                            )
                                                        )
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
        return sign*( -exp(lnPref)*( eprL + e2*e2*e2*e2*e2*e2*e2*( de*epr_cheby + COEFFSJb3_jh_SP[ia][ip][7] ) ) );
    }
    // this should not happen
    return 0.0;
}


REAL8 dJpa1_Approx(INT p, INT a, REAL8 e)
{
    if(a!=1)
    {
        XPrintError("Error - %s: a = %d must be in [1,]\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }

    if(e < 0.0 || e >= 1.0)
    {
        XPrintError("Error - %s: e = %f must be in [0,1)\n", __func__, e);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 sign = p<0 ? -1.:1.;
    int absp = abs(p);
    if (absp > 200) {
        XPrintError("Error - %s: |p| = %d must > 5 and < 500\n", __func__, absp);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 e2 = e*e;
    REAL8 e14 = e2*e2*e2*e2*e2*e2*e2;
    REAL8 deltae2 = 1. - e2;
    REAL8 deltae = sqrt(deltae2);
    REAL8 lnde = log(deltae);
    REAL8 t = 2.*deltae - 1.;
    REAL8 lnPref = (absp-1)*log(e) - ( a>1 ? (2.*a-2.) : 1.0 )*lnde ;
    REAL8 lneprL = 
            COEFFSdJb1_a1_jhc[absp-1][0] + e2*(
                COEFFSdJb1_a1_jhc[absp-1][1] + e2*(
                    COEFFSdJb1_a1_jhc[absp-1][2] + e2*(
                        COEFFSdJb1_a1_jhc[absp-1][3] + e2*(
                            COEFFSdJb1_a1_jhc[absp-1][4] + e2*(
                                COEFFSdJb1_a1_jhc[absp-1][5] + e2*(
                                    COEFFSdJb1_a1_jhc[absp-1][6]
                                )
                            )
                        )
                    )
                )
            );
    REAL8 epr_cheby = 
            COEFFSdJb1_a1_cheby[absp-1][0] + t*(
                COEFFSdJb1_a1_cheby[absp-1][1] + t*(
                    COEFFSdJb1_a1_cheby[absp-1][2] + t*(
                        COEFFSdJb1_a1_cheby[absp-1][3] + t*(
                            COEFFSdJb1_a1_cheby[absp-1][4] + t*(
                                COEFFSdJb1_a1_cheby[absp-1][5] + t*(
                                    COEFFSdJb1_a1_cheby[absp-1][6] + t*(
                                        COEFFSdJb1_a1_cheby[absp-1][7] + t*(
                                            COEFFSdJb1_a1_cheby[absp-1][8] + t*(
                                                COEFFSdJb1_a1_cheby[absp-1][9] + t*(
                                                    COEFFSdJb1_a1_cheby[absp-1][10] + t*(
                                                        COEFFSdJb1_a1_cheby[absp-1][11] + t*(
                                                            COEFFSdJb1_a1_cheby[absp-1][12] + t*(
                                                                COEFFSdJb1_a1_cheby[absp-1][13]
                                                            )
                                                        )
                                                    )
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
    return -sign*exp(lnPref+lneprL)*(1. + e14*( deltae*epr_cheby + COEFFSdJb1_a1_jhc[absp-1][7] ));
}

REAL8 dJpa2_Approx(INT p, INT a, REAL8 e)
{
    if(a!=1)
    {
        XPrintError("Error - %s: a = %d must be in [1,]\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }

    if(e < 0.0 || e >= 1.0)
    {
        XPrintError("Error - %s: e = %f must be in [0,1)\n", __func__, e);
        X_ERROR_REAL8(X_EINVAL);
    }

    int absp = abs(p);
    if (absp > 200) {
        XPrintError("Error - %s: |p| = %d must > 5 and < 500\n", __func__, absp);
        X_ERROR_REAL8(X_EINVAL);
    }
    REAL8 e2 = e*e;
    REAL8 deltae2 = 1. - e2;
    REAL8 deltae = sqrt(deltae2);
    REAL8 lnde = log(deltae);
    REAL8 t = 2.*deltae - 1.;
    REAL8 lnPref = (absp-1)*log(e) - ( a>0 ? (2.*a+1.) : 1.0 )*lnde ;
    REAL8 epr_cheby;
    if (absp>10) {
        REAL8 lneprL = 
                COEFFSdJb2_a1_jhc[absp-10][0] + e2*(
                    COEFFSdJb2_a1_jhc[absp-10][1] + e2*(
                        COEFFSdJb2_a1_jhc[absp-10][2]
                    )
                );
        epr_cheby = 
                COEFFSdJb2_a1_cheby[absp-10][0] + t*(
                    COEFFSdJb2_a1_cheby[absp-10][1] + t*(
                        COEFFSdJb2_a1_cheby[absp-10][2] + t*(
                            COEFFSdJb2_a1_cheby[absp-10][3] + t*(
                                COEFFSdJb2_a1_cheby[absp-10][4] + t*(
                                    COEFFSdJb2_a1_cheby[absp-10][5] + t*(
                                        COEFFSdJb2_a1_cheby[absp-10][6] + t*(
                                            COEFFSdJb2_a1_cheby[absp-10][7] + t*(
                                                COEFFSdJb2_a1_cheby[absp-10][8] + t*(
                                                    COEFFSdJb2_a1_cheby[absp-10][9] + t*(
                                                        COEFFSdJb2_a1_cheby[absp-10][10] + t*(
                                                            COEFFSdJb2_a1_cheby[absp-10][11] + t*(
                                                                COEFFSdJb2_a1_cheby[absp-10][12] + t*(
                                                                    COEFFSdJb2_a1_cheby[absp-10][13] + t*COEFFSdJb2_a1_cheby[absp-10][14]
                                                                        )
                                                                    )
                                                                )
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
        return exp(lnPref+lneprL)*(1. + e2*e2*e2*( deltae*epr_cheby + COEFFSdJb2_a1_jhc[absp-10][3] ));
    }
    REAL8 eprL = 
            COEFFSdJb2_a1_jhc_SP[absp-1][0] + e2*(
                COEFFSdJb2_a1_jhc_SP[absp-1][1] + e2*(
                    COEFFSdJb2_a1_jhc_SP[absp-1][2] + e2*(
                        COEFFSdJb2_a1_jhc_SP[absp-1][3] + e2*(
                            COEFFSdJb2_a1_jhc_SP[absp-1][4] + e2*(
                                COEFFSdJb2_a1_jhc_SP[absp-1][5] + e2*(
                                    COEFFSdJb2_a1_jhc_SP[absp-1][6]
                                )
                            )
                        )
                    )
                )
            );
        epr_cheby = 
                COEFFSdJb2_a1_cheby_SP[absp-1][0] + t*(
                    COEFFSdJb2_a1_cheby_SP[absp-1][1] + t*(
                        COEFFSdJb2_a1_cheby_SP[absp-1][2] + t*(
                            COEFFSdJb2_a1_cheby_SP[absp-1][3] + t*(
                                COEFFSdJb2_a1_cheby_SP[absp-1][4] + t*(
                                    COEFFSdJb2_a1_cheby_SP[absp-1][5] + t*(
                                        COEFFSdJb2_a1_cheby_SP[absp-1][6] + t*(
                                            COEFFSdJb2_a1_cheby_SP[absp-1][7] + t*(
                                                COEFFSdJb2_a1_cheby_SP[absp-1][8]
                                        )
                                    )
                                )
                            )
                        )
                    )
                ));

    return exp(lnPref)*(eprL + e2*e2*e2*e2*e2*e2*e2*(deltae*epr_cheby + COEFFSdJb2_a1_jhc_SP[absp-1][7]));
}
