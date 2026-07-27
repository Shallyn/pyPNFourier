#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat, 30 Nov 2024 13:24:35 +0000

@author: Shallyn
"""
import numpy as np
from pathlib import Path
import ctypes
from pathos.multiprocessing import ProcessingPool as Pool

from .ctypes_structs import *

def load_shared_lib_with_suffix(path_no_suffix):
    errors = []
    for suffix in (".so", ".dylib", ".dll"):
        try:
            return ctypes.CDLL(path_no_suffix + suffix)
        except OSError as exc:
            errors.append(f"{suffix}: {exc}")
    details = "\n".join(errors)
    raise OSError(
        f"Cannot load {path_no_suffix}.(so|dylib|dll). "
        "Build the native libraries before importing pyPNFourier.\n"
        f"{details}"
    )

libBasic_Prefix = Path(__file__).parent / 'libbasic/libbasic'
myLibBasic = load_shared_lib_with_suffix(str(libBasic_Prefix))

libcore_Prefix = Path(__file__).parent / 'libpnfourier/libpnFourier'
myLibcore = load_shared_lib_with_suffix(str(libcore_Prefix))

myLibBasic.CreateREAL8Vector.restype = ctypes.POINTER(pyREAL8Vector)

myLibcore.BesselJ.restype = ctypes.c_double
myLibcore.J_pq10_series.restype = ctypes.c_double
myLibcore.J_pqa0_series.restype = ctypes.c_double
myLibcore.J_pqa1_series.restype = ctypes.c_double
myLibcore.J_pqa2_series.restype = ctypes.c_double
myLibcore.J_pqa3_series.restype = ctypes.c_double

myLibcore.J_pqa0_series_optimized.restype = ctypes.c_double
myLibcore.J_pqa1_series_optimized.restype = ctypes.c_double
myLibcore.J_pqa2_series_optimized.restype = ctypes.c_double
myLibcore.J_pqa3_series_optimized.restype = ctypes.c_double

myLibcore.K_pqa0_series.restype = ctypes.c_double
myLibcore.K_pqa0_series_optimized.restype = ctypes.c_double

myLibcore.Jpa0_Approx.restype = ctypes.c_double
myLibcore.Jpa1_Approx.restype = ctypes.c_double
myLibcore.Jpa2_Approx.restype = ctypes.c_double
myLibcore.Jpa3_Approx.restype = ctypes.c_double
myLibcore.Kpa0_Approx.restype = ctypes.c_double

myLibcore.dJpa0_Approx.restype = ctypes.c_double
myLibcore.dJpa1_Approx.restype = ctypes.c_double
myLibcore.dJpa2_Approx.restype = ctypes.c_double


myLibcore.laplace_na.restype = ctypes.c_double
myLibcore.laplace_nega.restype = ctypes.c_double

myLibcore.loglaplace_na.restype = ctypes.c_double

myLibcore.Dlaplace_na.restype = ctypes.c_double


myLibcore.evaluate_JKIntegrals_Numeric.argtypes = (
    ctypes.c_int,
    ctypes.c_double,
    ctypes.POINTER(PNEllipticCacheV2),
    ctypes.c_double,
    ctypes.c_double
)
myLibcore.evaluate_JKIntegrals_Numeric.restype = ctypes.c_int

myLibcore.evaluate_JKIntegrals_Approx.argtypes = (
    ctypes.c_int,
    ctypes.c_double,
    ctypes.POINTER(PNEllipticCacheV2)
)
myLibcore.evaluate_JKIntegrals_Approx.restype = ctypes.c_int


def evaluate_JKIntegrals_Numeric(p: int, e: float, atol:float=1e-16, rtol:float=1e-16):
    """Numerically evaluate all fields of ``PNEllipticCacheV2``.

    Parameters
    ----------
    p : int
        Any signed integer representable by the C ``int`` used by the library.
    e : float
        Eccentricity satisfying ``0 < e < 1``.

    Returns
    -------
    dict
        Scalar fields are Python numbers and C array fields are lists.
    """
    if isinstance(p, bool) or not isinstance(p, int):
        raise TypeError("p must be an int")
    c_int_min = -(1 << (ctypes.sizeof(ctypes.c_int) * 8 - 1))
    c_int_max = (1 << (ctypes.sizeof(ctypes.c_int) * 8 - 1)) - 1
    if not c_int_min <= p <= c_int_max:
        raise OverflowError(
            f"p={p} cannot be represented by the library's C int "
            f"({c_int_min} <= p <= {c_int_max})"
        )
    if isinstance(e, bool) or not isinstance(e, (int, float, np.integer, np.floating)):
        raise TypeError("e must be a real number")
    e = float(e)
    if not np.isfinite(e) or not 0.0 < e < 1.0:
        raise ValueError("e must satisfy 0 < e < 1")

    cache = PNEllipticCacheV2()
    status = myLibcore.evaluate_JKIntegrals_Numeric(p, e, ctypes.byref(cache), ctypes.c_double(atol), ctypes.c_double(rtol))
    if status != 0:
        raise RuntimeError(
            f"evaluate_JKIntegrals_Numeric failed for p={p}, e={e} "
            f"(status={status})"
        )

    result = {}
    for name, field_type in cache._fields_:
        value = getattr(cache, name)
        if issubclass(field_type, ctypes.Array):
            result[name] = list(value)
        else:
            result[name] = value
    return result

def evaluate_JKIntegrals_Approx(p: int, e: float):
    """Numerically evaluate all fields of ``PNEllipticCacheV2``.

    Parameters
    ----------
    p : int
        Any signed integer representable by the C ``int`` used by the library.
    e : float
        Eccentricity satisfying ``0 < e < 1``.

    Returns
    -------
    dict
        Scalar fields are Python numbers and C array fields are lists.
    """
    if isinstance(p, bool) or not isinstance(p, int):
        raise TypeError("p must be an int")
    c_int_min = -(1 << (ctypes.sizeof(ctypes.c_int) * 8 - 1))
    c_int_max = (1 << (ctypes.sizeof(ctypes.c_int) * 8 - 1)) - 1
    if not c_int_min <= p <= c_int_max:
        raise OverflowError(
            f"p={p} cannot be represented by the library's C int "
            f"({c_int_min} <= p <= {c_int_max})"
        )
    if isinstance(e, bool) or not isinstance(e, (int, float, np.integer, np.floating)):
        raise TypeError("e must be a real number")
    e = float(e)
    if not np.isfinite(e) or not 0.0 < e < 1.0:
        raise ValueError("e must satisfy 0 < e < 1")

    cache = PNEllipticCacheV2()
    status = myLibcore.evaluate_JKIntegrals_Approx(p, e, ctypes.byref(cache))
    if status != 0:
        raise RuntimeError(
            f"evaluate_JKIntegrals_Approx failed for p={p}, e={e} "
            f"(status={status})"
        )

    result = {}
    for name, field_type in cache._fields_:
        value = getattr(cache, name)
        if issubclass(field_type, ctypes.Array):
            result[name] = list(value)
        else:
            result[name] = value
    return result



def convert_REAL8Vector_to_numpy(vec:ctypes.POINTER(pyREAL8Vector)):
    length = vec.contents.length
    ret = np.zeros(length)
    npdata = ret.ctypes.data
    ctypes.memmove(npdata, vec.contents.data, length*ctypes.sizeof(ctypes.c_double))
    return ret

def convert_ndarray_to_REAL8Vector(vec:np.ndarray):
    if len(vec.shape) > 1:
        raise Exception('the shape of vec should be 1-d')
    length = int(len(vec))
    npvec = np.zeros(length)
    npvec[:] = vec[:]
    ret = myLibBasic.CreateREAL8Vector(ctypes.c_uint(length))
    ctypes.memmove(ret.contents.data, npvec.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), length * ctypes.sizeof(ctypes.c_double))
    # print(ret.contents.data)
    # print(vec.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))
    # ctypes.memmove(ret.contents.data, vec.ctypes.data, vec.nbytes)
    return ret

def parallel_fVec_eval(f, vec: np.ndarray):
    with Pool() as pool:
        out = np.array(pool.map(f, vec.tolist() if isinstance(vec, np.ndarray) else vec))
    return np.asarray(out)

def J_pqa0(p:int, q:int, a:int, e:float, atol:float=1e-10, rtol:float=1e-10):
    return myLibcore.J_pqa0_series_optimized(ctypes.c_int(p), ctypes.c_int(q), ctypes.c_int(a), ctypes.c_double(e), ctypes.c_double(atol), ctypes.c_double(rtol))
def J_pqa1(p:int, q:int, a:int, e:float, atol:float=1e-10, rtol:float=1e-10):
    return myLibcore.J_pqa1_series_optimized(ctypes.c_int(p), ctypes.c_int(q), ctypes.c_int(a), ctypes.c_double(e), ctypes.c_double(atol), ctypes.c_double(rtol))
def J_pqa2(p:int, q:int, a:int, e:float, atol:float=1e-10, rtol:float=1e-10):
    return myLibcore.J_pqa2_series_optimized(ctypes.c_int(p), ctypes.c_int(q), ctypes.c_int(a), ctypes.c_double(e), ctypes.c_double(atol), ctypes.c_double(rtol))
def J_pqa3(p:int, q:int, a:int, e:float, atol:float=1e-10, rtol:float=1e-10):
    return myLibcore.J_pqa3_series_optimized(ctypes.c_int(p), ctypes.c_int(q), ctypes.c_int(a), ctypes.c_double(e), ctypes.c_double(atol), ctypes.c_double(rtol))
def K_pqa0(p:int, q:int, a:int, e:float, atol:float=1e-10, rtol:float=1e-10):
    return myLibcore.K_pqa0_series_optimized(ctypes.c_int(p), ctypes.c_int(q), ctypes.c_int(a), ctypes.c_double(e), ctypes.c_double(atol), ctypes.c_double(rtol))


def K_pa0_Approx(p:int, a:int, e:float):
    return myLibcore.Kpa0_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def J_pa0_Approx(p:int, a:int, e:float):
    return myLibcore.Jpa0_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def J_pa1_Approx(p:int, a:int, e:float):
    return myLibcore.Jpa1_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def J_pa2_Approx(p:int, a:int, e:float):
    return myLibcore.Jpa2_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def J_pa3_Approx(p:int, a:int, e:float):
    return myLibcore.Jpa3_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))

def dJ_pa0_Approx(p:int, a:int, e:float):
    return myLibcore.dJpa0_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def dJ_pa1_Approx(p:int, a:int, e:float):
    return myLibcore.dJpa1_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
def dJ_pa2_Approx(p:int, a:int, e:float):
    return myLibcore.dJpa2_Approx(ctypes.c_int(p), ctypes.c_int(a), ctypes.c_double(e))
