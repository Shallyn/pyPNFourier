/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/
#include "pnFourier_utils.h"
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <fenv.h>


REAL8 eval_beta(REAL8 e)
{
    return e < 1e-16 ? 0.5*e : (1.0 - sqrt(1.0 - e*e)) / e;
}


/*
 * BesselJ.c  —  REAL8-precision Bessel J_n(x) for integer n ≥ 0
 * https://github.com/machinekoder/cephes-math/tree/master
 *
 * Strategy
 *   1. m = n + floor( √(40 n) ) + 2，get ceil odd number；
 *   2. Miller iterate J_m, …, J_0；
 *   3. ∑_{k odd} J_k = J_0 / 2 normalize，
 *   4. return J_n。
 *
 */

/* ---------- tiny fall-back for J_0(x) when <math.h> lacks j0 ---------- */
static REAL8 j0_series(REAL8 x)
{
    REAL8 x2 = x * x, term = 1.0, sum = 1.0;
    for (INT k = 1; k < 40; ++k) {
        term *= -x2 / (4.0 * k * k);
        sum  += term;
        if (fabs(term) < 1e-16 * fabs(sum)) break;
    }
    return sum;
}
static REAL8 j0_asym(REAL8 x)
{
    const REAL8 inv_sqrt_pi = 0.5641895835477563;   /* 1 / √π */
    REAL8 ax = fabs(x), phi = ax - 0.25 * M_PI;
    return inv_sqrt_pi / sqrt(ax) * cos(phi);
}
static REAL8 j0_fallback(REAL8 x)
{
#if defined(__GLIBC__) || defined(__linux__)
    return j0(x);                 /* POSIX */
#else
    return (fabs(x) < 8.0) ? j0_series(x) : j0_asym(x);
#endif
}
/* --------------------------------------------------------------------- */

REAL8 BesselJ(INT n, REAL8 x)
{
    if (n < 0) { n = -n; if (n & 1) x = -x; }
    if (n == 0) return j0_fallback(x);
#if defined(__GLIBC__) || defined(__linux__)
    if (n == 1) return j1(x);
#endif
    if (x == 0.0) return 0.0;

    /* ---- ① init m ---- */
    INT m = n + (INT)(sqrt(40.0 * n) + 2.0);
    m |= 1;                             /* ceil odd number */

    REAL8 tox = 2.0 / x, bj = 1.0, bjm = 0.0, bjn = 0.0;

    /* ---- ② Miller Algorithm ---- */
    for (INT j = m; j > 0; --j) {
        REAL8 bjp = j * tox * bj - bjm;
        bjm = bj;  bj = bjp;

        if (j == n) bjn = bjm;          /* record \tilde J_n */

        if (fabs(bj) > 1e100) {        
            bj  *= 1e-100; bjm *= 1e-100; bjn *= 1e-100;
        }
    }

    /* ---- ③ Normalization ---- */
    REAL8 norm = j0_fallback(x) / bj;      /*  bj = \tilde J_0 */
    return bjn * norm;
}

void BesselJ_batch(INT n,
                   const REAL8 *restrict x,
                   REAL8       *restrict y,
                   size_t length)
{
#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (size_t i = 0; i < length; ++i)
        y[i] = BesselJ(n, x[i]);
}


/* ----------  log(Pochhammer(x,n))  = ln Γ(x+n) - ln Γ(x) -------- */
static REAL8 log_pochhammer(INT x, INT n)
{
    if (n == 0) return 0.0;
    if (n < 0) return -INFINITY;
    return (REAL8)(lgamma((REAL8)(x + n)) - lgamma((REAL8)x));
}

