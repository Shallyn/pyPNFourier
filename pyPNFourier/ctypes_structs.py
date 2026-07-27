#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon, 02 Dec 2024 23:11:12 +0000

@author: Shallyn
"""
import ctypes

class pyREAL8Vector(ctypes.Structure):
    _fields_ = (('length', ctypes.c_uint),
                ('data', ctypes.POINTER(ctypes.c_double)))

class c_complex(ctypes.Structure):
    _fields_ = (('real', ctypes.c_double),
                ('imag', ctypes.c_double))
    @property
    def value(self):
        return self.real+1j*self.imag # fields declared above

c_double_complex_p = ctypes.POINTER(c_complex)
c_double_p = ctypes.POINTER(ctypes.c_double)
b_double_complex = lambda c: ctypes.byref(c_complex(c.real, c.imag))


class PNEllipticCacheV2(ctypes.Structure):
    """ctypes mirror of the public C ``PNEllipticCacheV2`` structure."""

    _fields_ = (
        ('e', ctypes.c_double),
        ('p', ctypes.c_int),
        ('Jpa0Vec_aPos', ctypes.c_double * 14),
        ('Jpa1Vec_aPos', ctypes.c_double * 11),
        ('Jpa1Vec_aNeg', ctypes.c_double * 4),
        ('Jpa2Vec_aPos', ctypes.c_double * 7),
        ('Jpa2Vec_aNeg', ctypes.c_double * 3),
        ('Jpa3Vec_aPos', ctypes.c_double * 3),
        ('Jpa3Vec_aNeg', ctypes.c_double * 2),
        ('Kpa0Vec', ctypes.c_double * 6),
        ('BesselJ', ctypes.c_double),
        ('dBesselJ', ctypes.c_double),
        ('dJp10', ctypes.c_double),
        ('dJp11', ctypes.c_double),
        ('dJp12', ctypes.c_double),
        ('dJSIntVec_b1', ctypes.c_double * 13),
        ('dJCIntVec_b2', ctypes.c_double * 9),
        ('dJSIntVec_b3', ctypes.c_double * 3),
    )
