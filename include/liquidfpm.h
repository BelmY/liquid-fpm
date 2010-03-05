/*
 * Copyright (c) 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010 Virginia Polytechnic
 *                                Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIQUIDFPM_H__
#define __LIQUIDFPM_H__

/*
 * Make sure the version and version number macros weren't defined by
 * some prevoiusly included header file.
 */
#ifdef LIQUIDFPM_VERSION
#  undef LIQUIDFPM_VERSION
#endif
#ifdef LIQUIDPM_VERSION_NUMBER
#  undef LIQUIDPM_VERSION_NUMBER
#endif

/*
 * Compile-time version numbers
 * 
 * LIQUIDFPM_VERSION = "X.Y.Z"
 * LIQUIDFPM_VERSION_NUMBER = (X*1000000 + Y*1000 + Z)
 */
#define LIQUIDFPM_VERSION           "0.1.0"
#define LIQUIDFPM_VERSION_NUMBER    1000

/*
 * Run-time library version numbers
 */
extern const char liquidfpm_version[];
const char * liquidfpm_libversion(void);
int liquidfpm_libversion_number(void);


#ifdef __cplusplus
extern "C" {
#   define LIQUIDFPM_USE_COMPLEX_H 0
#else
#   define LIQUIDFPM_USE_COMPLEX_H 1
#endif /* __cplusplus */

#include <inttypes.h>

/* concatenation function */
#define LIQUIDFPM_CONCAT(prefix, name) prefix ## name

/* 
 * Default: use the C99 complex data type, otherwise
 * define complex type compatible with the C++ complex standard,
 * otherwise resort to defining binary compatible array.
 */
#if LIQUIDFPM_USE_COMPLEX_H==1
#   include <complex.h>
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef R _Complex C
#elif defined _GLIBCXX_COMPLEX
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef std::complex<R> C
#else
#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef R C[2]
#endif
//#   define LIQUIDFPM_DEFINE_COMPLEX(R,C) typedef C struct {R real; R imag;};

LIQUIDFPM_DEFINE_COMPLEX(float, liquidfpm_float_complex);


/* name-mangling macros */
#define LIQUIDFPM_MANGLE_Q32(name)  LIQUIDFPM_CONCAT(q32, name)
#define LIQUIDFPM_MANGLE_CQ32(name) LIQUIDFPM_CONCAT(cq32,name)

/* API definition macro
 *
 * Q        :   name-mangling macro
 * T        :   primitive data type
 * INTBITS  :   number of integer bits
 * FRACBITS :   number of fractional bits
 */
#define LIQUIDFPM_DEFINE_API(Q,T,INTBITS,FRACBITS)                  \
typedef T Q(_t);                                                    \
const static unsigned int Q(_bits) = INTBITS + FRACBITS;            \
const static unsigned int Q(_intbits) = INTBITS;                    \
const static unsigned int Q(_fracbits) = FRACBITS;                  \
static inline int Q(_intpart) (Q(_t) _x)                            \
    { return (_x >> FRACBITS); } ;                                  \
static inline int Q(_fracpart) (Q(_t) _x)                           \
    { return _x & ((1<<FRACBITS)-1); };                             \
                                                                    \
/* constants */                                                     \
const static Q(_t) Q(_min) = (1);                                   \
const static Q(_t) Q(_max) = (((1<<(FRACBITS+INTBITS-2))-1)<<1)+1;  \
const static Q(_t) Q(_one) = (1<<(FRACBITS))-1;                     \
const static Q(_t) Q(_pi)  = (1<<(FRACBITS+INTBITS-2))-1;           \
const static Q(_t) Q(_2pi) = (((1<<(FRACBITS+INTBITS-2))-1)<<1)+1;  \
const static Q(_t) Q(_pi_by_2)  =                                   \
    (((1<<(FRACBITS+INTBITS-2))-1)>>1);                             \
const static float Q(_angle_scalar) =                               \
    (3.14159265358979/(float)(1<<(INTBITS-2)));                     \
                                                                    \
/* conversion */                                                    \
static inline float Q(_fixed_to_float)(Q(_t) _x)                    \
    { return (float) (_x) / (float)(1 << FRACBITS); };              \
static inline Q(_t) Q(_float_to_fixed)(float _x)                    \
    { return (Q(_t)) (_x * (float)(1 << FRACBITS) + 0.5f); };       \
static inline float Q(_angle_fixed_to_float)(Q(_t) _x)              \
    { return Q(_fixed_to_float(_x)) * Q(_angle_scalar); };          \
static inline Q(_t) Q(_angle_float_to_fixed)(float _x)              \
    { return Q(_float_to_fixed(_x)) / Q(_angle_scalar); };          \
                                                                    \
/* arithmetic */                                                    \
static inline Q(_t) Q(_sign)(Q(_t) _x) {return (_x<0) ? -1 : 1;};   \
static inline Q(_t) Q(_abs)(Q(_t) _x) {return (_x<0) ? -_x : _x;};  \
static inline Q(_t) Q(_add)(Q(_t) _a, Q(_t) _b) {return _a + _b;};  \
static inline Q(_t) Q(_sub)(Q(_t) _a, Q(_t) _b) {return _a - _b;};  \
Q(_t) Q(_mul)(Q(_t) _a, Q(_t) _b);                                  \
Q(_t) Q(_div)(Q(_t) _a, Q(_t) _b);                                  \
Q(_t) Q(_inv)(Q(_t) _x, unsigned int _n); /* compute 1/_x */        \
Q(_t) Q(_ratio)(int _a, int _b, unsigned int _n); /* compute a/b */ \
                                                                    \
/* trig */                                                          \
Q(_t) Q(_sin)(Q(_t) _theta);                                        \
Q(_t) Q(_cos)(Q(_t) _theta);                                        \
void  Q(_sincos)(Q(_t) _theta, Q(_t) * _sin, Q(_t) * _cos);         \
Q(_t) Q(_tan)(Q(_t) _theta);                                        \
Q(_t) Q(_atan2)(Q(_t) _x, Q(_t) _y);                                \
                                                                    \
/* log, etc. */                                                     \
Q(_t) Q(_log2)(Q(_t) _x);                                           \
Q(_t) Q(_exp2)(Q(_t) _x);                                           \
Q(_t) Q(_log) (Q(_t) _x);                                           \
Q(_t) Q(_exp) (Q(_t) _x);                                           \
Q(_t) Q(_sqrt)(Q(_t) _x);                                           \
Q(_t) Q(_pow) (Q(_t) _x, Q(_t) _y);                                 \
                                                                    \
/* fixed|float type */                                              \
typedef struct {T base; Q(_t) frac;} Q(f_t);                        \
float Q(f_fixed_to_float)(Q(f_t) _x);                               \
Q(f_t) Q(f_float_to_fixed)(float _x);                               \
Q(f_t) Q(f_add)(Q(f_t) _x, Q(f_t) _y);                              \
Q(f_t) Q(f_sub)(Q(f_t) _x, Q(f_t) _y);                              \
Q(f_t) Q(f_mul)(Q(f_t) _x, Q(f_t) _y);                              \
Q(f_t) Q(f_div)(Q(f_t) _x, Q(f_t) _y);                              \
void Q(f_constrain)(Q(f_t) * _x);                                   \
                                                                    \
/* math: transcendentals */                                         \
Q(_t) Q(_gamma)(Q(_t) _z);                                          \
Q(_t) Q(_lngamma)(Q(_t) _z);                                        \
Q(_t) Q(_besseli0)(Q(_t) _z);                                       \
Q(_t) Q(_besselj0)(Q(_t) _z);                                       \
Q(_t) Q(_sinc)(Q(_t) _z);                                           \
Q(_t) Q(_kaiser)(unsigned int _n, unsigned int _N, Q(_t) _beta);    \
                                                                    \
/* vector operations */                                             \
Q(_t) Q(_dotprod)(Q(_t) * _x, Q(_t) * _v, unsigned int _n);


LIQUIDFPM_DEFINE_API(LIQUIDFPM_MANGLE_Q32, int32_t, 7, 25)

/* API definition macro (complex types)
 *
 * CQ       :   name-mangling macro (complex)
 * Q        :   name-mangling macro (real)
 */
#define LIQUIDFPM_DEFINE_COMPLEX_API(CQ,Q)                          \
typedef struct {Q(_t) real; Q(_t) imag;} CQ(_t);                    \
                                                                    \
/* accessor functions */                                            \
static inline Q(_t) CQ(_real)(CQ(_t) _a) {return _a.real;};         \
static inline Q(_t) CQ(_imag)(CQ(_t) _a) {return _a.imag;};         \
CQ(_t) CQ(_conj)(CQ(_t) _a);                                        \
                                                                    \
/* conversion */                                                    \
liquidfpm_float_complex CQ(_fixed_to_float)(CQ(_t) _x);             \
CQ(_t) CQ(_float_to_fixed)(liquidfpm_float_complex _x);             \
                                                                    \
/* arithmetic */                                                    \
CQ(_t) CQ(_add)(CQ(_t) _a, CQ(_t) _b);                              \
CQ(_t) CQ(_sub)(CQ(_t) _a, CQ(_t) _b);                              \
CQ(_t) CQ(_mul)(CQ(_t) _a, CQ(_t) _b);                              \
CQ(_t) CQ(_mul_scalar)(CQ(_t) _a, Q(_t) _b);                        \
CQ(_t) CQ(_div)(CQ(_t) _a, CQ(_t) _b);                              \
CQ(_t) CQ(_div_scalar)(CQ(_t) _a, Q(_t) _b);                        \
CQ(_t) CQ(_inv)(CQ(_t) _x); /* compute 1/_x */                      \
                                                                    \
/* trig */                                                          \
CQ(_t) CQ(_csin) (CQ(_t) _x);                                       \
CQ(_t) CQ(_ccos) (CQ(_t) _x);                                       \
CQ(_t) CQ(_ctan) (CQ(_t) _x);                                       \
static inline Q(_t) CQ(_carg)(CQ(_t) _x)                            \
    { return Q(_atan2)(_x.imag, _x.real); };                        \
Q(_t) CQ(_abs2) (CQ(_t) _x);                                        \
Q(_t) CQ(_abs) (CQ(_t) _x);                                         \
                                                                    \
/* log, etc. */                                                     \
CQ(_t) CQ(_cexp)(CQ(_t) _x);                                        \
CQ(_t) CQ(_clog)(CQ(_t) _x);                                        \
CQ(_t) CQ(_csqrt)(CQ(_t) _x);

LIQUIDFPM_DEFINE_COMPLEX_API(LIQUIDFPM_MANGLE_CQ32, LIQUIDFPM_MANGLE_Q32)

#ifdef __cplusplus
}   /* extern "C" */
#endif /* __cplusplus */

#endif // __LIQUIDFPM_H__
