/**
* Writer: Xiaolin.liu
* shallyn.liu@foxmail.com
**/
#include "pnFourier_Jpqa0.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <fftw3.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_sf_legendre.h> 
#include "core_Jpqa0_Approx.h"
#include "../libbasic/utils_GSL.h"
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

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

/* ------------- Utils ------------- */
static inline REAL8 safe_pow(INT m, REAL8 x)
{
    REAL8 delta = 1.0 - x;                     
    if (delta > 1e-8)       
        return pow(x, m);
    return exp(m * log1p(-delta)); 
}

/* ---------------------------------------------------------------- */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                            J^0_pq1                               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* ---------------------------------------------------------------- */

/* ---------- utility: choose a safe truncation length N --------- */
static INT choose_N(INT p_max, INT q, REAL8 e, REAL8 beta, REAL8 tol)
{
    REAL8 z   = fabs(q * e);
    INT N = p_max;
    REAL8 term = 1.0;          /* (z/2)^k / k!  */
    INT k0 = (INT)(ceil(log10(tol) / log10(beta)));
    INT kmax = k0 + 100000;
    for (INT k = 1; k < kmax; ++k) {
        term *= z / (2.0 * k);
        if (term < tol && k > k0) { N = k; break; }
    }
    /* keep it odd to align with FFT length 2N+1 */
    if ((N & 1) == 0) ++N;
    return N;
}

