/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
//  -*- Mode: C++;  -*-
//  File: qgauss.hh
//  Author: Sandy Wells (sw@ai.mit.edu)
//  Copyright (C) MIT Artificial Intelligence Laboratory, 1995
// *------------------------------------------------------------------
// * FUNCTION:
// *
// * Implements an approximation to the Gaussian function.
// * It is based on a piecewise-linear approximation
// * to the 2**x function for negative arguments using integer arithmetic and
// * bit fiddling.  
// * origins: May 14, 1995, sw.  
// *
// * On an alpha qgauss is about 3 times faster than vanilla gaussian.
// * The error seems to be a six percent ripple.
// *
// * HISTORY:
// * Last edited: Nov  3 15:26 1995 (sw)
// * Created: Wed Jun  7 02:03:35 1995 (sw)
// *------------------------------------------------------------------

#define ONE_OVER_ROOT_2_PI (0.3989422f)
#define MINUS_HALF (-0.5f)

// Summary:
// float qnexp2(float x);
// float qgauss(float inverse_sigma, float x);

//
// Abuse the type system.

#define COERCE(x, type) (*((type *)(&(x))))


// Some constants having to do with the way single
// floats are represented on alphas and sparcs

#define MANTSIZE (23)
#define SIGNBIT (1 << 31)
#define EXPMASK (255 << MANTSIZE)
#define MANTMASK ((~EXPMASK)&(~SIGNBIT))
#define PHANTOM_BIT (1 << MANTSIZE)
#define EXPBIAS 127
#define SHIFTED_BIAS (EXPBIAS << MANTSIZE)
#define SHIFTED_BIAS_COMP ((~ SHIFTED_BIAS) + 1)

// A piecewise linear approximation to 2**x for negative arugments
// Provides exact results when the argument is a power of two,
// and some other times as well.
// The strategy is rougly as follows:
//    coerce the single float argument to unsigned int
//    extract the exponent as a signed integer
//    construct the mantissa, including the phantom high bit, and negate it
//    construct the result bit pattern by leftshifting the signed mantissa
//      this is done for both cases of the exponent sign
//      and check for potenital underflow

// Does no conditional branching on alpha or sparc :Jun  7, 1995

inline float qnexp2(float x)
{
    unsigned result_bits;
    unsigned bits = COERCE(x, unsigned int);
    int exponent = ((EXPMASK & bits) >> MANTSIZE) - (EXPBIAS);
    int neg_mant =   - (int)((MANTMASK & bits) | PHANTOM_BIT);

    unsigned r1 = (neg_mant << exponent);
    unsigned r2 = (neg_mant >> (- exponent));

    result_bits = (exponent < 0) ? r2 : r1;
    result_bits = (exponent > 5) ? SHIFTED_BIAS_COMP  : result_bits;
    
    result_bits += SHIFTED_BIAS;

#ifdef DEBUG
    {
    float result;
    result = COERCE(result_bits, float);
    fprintf(stderr, "x %g, b %x, e %d, m %x, R %g =?",
           x,     bits, exponent,  neg_mant, pow(2.0, x));
    fflush(stderr);
    fprintf(stderr, " %g\n", result);
    }
#endif

    return(COERCE(result_bits, float));
}



#define MINUS_ONE_OVER_2_LOG_2 ((float) -.72134752)

// An approximation to the Gaussian function.
// The error seems to be a six percent ripple.

inline float qgauss(float inverse_sigma, float x)
{
    float tmp = inverse_sigma * x;
    return ONE_OVER_ROOT_2_PI * inverse_sigma 
    * qnexp2(MINUS_ONE_OVER_2_LOG_2 * tmp * tmp);
}




// Similar to above.  This one computes an
// approximation to the Gaussian of the square
// root of the argument, in other words, the
// argument should already be squared.

inline float qgauss_sqrt(float inverse_sigma, float x)
{
    return ONE_OVER_ROOT_2_PI * inverse_sigma 
    * qnexp2(MINUS_ONE_OVER_2_LOG_2 * inverse_sigma * inverse_sigma * x);
}

//
// ADDED BY DAVE FOR COMPARISON:
//

inline float gauss(float inverse_sigma, float x)
{
  return ONE_OVER_ROOT_2_PI * inverse_sigma * (float)exp(MINUS_HALF*x*x*inverse_sigma);
}
