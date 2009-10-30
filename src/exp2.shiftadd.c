// 
// exp2 shift/add
//
// Algorithm from fxtbook, ch. 31
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_EXP2_SHIFTADD     0

#define Q(name)     LIQUIDFPM_CONCAT(q32,name)

// natural logarithm
//    e^(x) = 2^(x * log2(e))
Q(_t) Q(_exp_shiftadd)(Q(_t) _x, unsigned int _n)
{
    return Q(_exp2_shiftadd)( Q(_mul)(_x,Q(_log2_e)), _n );
}

// base-10 logarithm
//    10^(x) = 2^(x * log2(10))
Q(_t) Q(_exp10_shiftadd)(Q(_t) _x, unsigned int _n)
{
    return Q(_exp2_shiftadd)( Q(_mul)(_x,Q(_log2_10)), _n );
}

// power function
//    b^(x) = 2^(x * log2(b))
Q(_t) Q(_pow_shiftadd)(Q(_t) _b,
                       Q(_t) _x,
                       unsigned int _n)
{
    Q(_t) log2_b = Q(_log2_shiftadd)(_b,_n);
    return Q(_exp2_shiftadd)( Q(_mul)(_x,log2_b), _n );
}

// Computes y = exp2(x)
Q(_t) Q(_exp2_shiftadd)(Q(_t) _x,
                        unsigned int _n)
{
    if (_x >= 0)
        return Q(_exp2_shiftadd_base)(_x,_n);

    // base, fraction
    int b   = Q(_intpart)(_x);
    Q(_t) f = Q(_fracpart)(_x);

#if DEBUG_EXP2_SHIFTADD
    printf("x : %12.8f  > %d + %12.8f\n", Q(_fixed_to_float)(_x), 
                                          b,
                                          Q(_fixed_to_float)(f));
    printf("b : %d\n", b);
    printf("f : %f\n", Q(_fixed_to_float)(f));
#endif

    // test if base is larger than data type
    // example: for a 32-bit fixed-point data type with 20
    //          fraction bits, 2^-20 is effectively 0
    if ( abs(b) >= Q(_fracbits) ) {
        if (b < 0) return 0;
    }

    // compute the fractional portion using the iterative
    // shift|add algorithm.
    Q(_t) y = Q(_exp2_shiftadd_base)(f,_n);

    // post-shift the output by the integer amount (effectively
    // multiply by 2^-b)
    y >>= -b;

    return y;
}

// computes y=exp2(x) where x >= 0
//
// For values of x < 0, it is necessary to first subtract its
// integer component (i.e. compute x = b+f s.t. b is a negative
// integer and f>0).  The algorithm will NOT converge for
// x < 0, nor will it validate that the input is in this range.
Q(_t) Q(_exp2_shiftadd_base)(Q(_t) _x,
                             unsigned int _n)
{
    Q(_t) tn = 0;
    Q(_t) en = Q(_one);
    Q(_t) un = 0;
    Q(_t) vn = Q(_one);
    Q(_t) x = _x;
    int dn;
    unsigned int n = _n;
    if (n>Q(_log2_shiftadd_nmax))
        n = Q(_log2_shiftadd_nmax);
    unsigned int i;
#if DEBUG_EXP2_SHIFTADD
    printf("   n           un           tn           en           An\n");
    printf("init            - %12.8f %12.8f %12.8f\n",
            Q(_fixed_to_float)(tn),
            Q(_fixed_to_float)(en),
            Q(_fixed_to_float)(Q(_log2_shiftadd_Ak_tab)[0]));
#endif

    for (i=1; i<n; i++) {
        vn >>= 1;
        while (1) {
            un = tn + Q(_log2_shiftadd_Ak_tab)[i];
            dn = (un <= x);
#if DEBUG_EXP2_SHIFTADD
            printf("%4u %12.8f %12.8f %12.8f %12.4e\n",
                    i,
                    Q(_fixed_to_float)(un),
                    Q(_fixed_to_float)(tn),
                    Q(_fixed_to_float)(en),
                    Q(_fixed_to_float)(Q(_log2_shiftadd_Ak_tab)[i]));
#endif
            if (dn == 0) break;
            tn = un;
            en += en>>i;
        }
    }

    return en;
}

