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

//
// Complex fixed-point logarithmic functions
//


// compute complex exponent
//      exp(x) = exp(x.real) * [cos(x.imag) + j*sin(x.imag)]
CQ(_t) CQ(_cexp)(CQ(_t) _x)
{
    unsigned int _n=20; // number of iterations (precision)

    // run CORDIC to compute sin|cos from angle
    // convert angle to qtype format (multiply by scaling factor)
    //      theta = _x.real * 2^(intbits-2) / pi
    Q(_t) theta = Q(_mul)(_x.imag, Q(_angle_scalar));
    Q(_t) qsin;     // pure in-phase component
    Q(_t) qcos;     // pure quadrature component
    Q(_sincos_cordic)(theta, &qsin, &qcos, _n);

    // compute scaling factor
    Q(_t) scale = Q(_exp_shiftadd)(_x.real, _n);

    // complex exponent
    CQ(_t) cexp = { Q(_mul)(scale,qcos), Q(_mul)(scale,qsin) };
    return cexp;
}

// compute complex logarithm:
//      log(x) = log(|x|) + j*arg(x)
CQ(_t) CQ(_clog)(CQ(_t) _x)
{
    unsigned int _n=20; // number of iterations (precision)

    Q(_t) r;        // magnitude
    Q(_t) theta;    // angle

    // run CORDIC to compute both magnitude and angle of
    // complex phasor
    Q(_atan2_cordic)(_x.imag, _x.real, &r, &theta, _n);

    // complex logarithm
    CQ(_t) clog = { Q(_log_shiftadd)(Q(_abs)(r),_n), theta };
    return clog;
}


// compute square root
CQ(_t) CQ(_csqrt)(CQ(_t) _x)
{
    unsigned int _n=20; // number of iterations (precision)

    // compute x = r exp(j*theta)
    Q(_t) r, theta;
    Q(_atan2_cordic)(_x.imag, _x.real, &r, &theta, 10);

    // sqrt(x) = sqrt(r) * exp( j*theta/2 )

    // run CORDIC to compute sin|cos from angle
    Q(_t) qsin;
    Q(_t) qcos;
    Q(_sincos_cordic)(theta>>1, &qsin, &qcos, _n);

    // compute sqrt(r)
    Q(_t) mag = Q(_sqrt_newton)(r, _n);

    // compute output
    CQ(_t) y;
    y.real = Q(_mul)(mag,qcos);
    y.imag = Q(_mul)(mag,qsin);
    
    return y;
}

