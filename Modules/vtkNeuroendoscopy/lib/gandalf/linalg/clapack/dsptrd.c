/**************************************************************************
*
* File:          $RCSfile: dsptrd.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:47 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/dsptrd.h>
#include <gandalf/linalg/clapack/dlarfg.h>
#include <gandalf/linalg/cblas/daxpy.h>
#include <gandalf/linalg/cblas/ddot.h>
#include <gandalf/linalg/cblas/dspr2.h>
#include <gandalf/linalg/cblas/dspmv.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_dsptrd(Gan_LapackUpLoFlag upper, long n, double *ap, 
                    double *d, double *e, double *tau, long *info)
{
   double taui;
   long i__;
   double alpha;
   long i1;
   long ii;
   long i1i1;

   /* Function Body */
   *info = 0;
   if (n < 0)
      *info = -2;

   if (*info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if (n <= 0)
      return GAN_TRUE;

   if (upper)
   {
      /*        Reduce the upper triangle of A.   
                I1 is the index in AP of A(1,I+1). */
      i1 = n*(n-1)/2;
      for (i__ = n-1; i__ >= 1; --i__)
      {
         /*           Generate elementary reflector H(i) = I - tau * v * v'   
                      to annihilate A(1:i-1,i+1) */
         gan_dlarfg(i__, &ap[i1+i__-1], &ap[i1], 1, &taui);
         e[i__-1] = ap[i1+i__-1];

         if (taui != 0.0)
         {
            /*              Apply H(i) from both sides to A(1:i,1:i) */
            ap[i1+i__-1] = 1.0;

            /*              Compute  y := tau * A * v  storing y in TAU(1:i) */
            gan_dspmv(upper, i__, taui, ap, &ap[i1], 1, 0.0, tau, 1);

            /*              Compute  w := y - 1/2 * tau * (y'*v) * v */
            alpha = taui * -0.5 * gan_ddot(i__, tau, 1, &ap[i1], 1);
            gan_daxpy(i__, alpha, &ap[i1], 1, tau, 1);

            /*              Apply the transformation as a rank-2 update:   
                            A := A - v * w' - w * v' */
            gan_dspr2(upper, i__, -1.0, &ap[i1], 1, tau, 1, ap);

            ap[i1+i__-1] = e[i__-1];
         }

         d[i__] = ap[i1+i__];
         tau[i__-1] = taui;
         i1 -= i__;
      }

      d[0] = ap[0];
   }
   else
   {
      /*        Reduce the lower triangle of A. II is the index in AP of   
                A(i,i) and I1I1 is the index of A(i+1,i+1). */
      ii = 1;
      for (i__ = 1; i__ <= n-1; ++i__)
      {
         i1i1 = ii+n-i__+1;

         /*           Generate elementary reflector H(i) = I - tau * v * v'   
                      to annihilate A(i+2:n,i) */
         gan_dlarfg(n-i__, &ap[ii], &ap[ii+1], 1, &taui);
         e[i__-1] = ap[ii];

         if (taui != 0.)
         {
            /*              Apply H(i) from both sides to A(i+1:n,i+1:n) */
            ap[ii] = 1.0;

            /*              Compute  y := tau * A * v  storing y in TAU(i:n-1) */
            gan_dspmv(upper, n-i__, taui, &ap[i1i1], &ap[ii], 1, 0.0,
                      &tau[i__-1], 1);

            /*              Compute  w := y - 1/2 * tau * (y'*v) * v */
            alpha = taui * -0.5 * gan_ddot(n-i__, &tau[i__-1], 1, &ap[ii], 1);
            gan_daxpy(n-i__, alpha, &ap[ii], 1, &tau[i__-1], 1);

            /*              Apply the transformation as a rank-2 update:   
                            A := A - v * w' - w * v' */
            gan_dspr2(upper, n-i__, -1.0, &ap[ii], 1, &tau[i__-1], 1, &ap[i1i1]);

            ap[ii] = e[i__-1];
         }

         d[i__-1] = ap[ii-1];
         tau[i__-1] = taui;
         ii = i1i1;
      }

      d[n-1] = ap[ii-1];
   }

   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
