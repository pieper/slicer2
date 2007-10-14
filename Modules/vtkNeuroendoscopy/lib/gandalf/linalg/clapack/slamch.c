/**************************************************************************
*
* File:          $RCSfile: slamch.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:49 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/slamch.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
float gan_slamch(Gan_LapackLamchFlag cmach)
{
   float ret_val;
   float base;
   long beta;
   float emin, prec, emax;
   long imin, imax;
   Gan_Bool lrnd;
   float rmin, rmax, t;
   extern Gan_Bool lsame_(char *, char *);
   float small, sfmin;
   long it;
   float rnd, eps;

   gan_slamc2(&beta, &it, &lrnd, &eps, &imin, &rmin, &imax, &rmax);
   base = (float) beta;
   t = (float) it;
   if (lrnd)
   {
      rnd = 1.0F;
      eps = pow(beta, 1-it)*0.5F;
   }
   else
   {
      rnd = 0.0F;
      eps = pow(base, 1-it);
   }

   prec = eps*base;
   emin = (float) imin;
   emax = (float) imax;
   sfmin = rmin;
   small = 1.0F / rmax;
   if (small >= sfmin)
   {
      /*           Use SMALL plus a bit, to avoid the possibility of rounding
                   causing overflow when computing  1/sfmin. */
      sfmin = small * (eps + 1.);
   }

   switch(cmach)
   {
      case GAN_LAMCH_EPS: ret_val = eps; break;
      case GAN_LAMCH_SFMIN: ret_val = sfmin; break;
      case GAN_LAMCH_BASE: ret_val = base; break;
      case GAN_LAMCH_PREC: ret_val = prec; break;
      case GAN_LAMCH_T: ret_val = t; break;
      case GAN_LAMCH_RND: ret_val = rnd; break;
      case GAN_LAMCH_EMIN: ret_val = emin; break;
      case GAN_LAMCH_RMIN: ret_val = rmin; break;
      case GAN_LAMCH_EMAX: ret_val = emax; break;
      case GAN_LAMCH_RMAX: ret_val = rmax; break;
      default:
        return DBL_MAX;
   }

   return ret_val;
}

Gan_Bool gan_slamc1(long *beta, long *t, Gan_Bool *rnd, Gan_Bool *ieee1)
{
   /* Local variables */
   Gan_Bool lrnd;
   float a, b, c, f;
   long lbeta;
   float savec;
   Gan_Bool lieee1;
   float t1, t2;
   long lt;

   a = 1.0F;
   c = 1.0F;

    while (c == 1.0F)
   {
        a *= 2.0F;
        c = gan_slamc3(a, 1);
        c = gan_slamc3(c, -a);
    }

   /* Now compute  b = 2.0F**m  with the smallest positive long m such that   
      fl( a + b ) .gt. a. */
    b = 1.0F;
    c = gan_slamc3(a, b);
    while (c == a)
   {
        b *= 2;
        c = gan_slamc3(a, b);
    }

   /* Now compute the base.  a and c  are neighbouring floating point   
      numbers  in the  interval  ( beta**t, beta**( t + 1 ) )  and so   
      their difference is beta. Adding 0.25 to c is to ensure that it   
      is truncated to beta and not ( beta - 1 ). */
    savec = c;
    c = gan_slamc3(c, -a);
    lbeta = (long) (c + 0.25);

   /* Now determine whether rounding or chopping occurs,  by adding a   
      bit  less  than  beta/2  and a  bit  more  than  beta/2  to  a. */
    b = (float) lbeta;
    f = gan_slamc3(0.5F*b, -0.01F*b);
    c = gan_slamc3(f, a);
    if (c == a)
        lrnd = GAN_TRUE;
   else
        lrnd = GAN_FALSE;

    f = gan_slamc3(0.5F*b, 0.01F*b);
    c = gan_slamc3(f, a);
    if (lrnd && c == a)
      lrnd = GAN_FALSE;

   /*        Try and decide whether rounding is done in the  IEEE  'round to   
             nearest' style. B/2 is half a unit in the last place of the two   
             numbers A and SAVEC. Furthermore, A is even, i.e. has last  bit   
             zero, and SAVEC is odd. Thus adding B/2 to A should not  change   
             A, but adding B/2 to SAVEC should change SAVEC. */
    t1 = gan_slamc3(0.5F*b, a);
    t2 = gan_slamc3(0.5F*b, savec);
    lieee1 = ((t1 == a) && (t2 > savec) && lrnd);

   /*        Now find  the  mantissa, t.  It should  be the  long part of   
             log to the base beta of a,  however it is safer to determine t   
             by powering.  So we find t as the smallest positive long for   
             which   

             fl( beta**t + 1.0F ) = 1.0F. */
    lt = 0;
    a = 1.0F;
    c = 1.0F;

    while (c == 1)
   {
        ++lt;
        a *= lbeta;
        c = gan_slamc3(a, 1.0F);
        c = gan_slamc3(c, -a);
    }

   *beta = lbeta;
   *t = lt;
   *rnd = lrnd;
   *ieee1 = lieee1;
   return GAN_TRUE;
}