/* -------------- FFT method  -------------*/
INT Jpq10_fft(INT p_min, INT p_max, INT q, REAL8 e,
                   REAL8 tol, REAL8 *out)
{
    if (e <= 0.0 || e >= EMAX) 
    {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR(X_EINVAL);
    }

    INT P = p_max - p_min + 1;
    REAL8 beta = eval_beta(e);
    INT N = choose_N(p_max > -p_min ? p_max : -p_min, q, e, beta, tol);
    INT L = 1;
    while(L<4*N+1) L<<=1;

    /* --- allocate FFT arrays --- */
    fftw_complex *A = fftw_malloc(sizeof(fftw_complex) * L); /* J_k */
    fftw_complex *B = fftw_malloc(sizeof(fftw_complex) * L); /* rho^|n| */
    fftw_complex *C = fftw_malloc(sizeof(fftw_complex) * L); /* result */
    for (INT i = 0; i < L; ++i) {
        SETCOMP(A[i],0,0);
        SETCOMP(B[i],0,0);
    }

    /* --- fill A: Bessel J_k(qe) for k = -N..N --- */
    REAL8 z = q * e;
    for (INT k = -N; k <= N; ++k) SETCOMP(A[(k + N)], jn(k, z), 0.0);

    /* --- fill B: rho^{|n|} / sqrt(1-e^2) --- */
    REAL8 norm = 1.0 / sqrt(1.0 - e*e);
    for (INT n = -N; n <= N; ++n) SETCOMP(B[(n + N)], norm * pow(beta, abs(n)), 0.0);

    /* --- FFT convolution C = ifft( fft(A) * fft(B) ) --- */
    fftw_plan fwdA = fftw_plan_dft_1d(L, A, A, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan fwdB = fftw_plan_dft_1d(L, B, B, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan invC = fftw_plan_dft_1d(L, C, C, FFTW_BACKWARD, FFTW_ESTIMATE);

    fftw_execute(fwdA);
    fftw_execute(fwdB);
    for (INT i = 0; i < L; ++i) {               
        REAL8 xr = GETREAL(A[i])*GETREAL(B[i]) - GETIMAG(A[i])*GETIMAG(B[i]);
        REAL8 xi = GETREAL(A[i])*GETIMAG(B[i]) + GETIMAG(A[i])*GETREAL(B[i]);
        SETCOMP(C[i], xr, xi);
    }
    fftw_execute(invC);

    /* --- extract required p and apply overall factor 2π / L --- */
    const REAL8 factor = 1.0 / L;
    for (INT p = p_min; p <= p_max; ++p) {
        INT idx = (p + 2*N);                       /* circular shift */
        out[p - p_min] = GETREAL(C[idx]) * factor;
    }

    fftw_destroy_plan(fwdA);
    fftw_destroy_plan(fwdB);
    fftw_destroy_plan(invC);
    fftw_free(A);  fftw_free(B);  fftw_free(C);
    return X_SUCCESS;
}

/* ------------ Series Method ------------*/
REAL8 J_pq10_series(INT p, INT q, REAL8 e, REAL8 atol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (p < 0) { 
        p = -p;
        q = -q;
    }
    // if (p == 0) return j0_fallback(x);
    // if (q == 0) return laplace...
    // REAL8 beta = (1.0 - sqrt(1.0 - e*e)) / e;
    REAL8 beta = eval_beta(e);

    REAL8 deltabeta = 1.0 - beta;
    INT N = choose_N(p, q, e, beta, atol);
    const REAL8 norm = 1.0 / sqrt(1.0 - e*e);

    REAL8 sum = 0.0;
    for (int k = -N; k <= N; ++k) {
        REAL8 Jk = jn(k, q*e);               /* POSIX libm */
        if (deltabeta > 1e-8)
            sum += Jk * pow(beta, abs(p - k));
        else
            sum += Jk * exp( abs(p - k) * log1p(-deltabeta) );
    }
    return norm * sum;
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

static void c_pow_eq(fftw_complex *z, int a)
{
    if (a == 0) { SETCOMP((*z), 1.0, 0.0); return; }
    if (a == 1) return;

    REAL8 re = GETREAL((*z)), im = GETIMAG((*z));
    REAL8 mag = hypot(re, im);
    REAL8 arg = atan2(im, re);
    REAL8 ln  = log(mag);
    REAL8 new_mag = exp(a * ln);
    REAL8 new_arg = a * arg;
    SETCOMP((*z), new_mag * cos(new_arg), new_mag * sin(new_arg));
}

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

static void build_B_coeff_fft(fftw_complex *B, INT N, INT a,
                              REAL8 beta, REAL8 e, INT L,
                              fftw_plan fwd, fftw_plan inv)
{
    const REAL8 norm1 = 1.0 / sqrt(1.0 - e*e);   /* (1−e²)^{−½} */

    /* Rₙ (a = 1 kernel) */
    for (INT i = 0; i < L; ++i) SETCOMP(B[i], 0.0, 0.0);
    for (INT n = -N; n <= N; ++n) {
        INT idx = ((n % L) + L) % L;
        SETCOMP(B[idx], norm1 * pow(beta, fabs((double)n)), 0.0);
    }

    /* R̂ = FFT(R) */
    fftw_execute(fwd);

    /* B̂ = R̂^{a}  (in‑place) */
    for (INT i = 0; i < L; ++i) c_pow_eq(&B[i], a);

    /* B  = ifft(B̂)   – NO extra scaling! */
    fftw_execute(inv);
    for (INT i = 0; i < L; ++i) {
        REAL8 re = GETREAL(B[i]) / (REAL8)L;   /* FFTW backward is un‑scaled */
        SETCOMP(B[i], re, 0.0);
    }
}

/* -------------- FFT method  -------------*/
INT Jpqa_fft(INT p_min, INT p_max, INT q, INT a, REAL8 e,
                 REAL8 tol, REAL8 *out)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR(X_EINVAL);
    }

    /*  a = 0 is analytic: (1/2π)∫ e^{i(px − q e sin x)} dx = J_p(q e)       */
    if (a == 0) {
        for (INT p = p_min; p <= p_max; ++p)
            out[p - p_min] = jn(p, q * e);
        return X_SUCCESS;
    }

    INT p_max_abs = (p_max > -p_min) ? p_max : -p_min;
    REAL8 beta = eval_beta(e);
    INT N = choose_N_a(p_max_abs, q, e, beta, tol, a);

    INT L = 1; while (L < 2 * a * N + 1) L <<= 1;

    fftw_complex *A = fftw_malloc(sizeof(fftw_complex) * L);
    fftw_complex *B = fftw_malloc(sizeof(fftw_complex) * L);
    fftw_complex *C = fftw_malloc(sizeof(fftw_complex) * L);

    fftw_plan fwdA = fftw_plan_dft_1d(L, A, A, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan fwdB = fftw_plan_dft_1d(L, B, B, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan invC = fftw_plan_dft_1d(L, C, C, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_plan fwdR = fftw_plan_dft_1d(L, B, B, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan invR = fftw_plan_dft_1d(L, B, B, FFTW_BACKWARD, FFTW_ESTIMATE);

    /* Bₙ  */
    build_B_coeff_fft(B, N, a, beta, e, L, fwdR, invR);

    /* A_k */
    for (INT i = 0; i < L; ++i) SETCOMP(A[i], 0.0, 0.0);
    REAL8 z = q * e;
    for (INT k = -N; k <= N; ++k) {
        INT idx = ((k % L) + L) % L;
        REAL8 Jk = (k >= 0) ? jn(k, z) : ((k & 1) ? -jn(-k, z) : jn(-k, z));
        SETCOMP(A[idx], Jk, 0.0);
    }

    /* convolution  */
    fftw_execute(fwdA);
    fftw_execute(fwdB);
    for (INT i = 0; i < L; ++i) {
        REAL8 xr = GETREAL(A[i])*GETREAL(B[i]) - GETIMAG(A[i])*GETIMAG(B[i]);
        REAL8 xi = GETREAL(A[i])*GETIMAG(B[i]) + GETIMAG(A[i])*GETREAL(B[i]);
        SETCOMP(C[i], xr, xi);
    }
    fftw_execute(invC);

    const REAL8 factor = 1.0 / L;            /* undo the single inverse */
    for (INT p = p_min; p <= p_max; ++p) {
        INT idx = ((p % L) + L) % L;
        out[p - p_min] = GETREAL(C[idx]) * factor;   /* already 1/(2π) */
    }

    fftw_destroy_plan(fwdA); fftw_destroy_plan(fwdB);
    fftw_destroy_plan(invC); fftw_destroy_plan(fwdR); fftw_destroy_plan(invR);
    fftw_free(A); fftw_free(B); fftw_free(C);
    return X_SUCCESS;
}

/*  Safe Bessel array via Miller downward (stable when z ≪ k)  */
static void bessel_fill(INT kmin, INT kmax, REAL8 z, REAL8 *J)
{
    /* Direct libm evaluation – O(N) but <300 calls in practice.  */
    for (INT k = kmin; k <= kmax; ++k) {
        REAL8 v;
        if (k >= 0) {
            v = jn(k, z);
        } else {
            INT kk = -k;
            v = jn(kk, z);
            if (kk & 1) v = -v;      /* J_{-n}(z)=(-1)^n J_n(z) */
        }
        J[k - kmin] = v;
    }
}

/* ------------ Series Method ------------*/
REAL8 J_pqa0_series(INT p, INT q, INT a, REAL8 e, REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    // if (a==0) return jn(p,q*e);  
    if(a==0) return jn(p,q*e);
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT N = choose_N_a(p_abs, q, e, beta, atol, a);

    INT kmin = -N, kmax = N;
    INT M = kmax - kmin + 1;
    INT kMin = p_abs < 5 ? 5 : p_abs;
    // INT kMin = 10;
    /* Bessel array J_k(z) (both signs)                                */
    // REAL8 *Jk = MYMalloc(M * sizeof(REAL8));
    // bessel_fill(kmin, kmax, z, Jk);
    BesselJCache *jc = CreateBesselJCache(kmax, z);
    size_t nMax = abs(kmax) + abs(p); // just guess
    LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);

    /* series summation                                                */
    INT ikcum = 0;
    INT ikcumMax = 2;
    REAL8 sum = get_BesselJ_from_BesselJCache(0, jc) * get_Laplace_from_LaplaceCache(p_abs, lc);
    REAL8 sumP = 0.0;
    for (INT k = 1 ;  ; ++k) {
        REAL8 bessP = get_BesselJ_from_BesselJCache(k, jc);
        // REAL8 bessM = get_BesselJ_from_BesselJCache(-k, jc);
        INT nP = abs(k + p);
        // INT nM = abs(-k+p);             /* |k+p| */
        REAL8 LP = get_Laplace_from_LaplaceCache(nP, lc);
        // REAL8 LM = get_Laplace_from_LaplaceCache(nM, lc);

        REAL8 term = (k&1 ? -1.0 : 1.0) * (bessP * LP);
        sumP += term;
        // print_debug("[%d]term/sumP = %.16e/%.16e\n", 
        //         k, term, sumP);
        if (fabs(term) <= rtol * fabs(sumP)) {   /* early exit */
            ikcum ++;
            if (k > kMin && ikcum > ikcumMax) break;            /* tail symmetric */
            // else ikcum = 0;
        } else 
            ikcum = 0;
    }
    REAL8 sumM = 0.0;
    for (INT k = 1 ;  ; ++k) {
        // REAL8 bessP = get_BesselJ_from_BesselJCache(k, jc);
        REAL8 bessM = get_BesselJ_from_BesselJCache(-k, jc);
        // INT nP = abs(k + p);
        INT nM = abs(-k+p);             /* |k+p| */
        // REAL8 LP = get_Laplace_from_LaplaceCache(nP, lc);
        REAL8 LM = get_Laplace_from_LaplaceCache(nM, lc);

        REAL8 term = (k&1 ? -1.0 : 1.0) * (bessM*LM);
        // print_debug("[%d]term/sumM = %.16e/%.16e\n", 
        //         k, term, sumM);
        sumM += term;
        if (fabs(term) <= rtol * fabs(sumM) ) {   /* early exit */
            ikcum++;
            if (k > kMin && ikcum > ikcumMax) break;            /* tail symmetric */
        } else 
            ikcum = 0;
    }

    // MYFree(Jk);
    STRUCTFREE(jc, BesselJCache);
    STRUCTFREE(lc, LaplaceCache);
    // DestroyLaplaceCache(lc);
    REAL8 factor = pow(1.0 + beta2, a);
    return factor * (sum + sumP + sumM);   /* already includes 1/(2π) via Laplace */
}

/*
 * Uniform absolute bound obtained by shifting the integration contour to
 * chi = x + i*s:
 *
 * |J_pqa0| <= exp(-|p|s + |q|e sinh(s)) / (1 - e cosh(s))^a,
 *             0 < s < acosh(1/e).
 *
 * The logarithm of the bound is convex.  Its minimum is found by bisection
 * on the derivative.  Returning zero is safe under the usual
 * atol + rtol*|value| error convention when bound <= atol/(1-rtol).
 */
static INT J_pqa0_below_absolute_tolerance(INT p, INT q, INT a, REAL8 e,
                                            REAL8 atol, REAL8 rtol)
{
    if (!(atol > 0.0) || !(rtol >= 0.0) || rtol >= 1.0 || a <= 0)
        return 0;

    REAL8 p_abs = fabs((REAL8)p);
    REAL8 q_abs = fabs((REAL8)q);

    /* No useful interior minimum if the derivative is non-negative at 0. */
    if (p_abs <= q_abs * e)
        return 0;

    REAL8 s_max = acosh(1.0 / e);
    REAL8 lo = 0.0;
    REAL8 hi = nextafter(s_max, 0.0);
    if (!(hi > 0.0))
        return 0;

    for (INT i = 0; i < 80; ++i) {
        REAL8 s = 0.5 * (lo + hi);
        REAL8 sh = sinh(s);
        REAL8 ch = cosh(s);
        REAL8 den = 1.0 - e * ch;
        REAL8 deriv = -p_abs + q_abs * e * ch + a * e * sh / den;
        if (deriv < 0.0)
            lo = s;
        else
            hi = s;
    }

    REAL8 s = 0.5 * (lo + hi);
    REAL8 den = 1.0 - e * cosh(s);
    if (!(den > 0.0))
        return 0;

    REAL8 log_bound = -p_abs * s + q_abs * e * sinh(s) - a * log(den);
    REAL8 log_allowed = log(atol) - log1p(-rtol);
    return log_bound <= log_allowed;
}

/*
 * Choose N from a p-independent absolute bound on the omitted Bessel tail.
 * Since |L_n^(a)| <= (1-beta)^(-2a),
 *
 * tail <= 2 (1+beta^2)^a (1-beta)^(-2a) sum_{k>N}|J_k(z)|.
 *
 * For k >= 0 we use
 * |J_k(z)| <= (|z|/2)^k/k! * exp(z^2/(4(k+1))).
 * If no positive absolute tolerance is supplied, retain the old conservative
 * p-dependent cutoff.
 */
static INT choose_N_a_optimized(INT p_abs, INT q, REAL8 e, REAL8 beta,
                                REAL8 atol, REAL8 rtol, INT a)
{
    REAL8 z = fabs((REAL8)q * e);
    if (!(atol > 0.0) || !isfinite(atol)) {
        REAL8 tol = (rtol > 0.0 && rtol < 1.0) ? rtol : DBL_EPSILON;
        return choose_N_a(p_abs, q, e, beta, tol, a);
    }
    if (z == 0.0)
        return 0;
    if (z > (REAL8)(INT_MAX - 2))
        return choose_N_a(p_abs, q, e, beta, atol, a);

    const REAL8 log_two = 0.693147180559945309417232121458176568;
    INT N = (INT)ceil(0.5 * z);
    REAL8 log_target = log(atol) - log_two;  /* reserve half of atol */
    REAL8 log_pref = log_two
                   + a * log1p(beta * beta)
                   - 2.0 * a * log1p(-beta);
    REAL8 log_z_over_2 = log(0.5 * z);

    for (;;) {
        REAL8 k = (REAL8)N + 1.0;
        REAL8 ratio = z / (2.0 * ((REAL8)N + 2.0));
        if (ratio < 1.0) {
            REAL8 log_first = k * log_z_over_2 - lgamma(k + 1.0)
                            + z * z / (4.0 * (k + 1.0));
            REAL8 log_tail = log_pref + log_first - log1p(-ratio);
            if (log_tail <= log_target)
                return N;
        }
        if (N >= INT_MAX - 2)
            return choose_N_a(p_abs, q, e, beta, atol, a);
        ++N;
    }
}

/* Add x to sum using Neumaier compensated summation. */
static inline void compensated_add(REAL8 x, REAL8 *sum, REAL8 *corr)
{
    REAL8 t = *sum + x;
    if (fabs(*sum) >= fabs(x))
        *corr += (*sum - t) + x;
    else
        *corr += (x - t) + *sum;
    *sum = t;
}

REAL8 J_pqa0_series_optimized(INT p, INT q, INT a, REAL8 e,
                              REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e, EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be >= 0\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (atol < 0.0 || rtol < 0.0 || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: atol and rtol must be non-negative and not both zero\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    /* J_{-p,-q}=J_{p,q}; work with p >= 0. */
    if (p < 0) {
        if (p == INT_MIN || q == INT_MIN) {
            XPrintError("Error - %s: p and q must be greater than INT_MIN\n", __func__);
            X_ERROR_REAL8(X_EINVAL);
        }
        p = -p;
        q = -q;
    }
    if (a == 0)
        return jn(p, (REAL8)q * e);
    if (J_pqa0_below_absolute_tolerance(p, q, a, e, atol, rtol))
        return 0.0;

    REAL8 beta = eval_beta(e);
    INT N = choose_N_a_optimized(p, q, e, beta, atol, rtol, a);
    REAL8 z = (REAL8)q * e;
    REAL8 z_abs = fabs(z);

    REAL8 *J = MYMalloc(((size_t)N + 1) * sizeof(*J));
    if (!J) {
        XPrintError("Error - %s: cannot allocate Bessel array\n", __func__);
        X_ERROR_REAL8(X_ENOMEM);
    }

    /* One O(N) recurrence replaces N independent, increasingly costly jn calls. */
    INT gsl_status;
    X_CALLGSL(gsl_status = gsl_sf_bessel_Jn_array(0, N, z_abs, J));
    if (gsl_status != GSL_SUCCESS) {
        /* GSL may report underflow for the whole array; preserve correctness. */
        for (INT k = 0; k <= N; ++k)
            J[k] = jn(k, z_abs);
    }

    REAL8 sum = 0.0;
    REAL8 corr = 0.0;
    REAL8 sum_abs = 0.0;
    for (INT k = 0; k <= N; ++k) {
        REAL8 Jk = J[k];
        if (z < 0.0 && (k & 1))
            Jk = -Jk;

        INT n_minus = abs(p - k);
        REAL8 L_minus = laplace_na(n_minus, a, beta);
        REAL8 term;
        if (k == 0) {
            term = Jk * L_minus;
        } else {
            REAL8 L_plus = laplace_na(p + k, a, beta);
            term = Jk * (L_minus + ((k & 1) ? -L_plus : L_plus));
        }
        sum_abs += fabs(term);
        compensated_add(term, &sum, &corr);
    }

    MYFree(J);
    REAL8 factor = pow(1.0 + beta * beta, a);
    REAL8 value = factor * (sum + corr);
    REAL8 roundoff_est = 16.0 * DBL_EPSILON * factor * sum_abs;
    if (roundoff_est > atol + rtol * fabs(value)) {
        /* Preserve the legacy result in severely cancellation-dominated cases. */
        return J_pqa0_series(p, q, a, e, atol, rtol);
    }
    return value;
}

REAL8 J_pqa0_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *jc, LaplaceCache2D *lc,
    REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    // if (a==0) return jn(p,q*e);  
    if(a==0) return get_BesselJ_from_BesselJCache2D(p, q, jc);
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT N = choose_N_a(p_abs, q, e, beta, atol, a);

    INT kmin = -N, kmax = N;
    INT M = kmax - kmin + 1;
    INT kMin = p_abs < 5 ? 5 : p_abs;
    // INT kMin = 10;
    /* Bessel array J_k(z) (both signs)                                */
    // REAL8 *Jk = MYMalloc(M * sizeof(REAL8));
    // bessel_fill(kmin, kmax, z, Jk);
    // BesselJCache *jc = CreateBesselJCache(kmax, z);
    size_t nMax = abs(kmax) + abs(p); // just guess
    // LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);

    /* series summation                                                */
    INT ikcum = 0;
    INT ikcumMax = 2;
    REAL8 sum = get_BesselJ_from_BesselJCache2D(0, q, jc) * get_Laplace_from_LaplaceCache2D(p_abs, a, lc);
    REAL8 sumP = 0.0;
    for (INT k = 1 ;  ; ++k) {
        REAL8 bessP = get_BesselJ_from_BesselJCache2D(k, q, jc);
        // REAL8 bessM = get_BesselJ_from_BesselJCache(-k, jc);
        INT nP = abs(k + p);
        // INT nM = abs(-k+p);             /* |k+p| */
        REAL8 LP = get_Laplace_from_LaplaceCache2D(nP, a, lc);
        // REAL8 LM = get_Laplace_from_LaplaceCache(nM, lc);

        REAL8 term = (k&1 ? -1.0 : 1.0) * (bessP * LP);
        sumP += term;
        // print_debug("[%d]term/sumP = %.16e/%.16e\n", 
        //         k, term, sumP);
        if (fabs(term) <= rtol * fabs(sumP)) {   /* early exit */
            ikcum ++;
            if (k > kMin && ikcum > ikcumMax) break;            /* tail symmetric */
            // else ikcum = 0;
        } else 
            ikcum = 0;
    }
    REAL8 sumM = 0.0;
    for (INT k = 1 ;  ; ++k) {
        // REAL8 bessP = get_BesselJ_from_BesselJCache(k, jc);
        REAL8 bessM = get_BesselJ_from_BesselJCache2D(-k, q, jc);
        // INT nP = abs(k + p);
        INT nM = abs(-k+p);             /* |k+p| */
        // REAL8 LP = get_Laplace_from_LaplaceCache(nP, lc);
        REAL8 LM = get_Laplace_from_LaplaceCache2D(nM, a, lc);

        REAL8 term = (k&1 ? -1.0 : 1.0) * (bessM*LM);
        // print_debug("[%d]term/sumM = %.16e/%.16e\n", 
        //         k, term, sumM);
        sumM += term;
        if (fabs(term) <= rtol * fabs(sumM)) {   /* early exit */
            ikcum ++;
            if (k > kMin && ikcum > ikcumMax) break;            /* tail symmetric */
        } else 
            ikcum = 0;
    }

    // MYFree(Jk);
    // STRUCTFREE(jc, BesselJCache);
    // STRUCTFREE(lc, LaplaceCache);
    // DestroyLaplaceCache(lc);
    REAL8 factor = pow(1.0 + beta2, a);
    return factor * (sum + sumP + sumM);   /* already includes 1/(2π) via Laplace */
}

REAL8 J_pqa0_series_cache_optimized(INT p, INT q, INT a, REAL8 e,
    BesselJCache2D *jc, LaplaceCache2D *lc,
    REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e, EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be >= 0\n", __func__, a);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (!jc || !lc) {
        XPrintError("Error - %s: input caches must not be NULL\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (atol < 0.0 || rtol < 0.0 || (atol == 0.0 && rtol == 0.0)) {
        XPrintError("Error - %s: atol and rtol must be non-negative and not both zero\n", __func__);
        X_ERROR_REAL8(X_EINVAL);
    }

    if (p < 0) {
        if (p == INT_MIN || q == INT_MIN) {
            XPrintError("Error - %s: p and q must be greater than INT_MIN\n", __func__);
            X_ERROR_REAL8(X_EINVAL);
        }
        p = -p;
        q = -q;
    }
    if (a == 0)
        return get_BesselJ_from_BesselJCache2D(p, q, jc);
    if (J_pqa0_below_absolute_tolerance(p, q, a, e, atol, rtol))
        return 0.0;

    REAL8 beta = eval_beta(e);
    INT N = choose_N_a_optimized(p, q, e, beta, atol, rtol, a);
    REAL8 sum = 0.0;
    REAL8 corr = 0.0;
    REAL8 sum_abs = 0.0;

    for (INT k = 0; k <= N; ++k) {
        REAL8 Jk = get_BesselJ_from_BesselJCache2D(k, q, jc);
        INT n_minus = abs(p - k);
        REAL8 L_minus = get_Laplace_from_LaplaceCache2D(n_minus, a, lc);
        REAL8 term;
        if (k == 0) {
            term = Jk * L_minus;
        } else {
            REAL8 L_plus = get_Laplace_from_LaplaceCache2D(p + k, a, lc);
            term = Jk * (L_minus + ((k & 1) ? -L_plus : L_plus));
        }
        sum_abs += fabs(term);
        compensated_add(term, &sum, &corr);
    }

    REAL8 factor = pow(1.0 + beta * beta, a);
    REAL8 value = factor * (sum + corr);
    REAL8 roundoff_est = 16.0 * DBL_EPSILON * factor * sum_abs;
    if (roundoff_est > atol + rtol * fabs(value)) {
        return J_pqa0_series_cache(p, q, a, e, jc, lc, atol, rtol);
    }
    return value;
}


/**
 * 
 *              
 * 
 *                  Analytic Approximation
 * 
 * 
 */
REAL8 Jpa0_Approx(INT p, INT a, REAL8 e)
{
    if(a < 1 || a>14)
    {
        XPrintError("Error - %s: a = %d must be in [1,14]\n", __func__, a);
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
    REAL8 e14 = e2*e2*e2*e2*e2*e2*e2;
    REAL8 deltae2 = 1. - e2;
    REAL8 deltae = sqrt(deltae2);
    REAL8 t = 2.*deltae - 1.;
    REAL8 lnPref = absp*log(e) - (2.*a-1.)*log(deltae);
    REAL8 term_SP;
    if ( a==8 && absp==1 ) {
        term_SP = 
            COEFFS_a8_SP[0] + e2*(
                COEFFS_a8_SP[1] + e2*(
                    COEFFS_a8_SP[2] + e2*(
                        COEFFS_a8_SP[3] + e2*(
                            COEFFS_a8_SP[4] + e2*(
                                COEFFS_a8_SP[5] + e2*(
                                    COEFFS_a8_SP[6] + e2*(
                                        COEFFS_a8_SP[7]
                                    )
                                )
                            )
                        )
                    )
                )
            ); 
        REAL8 term_cheby_SP = COEFFS_a8_Cheby_SP[0] + t*(
                COEFFS_a8_Cheby_SP[1] + t*(
                    COEFFS_a8_Cheby_SP[2] + t*(
                        COEFFS_a8_Cheby_SP[3] + t*COEFFS_a8_Cheby_SP[4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if ( a==9 && absp==1 ) {
        term_SP = 
            COEFFS_a9_SP[0] + e2*(
                COEFFS_a9_SP[1] + e2*(
                    COEFFS_a9_SP[2] + e2*(
                        COEFFS_a9_SP[3] + e2*(
                            COEFFS_a9_SP[4] + e2*(
                                COEFFS_a9_SP[5] + e2*(
                                    COEFFS_a9_SP[6] + e2*(
                                        COEFFS_a9_SP[7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a9_Cheby_SP[0] + t*(
                COEFFS_a9_Cheby_SP[1] + t*(
                    COEFFS_a9_Cheby_SP[2] + t*(
                        COEFFS_a9_Cheby_SP[3] + t*COEFFS_a9_Cheby_SP[4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if ( a==10 && absp<3 ) {
        term_SP = 
            COEFFS_a10_SP[absp-1][0] + e2*(
                COEFFS_a10_SP[absp-1][1] + e2*(
                    COEFFS_a10_SP[absp-1][2] + e2*(
                        COEFFS_a10_SP[absp-1][3] + e2*(
                            COEFFS_a10_SP[absp-1][4] + e2*(
                                COEFFS_a10_SP[absp-1][5] + e2*(
                                    COEFFS_a10_SP[absp-1][6] + e2*(
                                        COEFFS_a10_SP[absp-1][7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a10_Cheby_SP[absp-1][0] + t*(
                COEFFS_a10_Cheby_SP[absp-1][1] + t*(
                    COEFFS_a10_Cheby_SP[absp-1][2] + t*(
                        COEFFS_a10_Cheby_SP[absp-1][3] + t*COEFFS_a10_Cheby_SP[absp-1][4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if ( a==11 && absp<3 ) {
        term_SP = 
            COEFFS_a11_SP[absp-1][0] + e2*(
                COEFFS_a11_SP[absp-1][1] + e2*(
                    COEFFS_a11_SP[absp-1][2] + e2*(
                        COEFFS_a11_SP[absp-1][3] + e2*(
                            COEFFS_a11_SP[absp-1][4] + e2*(
                                COEFFS_a11_SP[absp-1][5] + e2*(
                                    COEFFS_a11_SP[absp-1][6] + e2*(
                                        COEFFS_a11_SP[absp-1][7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a11_Cheby_SP[absp-1][0] + t*(
                COEFFS_a11_Cheby_SP[absp-1][1] + t*(
                    COEFFS_a11_Cheby_SP[absp-1][2] + t*(
                        COEFFS_a11_Cheby_SP[absp-1][3] + t*COEFFS_a11_Cheby_SP[absp-1][4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if ( a==12 && absp<3 ) {
        term_SP = 
            COEFFS_a12_SP[absp-1][0] + e2*(
                COEFFS_a12_SP[absp-1][1] + e2*(
                    COEFFS_a12_SP[absp-1][2] + e2*(
                        COEFFS_a12_SP[absp-1][3] + e2*(
                            COEFFS_a12_SP[absp-1][4] + e2*(
                                COEFFS_a12_SP[absp-1][5] + e2*(
                                    COEFFS_a12_SP[absp-1][6] + e2*(
                                        COEFFS_a12_SP[absp-1][7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a12_Cheby_SP[absp-1][0] + t*(
                COEFFS_a12_Cheby_SP[absp-1][1] + t*(
                    COEFFS_a12_Cheby_SP[absp-1][2] + t*(
                        COEFFS_a12_Cheby_SP[absp-1][3] + t*COEFFS_a12_Cheby_SP[absp-1][4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if (a==13 && absp<3) {
        term_SP = 
            COEFFS_a13_SP[absp-1][0] + e2*(
                COEFFS_a13_SP[absp-1][1] + e2*(
                    COEFFS_a13_SP[absp-1][2] + e2*(
                        COEFFS_a13_SP[absp-1][3] + e2*(
                            COEFFS_a13_SP[absp-1][4] + e2*(
                                COEFFS_a13_SP[absp-1][5] + e2*(
                                    COEFFS_a13_SP[absp-1][6] + e2*(
                                        COEFFS_a13_SP[absp-1][7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a13_Cheby_SP[absp-1][0] + t*(
                COEFFS_a13_Cheby_SP[absp-1][1] + t*(
                    COEFFS_a13_Cheby_SP[absp-1][2] + t*(
                        COEFFS_a13_Cheby_SP[absp-1][3] + t*COEFFS_a13_Cheby_SP[absp-1][4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    } else if (a==14 && absp<4) {
        term_SP = 
            COEFFS_a14_SP[absp-1][0] + e2*(
                COEFFS_a14_SP[absp-1][1] + e2*(
                    COEFFS_a14_SP[absp-1][2] + e2*(
                        COEFFS_a14_SP[absp-1][3] + e2*(
                            COEFFS_a14_SP[absp-1][4] + e2*(
                                COEFFS_a14_SP[absp-1][5] + e2*(
                                    COEFFS_a14_SP[absp-1][6] + e2*(
                                        COEFFS_a14_SP[absp-1][7]
                                    )
                                )
                            )
                        )
                    )
                )
            );
        REAL8 term_cheby_SP = COEFFS_a14_Cheby_SP[absp-1][0] + t*(
                COEFFS_a14_Cheby_SP[absp-1][1] + t*(
                    COEFFS_a14_Cheby_SP[absp-1][2] + t*(
                        COEFFS_a14_Cheby_SP[absp-1][3] + t*COEFFS_a14_Cheby_SP[absp-1][4]
                    )
                )
            );
        return exp(lnPref) * (term_SP + e14*deltae*term_cheby_SP);
    }
    REAL8 term0 = 
        COEFFS_jh[a-1][absp-1][0] + e2*(
            COEFFS_jh[a-1][absp-1][1] + e2*(
                COEFFS_jh[a-1][absp-1][2] + e2*(
                    COEFFS_jh[a-1][absp-1][3] + e2*(
                        COEFFS_jh[a-1][absp-1][4] + e2*(
                            COEFFS_jh[a-1][absp-1][5] + e2*COEFFS_jh[a-1][absp-1][6]
                        )
                    )
                )
            )
        );
    REAL8 term1 = 
        COEFFS_jc[a-1][absp-1][0] + deltae*(
            COEFFS_jc[a-1][absp-1][1] + deltae*COEFFS_jc[a-1][absp-1][2]
        );
    term1 = e14*term1;
    REAL8 term2 = COEFFS_cheby[a-1][absp-1][0] + t*(
            COEFFS_cheby[a-1][absp-1][1] + t*(
                COEFFS_cheby[a-1][absp-1][2] + t*(
                    COEFFS_cheby[a-1][absp-1][3] + t*(
                        COEFFS_cheby[a-1][absp-1][4] + t*(
                            COEFFS_cheby[a-1][absp-1][5] + t*(
                                COEFFS_cheby[a-1][absp-1][6] + t*(
                                    COEFFS_cheby[a-1][absp-1][7] + t*(
                                        COEFFS_cheby[a-1][absp-1][8] + t*(
                                            COEFFS_cheby[a-1][absp-1][9] + t*(
                                                COEFFS_cheby[a-1][absp-1][10]
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
    term2 = e14*deltae*deltae2*term2;
    return exp(lnPref + term0 + term1 + term2 );
    // return COEFFSJPA0f[a-1][absp-1][1];
}

REAL8 dJpa0_Approx(INT p, INT a, REAL8 e)
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
    REAL8 e14 = e2*e2*e2*e2*e2*e2*e2;
    REAL8 deltae2 = 1. - e2;
    REAL8 deltae = sqrt(deltae2);
    REAL8 t = 2.*deltae - 1.;
    REAL8 lnPref = (absp-1)*log(e) - (2.*a+1.)*log(deltae);
    REAL8 lneprL = 
            COEFFSdJb0_a1_jhc[absp-1][0] + e2*(
                COEFFSdJb0_a1_jhc[absp-1][1] + e2*(
                    COEFFSdJb0_a1_jhc[absp-1][2] + e2*(
                        COEFFSdJb0_a1_jhc[absp-1][3] + e2*(
                            COEFFSdJb0_a1_jhc[absp-1][4] + e2*(
                                COEFFSdJb0_a1_jhc[absp-1][5] + e2*(
                                    COEFFSdJb0_a1_jhc[absp-1][6]
                                )
                            )
                        )
                    )
                )
            );
    REAL8 epr_cheby = 
            COEFFSdJb0_a1_cheby[absp-1][0] + t*(
                COEFFSdJb0_a1_cheby[absp-1][1] + t*(
                    COEFFSdJb0_a1_cheby[absp-1][2] + t*(
                        COEFFSdJb0_a1_cheby[absp-1][3] + t*(
                            COEFFSdJb0_a1_cheby[absp-1][4] + t*(
                                COEFFSdJb0_a1_cheby[absp-1][5] + t*(
                                    COEFFSdJb0_a1_cheby[absp-1][6] + t*(
                                        COEFFSdJb0_a1_cheby[absp-1][7] + t*(
                                            COEFFSdJb0_a1_cheby[absp-1][8] + t*COEFFSdJb0_a1_cheby[absp-1][9]
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            );
    return exp(lnPref+lneprL)*(1. + e14*( deltae*epr_cheby + COEFFSdJb0_a1_jhc[absp-1][7] ));
}

#if 0
REAL8 J_pqa0_series_cache(INT p, INT q, INT a, REAL8 e, 
    BesselJCache2D *jc, LaplaceCache2D *lc,
    REAL8 atol, REAL8 rtol)
{
    if (e <= 0.0 || e >= EMAX) {                      
        XPrintError("Error - %s: e = %f must be in (0,%e)\n", __func__, e , EMAX);
        X_ERROR_REAL8(X_EINVAL);
    }
    if (a < 0) {
        XPrintError("Error - %s: power a = %d must be ≥ 0\n", __func__, a );
        X_ERROR_REAL8(X_EINVAL);
    }

    // if (a==0) return jn(p,q*e);  
    if(a==0) return jn(p,q*e);
    REAL8 beta = eval_beta(e);
    REAL8 beta2 = beta * beta;
    REAL8 z    = q * e;

    /* choose truncation N so that tail < tol                          */
    INT p_abs = abs(p);
    INT N = choose_N_a(p_abs, q, e, beta, atol, a);

    INT kmin = -N, kmax = N;
    INT M = kmax - kmin + 1;

    /* Bessel array J_k(z) (both signs)                                */
    // REAL8 *Jk = MYMalloc(M * sizeof(REAL8));
    // bessel_fill(kmin, kmax, z, Jk);
    size_t nMax = abs(kmax) + abs(p); // just guess
    // LaplaceCache *lc = CreateLaplaceCache(nMax, a, beta);

    /* series summation                                                */
    REAL8 sum = 0.0;
    for (INT k = kmin; k <= kmax; ++k) {
        // INT idx = k - kmin;
        // REAL8 bess = Jk[idx];
        REAL8 bess = get_BesselJ_from_BesselJCache2D(k, q, jc);
        if (k & 1) bess = -bess;          /* (-1)^k factor */
        INT n = abs(k + p);             /* |k+p| */
        // REAL8 Lnp = laplace_na(n, a, beta);
        // REAL8 Lnp = get_Laplace_from_LaplaceCache(n, lc);
        REAL8 Lnp = get_Laplace_from_LaplaceCache2D(n, a, lc);

        REAL8 term = bess * Lnp;
        // print_debug("Laplace(%d, %d, %e) = %e\n", n, a, beta, Lnp);
        sum += term;
        if (fabs(term) <= rtol * fabs(sum)) {   /* early exit */
            if (k > p_abs) break;            /* tail symmetric */
        }
    }
    // MYFree(Jk);
    // DestroyLaplaceCache(lc);
    REAL8 factor = pow(1.0 + beta2, a);
    return factor * sum;   /* already includes 1/(2π) via Laplace */
}
#endif

/* ---------- test ------------------------------------------ */
// INT testit()
// {
//     INT    p_min = 0;
//     INT    p_max = 5;
//     INT    q     = 1;
//     INT    a     = 20; 
//     REAL8  e     = 0.99;

//     int P = p_max - p_min + 1;
//     double *vals = MYMalloc(sizeof(double)*P);

//     Jpqa_rangep(p_min, p_max, q, a, e, 1e-16, vals);

//     for (int i=0;i<P;++i)
//         printf("Jnew[%d,%d,%d,%.6g]/(2π) = %.15e\n",
//                p_min+i, q, a, e, vals[i]);

//     MYFree(vals);
//     return 0;
// }