/* -------------------------------------------------------- */
/*                                                          */
/*                                                          */
/*                                                          */
/*                         Laplace                          */
/*                                                          */
/*                                                          */
/*                                                          */
/* -------------------------------------------------------- */
REAL8 laplace_nega(INT n, INT a, REAL8 beta)
{
    if (beta < 0 || beta >= BMAX || a > 0) {
        XPrintError("Error - %s: beta = %f must be in (0,%e) and a = %d must < 0\n", __func__,  beta, BMAX, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (n<0) n = -n;
    INT absa = -a;
    if (a==0) {
        if (n==0) return 1.0;
        else return 0.0;
    } else if (absa < n) return 0.0;
    REAL8 beta2 = beta*beta;
    REAL8 fac2 = beta2 / (1. + beta2)/(1. + beta2);
    REAL8 pref = pow(beta / (1. + beta2), n) * pow(1. + beta2, absa) * exp(lgamma((double)(absa+1))-lgamma((REAL8)(absa-n+1)) - lgamma((REAL8)(n+1)));
    INT mmax =  (absa - n)/2;
    REAL8 T0    = 1.0;                    
    REAL8 S     = T0;                               
    REAL8 tol = 1e-16;
    for (INT m=1; m<=mmax; m++) {
        REAL8 den1 = (REAL8) (m);
        REAL8 den2 = (REAL8) (n + m);
        REAL8 num = (REAL8) ((absa-n-2*m+1)*(absa-n-2*m+2));
        REAL8 ratio = fac2 * num / (den1 * den2);
        T0 *= ratio;
        // print_debug("[%d] T = %.16e\n", m, T0);
        S += T0;
        if (fabs(T0) < tol * fabs(S)) 
            break;
    }
    // print_debug("pref = %.16e\n", pref);
    // print_debug("S = %.16e\n", S);

    return (n%2 ? -pref*S : pref*S);
}

REAL8 laplace_na(INT n, INT a, REAL8 beta)
{
    if (beta < 0 || beta >= BMAX || a < 0) {
        XPrintError("Error - %s: beta = %f must be in (0,%e) and a = %d must > 0\n", __func__,  beta, BMAX, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a==0) return n==0 ? 1.0 : 0.0;
    if (n<0) n = -n;
    REAL8 beta2 = beta*beta;
    REAL8 ln_pref =
          log_pochhammer(a, n)                      /* ln (a)_n         */
        + n * log(beta)                             /* ln β^n           */
        + (1 - 2*a) * log(1.0 - beta2);             /* ln(1-β^2)^{1-2a} */
    REAL8 ln_T0 = -lgamma((REAL8)n + 1.0);          /* ln(1/n!) */
    // REAL8 T     = exp(ln_T0);   
    REAL8 lnpref = ln_pref+ln_T0;
    REAL8 T0    = 1.0;                    
    REAL8 S     = T0;                               
    REAL8 tol = 1e-16;
    INT mmax = n>=a ? a : a-n;
    for (INT m=0; m<mmax; m++) {
        REAL8 num   = (REAL8)(n + 1 - a + m);
        REAL8 num2  = (REAL8)(a - 1 - m);
        REAL8 den   = (REAL8)(m + 1) * (n + m + 1);
        REAL8 ratio = - beta2 * num * num2 / den;

        T0 *= ratio;
        S += T0;
        if (fabs(T0) < tol * fabs(S)) 
            break;
    }
    return exp(lnpref) * S;
}


/* ---------- safe logLaplace  ---------- */
REAL8 loglaplace_na(INT n, INT a, REAL8 beta)
{
    if (beta < 0 || beta >= BMAX || a < 0) {
        XPrintError("Error - %s: beta = %f must be in (0,%e) and a = %d must > 0\n", __func__,  beta, BMAX, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (n<0) n = -n;
    REAL8 beta2 = beta*beta;
    REAL8 ln_pref =
          log_pochhammer(a, n)                      /* ln (a)_n         */
        + n * log(beta)                             /* ln β^n           */
        + (1 - 2*a) * log(1.0 - beta2);             /* ln(1-β^2)^{1-2a} */
    REAL8 ln_T0 = -lgamma((REAL8)n + 1.0);          /* ln(1/n!) */
    // REAL8 T     = exp(ln_T0);   
    REAL8 lnpref = ln_pref+ln_T0;
    REAL8 T0    = 1.0;                    
    REAL8 S     = T0;                               
    REAL8 tol = 1e-16;
    INT mmax = n>=a ? a : a-n;
    for (INT m=0; m<mmax; m++) {
        REAL8 num   = (REAL8)(n + 1 - a + m);
        REAL8 num2  = (REAL8)(a - 1 - m);
        REAL8 den   = (REAL8)(m + 1) * (n + m + 1);
        REAL8 ratio = - beta2 * num * num2 / den;

        T0 *= ratio;
        S += T0;
        if (fabs(T0) < tol * fabs(S)) 
            break;
    }
    return lnpref + log(S);
}

LaplaceCache *CreateLaplaceCache(size_t nMax, INT a, REAL8 beta)
{
    LaplaceCache *ret = MYMalloc(sizeof(LaplaceCache));
    ret->a = a;
    ret->nMax = nMax;
    ret->beta = beta;
    ret->tbl = MYMalloc( (nMax+1) * sizeof(REAL8));
    if (!ret->tbl) {
        XPrintError("Error - %s: cannot allocate memory for Ltbl\n", __func__);
        X_ERROR_NULL(X_ENOMEM);
    }
    for (INT i = 0; i <= nMax; ++i)
        ret->tbl[i] = laplace_na(i, a, beta);
    return ret;
}

void DestroyLaplaceCache(LaplaceCache *cache)
{
    if (!cache)
        return;
    if (cache->tbl) {
        MYFree(cache->tbl);
        cache->tbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 get_Laplace_from_LaplaceCache(INT m, LaplaceCache *C)
{
    unsigned idx = (m < 0 ? -m : m);

    if (idx > C->nMax) {                      /* need more */
        unsigned old = C->nMax + 1;
        unsigned new = idx + 32;       

        C->tbl  = MYRealloc(C->tbl, (new+1)*sizeof *C->tbl);
        if (!C->tbl) { 
            XPrintError("Error - %s: cannot allocate memory for Ltbl\n", __func__);
            X_ERROR_REAL8(X_ENOMEM);
        }

        for (unsigned i = old; i <= new; ++i)
            C->tbl[i] = laplace_na(i, C->a, C->beta);

        C->nMax = new;
    }
    return C->tbl[idx];
}

// 2D general cache
static INT ExtendLaplaceCache2D(size_t need_n, size_t need_a, LaplaceCache2D *lc)
{
    size_t new_nMax = lc->nMax ? lc->nMax : 1, new_aMax = lc->aMax ? lc->aMax : 1;
    while (new_nMax < need_n) new_nMax <<= 1;
    while (new_aMax < need_a) new_aMax <<= 1;

    size_t newSize = (new_nMax + 1) * (new_aMax + 1);
    REAL8 *newBuf = MYMalloc(newSize * sizeof(REAL8));
    if (!newBuf) {
        X_ERROR(X_ENOMEM);
    }
    for (size_t i=0; i<newSize; i++) newBuf[i] = X_REAL8_FAIL_NAN;
    for (size_t ia = 0; ia <= lc->aMax; ia++)
        memcpy(&newBuf[ia*(new_nMax+1)],
               &(lc->tbl[ia*lc->stride]),
                (lc->stride)*sizeof(REAL8));
    MYFree(lc->tbl);
    lc->tbl = newBuf;
    lc->aMax = new_aMax;
    lc->nMax = new_nMax;
    lc->size = newSize;
    lc->stride = lc->nMax + 1;
    return X_SUCCESS;
}

LaplaceCache2D *CreateLaplaceCache2D(size_t nMax, size_t aMax, REAL8 beta)
{
    LaplaceCache2D *ret = MYMalloc(sizeof(LaplaceCache2D));
    ret->aMax = 0;
    ret->nMax = 0;
    ret->stride = ret->nMax+1;
    ret->size = (ret->nMax + 1)*(ret->aMax + 1);
    ret->beta = beta;
    ret->tbl = MYMalloc( sizeof(REAL8));
    ret->tbl[0] = 1.0;

    if (ExtendLaplaceCache2D(nMax, aMax, ret)!=X_SUCCESS) {
        X_ERROR_NULL(X_EFAILED);
    }
    return ret;
}


void DestroyLaplaceCache2D(LaplaceCache2D *cache)
{
    if (!cache)
        return;
    if (cache->tbl) {
        MYFree(cache->tbl);
        cache->tbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 get_Laplace_from_LaplaceCache2D(INT n, INT a, LaplaceCache2D *C)
{
    size_t absn = (n < 0 ? -n : n);
    if (a > C->aMax || absn > C->nMax) {
        if (ExtendLaplaceCache2D(absn, a, C) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    size_t idx = (size_t)a * C->stride + absn;
    REAL8 ret = C->tbl[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
        ret = laplace_na(absn, a, C->beta);
        C->tbl[idx] = ret;
    }
    return ret;
}

/* -------------------------------------------------------- */
/*                                                          */
/*                                                          */
/*                                                          */
/*                        DLaplace                          */
/*                                                          */
/*                                                          */
/*                                                          */
/* -------------------------------------------------------- */
/* ---------- Harmonic numbers with cache ---------- */
#define MAX(a,b) ((a)>(b)?(a):(b))
static REAL8 *HarmonicNumberCached = NULL;
static size_t H_size = 0;
static void ensure_H(size_t k) {
    if (k < H_size) return;
    size_t newN = MAX(2*H_size, k+1);
    HarmonicNumberCached = (REAL8*) MYRealloc(HarmonicNumberCached, newN * sizeof(REAL8));
    for (size_t i=H_size; i<newN; ++i) {
        if (i==0) HarmonicNumberCached[i] = 0.0;
        else      HarmonicNumberCached[i] = HarmonicNumberCached[i-1] + 1.0/(REAL8)i;
    }
    H_size = newN;
}
#undef MAX
static inline REAL8 HarmonicNumber(size_t k) { ensure_H(k); return HarmonicNumberCached[k]; }

static inline REAL8 dHarm(size_t n, size_t m) {
    return HarmonicNumber(n? n-1:0) - HarmonicNumber(m? m-1:0);
}

static inline REAL8 poch_modified_next(REAL8 P, REAL8 b, INT absn, INT m) {
     return P * (b + m) / ( (absn + m + 1.0) * (m+1.0) ); 
}

static REAL8 DPoch1Mam(INT a, INT m, REAL8 Poch1mam) {
    /* m<=a-1  or  Gamma branch */
    if (m <= a-1) {
        return Poch1mam * dHarm((size_t)a, (size_t)(a-m));
    } else {
        /* (-1)^a Γ(a) Γ(m+1-a) */
        REAL8 lg = lgamma(a) + lgamma(m + 1 - a);
        REAL8 val = exp(lg);
        return (a&1 ? -val :  val);
    }
}

static REAL8 DPoch1pnMam(INT a, INT n, INT m, REAL8 Poch1pn) {
    if (a <= n) {
        return Poch1pn * dHarm((size_t)(n+1-a), (size_t)(n+1+m-a));
    } else {
        if (m <= a - n - 1) {
            return Poch1pn * dHarm((size_t)(a-n), (size_t)(a-n-m));
        } else {
            REAL8 lg = lgamma(a - n) + lgamma(m + n + 1 - a);
            REAL8 val = exp(lg);
            return (((a-n)&1) ? -val : val);
        }
    }
}

// can be cached
// DPoch1MamPoch1pnMam[a, |n|, m] / (|n| + m)! / m!
// = ((1-a)_m * d_a(1+n-a)_m + (1+n-a)_m * d_a(1-a)_m) / ((|n| + m)! * m!)
// = 
static REAL8 DH2F1SeriesP1(INT absn, INT a, REAL8 beta2)
{
    REAL8 beta2Pow = beta2;
    INT  m_cut = (absn < a) ? a-absn-1 : -1;
    if (m_cut == 0) return 0;
    REAL8 poch1MapnOverDen = 1.0;
    REAL8 poc1Ma = 1.0;
    REAL8 sum = 0.0;
    REAL8 tol = 1e-16;
    // print_debug("absn = %d, a = %d, beta2 = %e\n", absn, a, beta2);
    for (int m=1 ; m<a ; m++) {
        poch1MapnOverDen *= (REAL8)(absn-a+m) / (REAL8)((absn + m) * m);
        poc1Ma *= m-a;
        sum += poc1Ma * poch1MapnOverDen * beta2Pow * dHarm(a, a-m);

        // preparation
        beta2Pow *= beta2;
        // print_debug("[%d]: poc1Ma = %e, poch1MapnOverDen = %e, dHarm = %e\n", m, poc1Ma, poch1MapnOverDen, dHarm(a, a-m));
        // print_debug("[%d]: res = %e\n", m, poc1Ma * poch1MapnOverDen * dHarm(a, a-m));
        if (m_cut != -1 && m >= m_cut) return sum;
    }
    REAL8 apref = ( (a & 1) ? -1.0 : 1.0) * exp(lgamma(a));
    REAL8 fac2 = 1.0;
    poch1MapnOverDen *= (REAL8)absn/( (REAL8)a*(absn+a));
    sum += poch1MapnOverDen * apref *beta2Pow;
    // print_debug("[%d]: res = %e, apref = %e\n", a, poch1MapnOverDen * apref, apref);
    beta2Pow *= beta2;
    for (int m=1;; m++) {
        poch1MapnOverDen *= (REAL8)(absn+m)/ ((REAL8)(absn + m + a));
        fac2 *= (REAL8)m / ((REAL8)(m+a));
        REAL8 term = poch1MapnOverDen*fac2*apref*beta2Pow;
        sum += term;
        beta2Pow *= beta2;
        // print_debug("[%d]: res = %e\n", a+m, poch1MapnOverDen*fac2*apref);
        if (fabs(term) < tol*fabs(sum)) break;
    }
    return sum;
}

static REAL8 DH2F1SeriesP2(INT absn, INT a, REAL8 beta2)
{
    REAL8 beta2Pow = beta2;
    if (a<=absn) {
        REAL8 coeff = 1.0;
        REAL8 sum = 0.0;
        for (int m=1; m<a; m++) {
            coeff = coeff * ((REAL8) (m-a) * (absn + m - a)) / ((REAL8)(absn+m)*m );
            sum = sum + coeff * beta2Pow * dHarm(absn-a+1, absn+1+m-a);
            beta2Pow *= beta2;
        }
        return sum;
    }
    REAL8 poch1MaOver = 1.0;
    REAL8 poch1Mapn = 1.0;
    REAL8 sum = 0.0;
    REAL8 tol = 1e-16;
    // print_debug("absn = %d, a = %d, beta2 = %e\n", absn, a, beta2);
    for (int m=1 ; m<a-absn ; m++) {
        poch1MaOver *= (REAL8)(m-a) / (REAL8)((absn + m) * m);
        poch1Mapn *= absn+m-a;
        sum += poch1Mapn * poch1MaOver * beta2Pow * dHarm(a-absn, a-absn-m);
        // preparation
        beta2Pow *= beta2;
        // print_debug("[%d]: poc1Ma = %e, poch1MapnOverDen = %e, dHarm = %e\n", m, poc1Ma, poch1MapnOverDen, dHarm(a, a-m));
        // print_debug("[%d]: res = %e\n", m, poc1Ma * poch1MapnOverDen * dHarm(a, a-m));
    }
    INT da = a - absn;
    REAL8 apref = ( ( da & 1) ? -1.0 : 1.0) * exp(lgamma(da));
    REAL8 fac2 = 1.0;
    poch1MaOver *= (REAL8)(-absn)/( (REAL8)a*da);
    sum += poch1MaOver * apref * beta2Pow;
    // print_debug("[%d]: res = %e, apref = %e\n", a, poch1MapnOverDen * apref, apref);
    beta2Pow *= beta2;
    for (int m=1; m < absn ; m++) {
        poch1MaOver *= (REAL8)(m-absn)/ ((REAL8)(m + da));
        fac2 *= (REAL8) m / ((REAL8)(m+a));
        sum += poch1MaOver*fac2*apref*beta2Pow;
        beta2Pow *= beta2;
    }
    return sum;
}

static REAL8 H2F1Series(INT a, INT absn, REAL8 beta2)
{
    // REAL8 pref = exp(lgamma((REAL8)a));
    // REAL8 T     = exp(ln_T0);   
    REAL8 T0    = 1.0;                    
    REAL8 S     = T0;                               
    REAL8 tol = 1e-16;
    INT mmax = absn>=a ? a : a-absn;
    for (INT m=1; m<mmax; m++) {
        REAL8 num   = (REAL8)(absn - a + m)* (a-m);
        REAL8 den   = (REAL8)(m) * (absn + m);
        REAL8 ratio = - beta2*num / den;

        T0 *= ratio;
        S += T0;
        // print_debug("[%d]: ratio = %e\n", m, T0);
        // if (fabs(T0) < tol * fabs(S)) 
        //     break;
    }
    return S;
}

/* Dlaplace[n, a] = L[n, a]*ln[1 + beta^2] + d/da L[n, a] */
REAL8 Dlaplace_na(INT n, INT a, REAL8 beta)
{
    if (beta < 0 || beta >= BMAX || a < 0) {
        XPrintError("Error - %s: beta = %f must be in (0,%e) and a = %d must > 0\n", __func__,  beta, BMAX, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a==0) {
        return n==0 ? 0.0 : pow(beta, abs(n)) / abs(n);
    }
    if (n<0) n = -n;
    REAL8 beta2 = beta*beta;
    REAL8 lnpref = n*log(beta) + (1.-2.*a)*log(1-beta2) - lgamma(n+1) + log_pochhammer(a, n);
    REAL8 term1 = H2F1Series(a, n, beta2) * (2*log(1-beta2) - dHarm(a+n, a));
    REAL8 term2 = DH2F1SeriesP1(n, a, beta2) + DH2F1SeriesP2(n, a, beta2);
    // print_debug("pref = %e, term1 = %e, term2 = %e\n", exp(lnpref), term1, term2);
    return -exp(lnpref)*(term1-term2);
    // return 0.0;
}

DLaplaceCache *CreateDLaplaceCache(size_t nMax, INT a, REAL8 beta)
{
    DLaplaceCache *ret = MYMalloc(sizeof(DLaplaceCache));
    ret->a = a;
    ret->nMax = nMax;
    ret->beta = beta;
    ret->tbl = MYMalloc( (nMax+1) * sizeof(REAL8));
    if (!ret->tbl) {
        XPrintError("Error - %s: cannot allocate memory for DLtbl\n", __func__);
        X_ERROR_NULL(X_ENOMEM);
    }
    for (INT i = 0; i <= nMax; ++i)
        ret->tbl[i] = Dlaplace_na(i, a, beta);
    return ret;
}

void DestroyDLaplaceCache(DLaplaceCache *cache)
{
    if (!cache)
        return;
    if (cache->tbl) {
        MYFree(cache->tbl);
        cache->tbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 get_DLaplace_from_DLaplaceCache(INT m, DLaplaceCache *C)
{
    unsigned idx = (m < 0 ? -m : m);

    if (idx > C->nMax) {                      /* need more */
        unsigned old = C->nMax + 1;
        unsigned new = idx + 32;       

        C->tbl  = MYRealloc(C->tbl, (new+1)*sizeof *C->tbl);
        if (!C->tbl) { 
            XPrintError("Error - %s: cannot allocate memory for DLtbl\n", __func__);
            X_ERROR_REAL8(X_ENOMEM);
        }

        for (unsigned i = old; i <= new; ++i)
            C->tbl[i] = Dlaplace_na(i, C->a, C->beta);

        C->nMax = new;
    }
    return C->tbl[idx];
}



// 2D general cache
static INT ExtendDLaplaceCache2D(size_t need_n, size_t need_a, DLaplaceCache2D *lc)
{
    size_t new_nMax = lc->nMax ? lc->nMax : 1, new_aMax = lc->aMax ? lc->aMax : 1;
    while (new_nMax < need_n) new_nMax <<= 1;
    while (new_aMax < need_a) new_aMax <<= 1;

    size_t newSize = (new_nMax + 1) * (new_aMax + 1);
    REAL8 *newBuf = MYMalloc(newSize * sizeof(REAL8));
    if (!newBuf) {
        X_ERROR(X_ENOMEM);
    }
    for (size_t i=0; i<newSize; i++) newBuf[i] = X_REAL8_FAIL_NAN;
    for (size_t ia = 0; ia <= lc->aMax; ia++)
        memcpy(&newBuf[ia*(new_nMax+1)],
               &(lc->tbl[ia*lc->stride]),
                (lc->stride)*sizeof(REAL8));
    MYFree(lc->tbl);
    lc->tbl = newBuf;
    lc->aMax = new_aMax;
    lc->nMax = new_nMax;
    lc->size = newSize;
    lc->stride = lc->nMax + 1;
    return X_SUCCESS;
}

DLaplaceCache2D *CreateDLaplaceCache2D(size_t nMax, size_t aMax, REAL8 beta)
{
    DLaplaceCache2D *ret = MYMalloc(sizeof(DLaplaceCache2D));
    ret->aMax = 0;
    ret->nMax = 0;
    ret->stride = ret->nMax+1;
    ret->size = (ret->nMax + 1)*(ret->aMax + 1);
    ret->beta = beta;
    ret->tbl = MYMalloc( sizeof(REAL8));
    ret->tbl[0] = 0.0; // DLaplace[0, 0] = 0

    if (ExtendDLaplaceCache2D(nMax, aMax, ret)!=X_SUCCESS) {
        X_ERROR_NULL(X_EFAILED);
    }
    return ret;
}


void DestroyDLaplaceCache2D(DLaplaceCache2D *cache)
{
    if (!cache)
        return;
    if (cache->tbl) {
        MYFree(cache->tbl);
        cache->tbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 get_DLaplace_from_DLaplaceCache2D(INT n, INT a, DLaplaceCache2D *C)
{
    size_t absn = (n < 0 ? -n : n);
    if (a > C->aMax || absn > C->nMax) {
        if (ExtendDLaplaceCache2D(absn, a, C) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    size_t idx = (size_t)a * C->stride + absn;
    REAL8 ret = C->tbl[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
        ret = Dlaplace_na(absn, a, C->beta);
        C->tbl[idx] = ret;
    }
    return ret;
}

/* -------------------------------------------------------- */
/*                                                          */
/*                                                          */
/*                                                          */
/*                     Bessel Cache                         */
/*                                                          */
/*                                                          */
/*                                                          */
/* -------------------------------------------------------- */
BesselJCache *CreateBesselJCache(size_t kMax, REAL8 z)
{
    BesselJCache *ret = MYMalloc(sizeof(BesselJCache));
    ret->kMax = kMax;
    ret->z = z;
    ret->Jbl = MYMalloc( (kMax+1) * sizeof(REAL8));
    if (!ret->Jbl) {
        XPrintError("Error - %s: cannot allocate memory for Jtbl\n", __func__);
        X_ERROR_NULL(X_ENOMEM);
    }
    for (INT i = 0; i <= kMax; ++i)
        ret->Jbl[i] = jn(i, z);
    return ret;
}

void DestroyBesselJCache(BesselJCache *cache)
{
    if (!cache)
        return;
    if (cache->Jbl) {
        MYFree(cache->Jbl);
        cache->Jbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}
// Only Positive m
REAL8 get_BesselJ_from_BesselJCache(INT m, BesselJCache *C)
{
    INT idx = (m < 0 ? -m : m);
    if (idx > C->kMax) {                      /* need more */
        INT old = C->kMax + 1;
        INT new = idx + 32;       

        C->Jbl  = MYRealloc(C->Jbl, (new+1)*sizeof *C->Jbl);
        if (!C->Jbl) { 
            XPrintError("Error - %s: cannot allocate memory for Jtbl\n", __func__);
            X_ERROR_REAL8(X_ENOMEM);
        }

        for (INT i = old; i <= new; ++i) {
            C->Jbl[i] = jn(i, C->z);
            // print_debug("extension: jn(%d, %.5f) = %.16e\n", i, C->z, C->Jbl[i]);
        }

        C->kMax = new;
    }
    return ( (m<0 && idx&1) ? -1.0 : 1.0)*C->Jbl[idx];
    // REAL8 pref = idx & 1 ? (m<0 ? -1.0 : 1.0) : 1.0;
    // return pref*ret;
}


// 2D general cach
static INT ExtendBesselJCache2D(size_t need_k, size_t need_q, BesselJCache2D *jc)
{
    size_t new_abskMax = jc->abskMax ? jc->abskMax : 1, new_absqMax = jc->absqMax ? jc->absqMax : 1;
    while (new_abskMax < need_k) new_abskMax <<= 1;
    while (new_absqMax < need_q) new_absqMax <<= 1;

    size_t newSize = (new_abskMax + 1) * (new_absqMax + 1);
    REAL8 *newBuf = MYMalloc(newSize * sizeof(REAL8));
    if (!newBuf) {
        X_ERROR(X_ENOMEM);
    }
    for (size_t i=0; i<newSize; i++) newBuf[i] = X_REAL8_FAIL_NAN;
    for (size_t ik = 0; ik <= jc->abskMax; ik++)
        memcpy(&newBuf[ik*(new_absqMax+1)],
               &(jc->tbl[ik*jc->stride]),
                (jc->stride)*sizeof(REAL8));
    MYFree(jc->tbl);
    jc->tbl = newBuf;
    jc->abskMax = new_abskMax;
    jc->absqMax = new_absqMax;
    jc->size = newSize;
    jc->stride = jc->absqMax + 1;
    return X_SUCCESS;
}

BesselJCache2D *CreateBesselJCache2D(size_t abskMax, size_t absqMax, REAL8 e)
{
    BesselJCache2D *ret = MYMalloc(sizeof(BesselJCache2D));
    ret->abskMax = 0;
    ret->absqMax = 0;
    ret->stride = ret->absqMax + 1;
    ret->size = (ret->abskMax + 1)*(ret->absqMax + 1);
    ret->e = e;
    ret->tbl = MYMalloc( sizeof(REAL8));
    ret->tbl[0] = 1.0;

    if (ExtendBesselJCache2D(abskMax, absqMax, ret)!=X_SUCCESS) {
        X_ERROR_NULL(X_EFAILED);
    }
    return ret;
}


void DestroyBesselJCache2D(BesselJCache2D *cache)
{
    if (!cache)
        return;
    if (cache->tbl) {
        MYFree(cache->tbl);
        cache->tbl = NULL;
    }
    MYFree(cache);
    cache = NULL;
    return;
}

REAL8 get_BesselJ_from_BesselJCache2D(INT k, INT q, BesselJCache2D *C)
{
    // size_t absk = (k < 0 ? -k : k);
    size_t absk = (k<0 ? -k : k);
    size_t absq = (q<0 ? -q : q);
    if (absk > C->abskMax || absq > C->absqMax) {
        if (ExtendBesselJCache2D(absk, absq, C) != X_SUCCESS) {
            X_ERROR_REAL8(X_ENOMEM);
        }
    }
    REAL8 pref = absk & 1 ? ((k<0 ? -1.0 : 1.0) * (q<0 ? -1.0 : 1.0)) : 1.0;
    size_t idx = absk * C->stride + absq;
    REAL8 ret = C->tbl[idx];
    if (X_IS_REAL8_FAIL_NAN(ret)) {
        ret = jn(absk, C->e*absq);
        C->tbl[idx] = ret;
    }
    return pref*ret;
}