Gan_Bool gan_slamc2(long *beta, long *t, Gan_Bool *rnd, 
                    float *eps, long *emin, float *rmin, long *emax, 
                    float *rmax)
{
   /* Initialized data */
   Gan_Bool first = GAN_TRUE;
   Gan_Bool iwarn = GAN_FALSE;
   /* System generated locals */
   long i__1;
   float d__1, d__2, d__3, d__4, d__5;
   /* Local variables */
   Gan_Bool ieee;
   Gan_Bool lrnd;
   float leps, zero, a, b, c;
   long i, lbeta;
   float rbase;
   long lemin, lemax, gnmin;
   float small;
   long gpmin;
   float third, lrmin, lrmax, sixth;
   Gan_Bool lieee1;
   long lt, ngnmin, ngpmin;
   float one, two;



   if (first)
   {
      first = GAN_FALSE;
      zero = 0.0F;
      one = 1.0F;
      two = 2.0F;

      /*        LBETA, LT, LRND, LEPS, LEMIN and LRMIN  are the local values
                of   
                BETA, T, RND, EPS, EMIN and RMIN.   

                Throughout this routine  we use the function  SLAMC3  to ens
                ure   
                that relevant values are stored  and not held in registers, 
                or   
                are not affected by optimizers.   

                SLAMC1 returns the parameters  LBETA, LT, LRND and LIEEE1. 
      */

      gan_slamc1(&lbeta, &lt, &lrnd, &lieee1);

      /*        Start to find EPS. */

      b = (float) lbeta;
      a = pow(b, -lt);
      leps = a;

      /*        Try some tricks to see whether or not this is the correct  E
                PS. */
      b = 2.0F/3.0F;
      sixth = gan_slamc3(b, -0.5F);
      third = gan_slamc3(sixth, sixth);
      b = gan_slamc3(third, -0.5F);
      b = gan_slamc3(b, sixth);
      b = fabs(b);
      if (b < leps)
         b = leps;

      leps = 1.0F;

      while (leps > b && b > zero)
      {
         leps = b;
         d__1 = 0.5F * leps;
         /* Computing 5th power */
         d__3 = 2.0F, d__4 = d__3, d__3 *= d__3;
         /* Computing 2nd power */
         d__5 = leps;
         d__2 = d__4 * (d__3 * d__3) * (d__5 * d__5);
         c = gan_slamc3(d__1, d__2);
         c = gan_slamc3(0.5F, -c);
         b = gan_slamc3(0.5F, c);
         c = gan_slamc3(0.5F, -b);
         b = gan_slamc3(0.5F, c);
      }

      if (a < leps)
         leps = a;

      /*        Computation of EPS complete.   

      Now find  EMIN.  Let A = + or - 1, and + or - (1 + BASE**(-3
      )).   
      Keep dividing  A by BETA until (gradual) underflow occurs. T
      his   
      is detected when we cannot recover the previous A. */

      rbase = 1.0F/lbeta;
      small = 1;
      for (i = 0; i < 3; ++i)
         small = gan_slamc3(small * rbase, 0.0F);

      a = gan_slamc3(1, small);
      gan_slamc4(&ngpmin, 1, lbeta);
      gan_slamc4(&ngnmin, -1, lbeta);
      gan_slamc4(&gpmin, a, lbeta);
      gan_slamc4(&gnmin, -a, lbeta);
      ieee = GAN_FALSE;

      if (ngpmin == ngnmin && gpmin == gnmin)
      {
         if (ngpmin == gpmin)
         {
            lemin = ngpmin;
            /*            ( Non twos-complement machines, no gradual under
                          flow;   
                          e.g.,  VAX ) */
         }
         else if (gpmin - ngpmin == 3)
         {
            lemin = ngpmin - 1 + lt;
            ieee = GAN_TRUE;
            /*            ( Non twos-complement machines, with gradual und
                          erflow;   
                          e.g., IEEE standard followers ) */
         }
         else
         {
            lemin = gan_min2(ngpmin,gpmin);
            /*            ( A guess; no known machine ) */
            iwarn = GAN_TRUE;
         }
      }
      else if (ngpmin == gpmin && ngnmin == gnmin)
      {
         if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1)
         {
            lemin = gan_max2(ngpmin,ngnmin);
            /*            ( Twos-complement machines, no gradual underflow
                          ;   
                          e.g., CYBER 205 ) */
         }
         else
         {
            lemin = gan_min2(ngpmin,ngnmin);
            /*            ( A guess; no known machine ) */
            iwarn = GAN_TRUE;
         }

      }
      else if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1 && gpmin == gnmin)
      {
         if (gpmin - gan_min2(ngpmin,ngnmin) == 3)
         {
            lemin = gan_max2(ngpmin,ngnmin) - 1 + lt;
            /*            ( Twos-complement machines with gradual underflo
                          w;   
                          no known machine ) */
         }
         else
         {
            lemin = gan_min2(ngpmin,ngnmin);
            /*            ( A guess; no known machine ) */
            iwarn = GAN_TRUE;
         }

      }
      else
      {
         /* Computing MIN */
         lemin = gan_min4(ngpmin,ngnmin, gpmin, gnmin);
         /*         ( A guess; no known machine ) */
         iwarn = GAN_TRUE;
      }
      /* **   
         Comment out this if block if EMIN is ok */
      if (iwarn)
      {
         first = GAN_TRUE;
         printf("\n\n WARNING. The value EMIN may be incorrect:- ");
         printf("EMIN = %8li\n",lemin);
         printf("If, after inspection, the value EMIN looks acceptable");
         printf("please comment out \n the IF block as marked within the"); 
         printf("code of routine SLAMC2, \n otherwise supply EMIN"); 
         printf("explicitly.\n");
      }
      /* **   

      Assume IEEE arithmetic if we found denormalised  numbers abo
      ve,   
      or if arithmetic seems to round in the  IEEE style,  determi
      ned   
      in routine SLAMC1. A true IEEE machine should have both  thi
      ngs   
      true; however, faulty machines may have one or the other. */

      ieee = ieee || lieee1;

      /*        Compute  RMIN by successive division by  BETA. We could comp
                ute   
                RMIN as BASE**( EMIN - 1 ),  but some machines underflow dur
                ing   
                this computation. */

      lrmin = 1.;
      i__1 = 1 - lemin;
      for (i = 1; i <= 1-lemin; ++i)
         lrmin = gan_slamc3(lrmin * rbase, 0.0F);

      /*        Finally, call SLAMC5 to compute EMAX and RMAX. */
      gan_slamc5(lbeta, lt, lemin, ieee, &lemax, &lrmax);
   }

   *beta = lbeta;
   *t = lt;
   *rnd = lrnd;
   *eps = leps;
   *emin = lemin;
   *rmin = lrmin;
   *emax = lemax;
   *rmax = lrmax;

   return GAN_TRUE;
}

float gan_slamc3(float a, float b)
{
   return (a+b);
}

Gan_Bool gan_slamc4(long *emin, float start, long base)
{
   float a;
   long i;
   float rbase, b1, b2, c1, c2, d1, d2;
   float one;

   a = start;
   one = 1.;
   rbase = 1.0F/(float)base;
   *emin = 1;
   b1 = gan_slamc3(a*rbase, 0.0F);
   c1 = a;
   c2 = a;
   d1 = a;
   d2 = a;
   while((c1 == a) && (c2 == a) && (d1 == a) && (d2 == a))
   {
      --(*emin);
      a = b1;
      b1 = gan_slamc3(a/base, 0.0F);
      c1 = gan_slamc3(b1*base, 0.0F);
      d1 = 0.0F;
      for (i = 1; i <= base; ++i)
         d1 += b1;

      b2 = gan_slamc3(a*rbase, 0.0F);
      c2 = gan_slamc3(b2/rbase, 0.0F);
      d2 = 0.0F;
      for (i = 0; i < base; ++i)
         d2 += b2;
   }

   return GAN_TRUE;
}

Gan_Bool gan_slamc5(long beta, long p, long emin, Gan_Bool ieee,
                    long *emax, float *rmax)
{
   /* Local variables */
   long lexp;
   float oldy=0.0F;
   long uexp, i;
   float y, z;
   long nbits;
   float recbas;
   long exbits, expsum;
   long try__;

   lexp = 1;
   exbits = 1;
   for(;;)
   {
      try__ = lexp << 1;
      if (try__ > -emin)
         break;

      lexp = try__;
      ++exbits;
   }

   if (lexp == -emin)
      uexp = lexp;
   else
   {
      uexp = try__;
      ++exbits;
   }

   /*     Now -LEXP is less than or equal to EMIN, and -UEXP is greater   
          than or equal to EMIN. EXBITS is the number of bits needed to   
          store the exponent. */

   if (uexp + emin > -lexp - emin)
      expsum = lexp << 1;
   else
      expsum = uexp << 1;

   /*     EXPSUM is the exponent range, approximately equal to   
          EMAX - EMIN + 1 . */

   *emax = expsum + emin - 1;
   nbits = exbits + 1 + p;

   /*     NBITS is the total number of bits needed to store a   
          floating-point number. */

   if (nbits % 2 == 1 && beta == 2)
   {
      /*        Either there are an odd number of bits used to store a   
                floating-point number, which is unlikely, or some bits are 
  
                not used in the representation of numbers, which is possible
                ,   
                (e.g. Cray machines) or the mantissa has an implicit bit,   
                (e.g. IEEE machines, Dec Vax machines), which is perhaps the
   
                most likely. We have to assume the last alternative.   
                If this is true, then we need to reduce EMAX by one because 
  
                there must be some way of representing zero in an implicit-b
                it   
                system. On machines like Cray, we are reducing EMAX by one 
  
                unnecessarily. */

      --(*emax);
   }

   if (ieee)
   {
      /*        Assume we are on an IEEE machine which reserves one exponent
   
      for infinity and NaN. */
      --(*emax);
   }

   /*     Now create RMAX, the largest machine number, which should   
          be equal to (1.0F - BETA**(-P)) * BETA**EMAX .   

          First compute 1.0F - BETA**(-P), being careful that the   
          result is less than 1.0F . */

   recbas = 1.0F/beta;
   z = beta - 1.0F;
   y = 0.0F;
   for (i=0; i<p; ++i)
   {
      z *= recbas;
      if (y < 1.0F)
         oldy = y;

      y = gan_slamc3(y, z);
   }

   if (y >= 1.0F)
      y = oldy;

   /*     Now multiply by BETA**EMAX to get RMAX. */
   for (i = 1; i <= *emax; ++i)
      y = gan_slamc3(y*beta, 0.0F);

   *rmax = y;
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
