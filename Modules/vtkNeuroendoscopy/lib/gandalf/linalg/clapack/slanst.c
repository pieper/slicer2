/**************************************************************************
*
* File:          $RCSfile: slanst.c,v $
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
#include <gandalf/linalg/clapack/slanst.h>
#include <gandalf/linalg/clapack/slassq.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
float gan_slanst(Gan_LapackNormType norm, long n, float *d, float *e)
{
   float ret_val, d__3, d__4, d__5;
   long i__;
   float scale;
   float anorm=0.0F;
   float sum;

   /* Function Body */
   if (n <= 0) {
      anorm = 0.;
   }
   else if (norm == GAN_NORMTYPE_MAXABSVAL)
   {
      /*        Find max(abs(A(i,j))). */
      anorm = fabs(d[n-1]);
      for (i__ = 0; i__ < n-1; ++i__)
      {
         /* Computing MAX */
         anorm = gan_max2(anorm, fabs(d[i__]));

         /* Computing MAX */
         anorm = gan_max2_d(anorm, fabs(e[i__]));
      }
   }
   else if (norm == GAN_NORMTYPE_ONE || norm == GAN_NORMTYPE_INFINITY)
   {
      /*        Find norm1(A). */
      if (n == 1)
         anorm = fabs(d[0]);
      else
      {
         /* Computing MAX */
         d__3 = abs(d[0]) + abs(e[0]);
         d__4 = fabs(e[n-2]) + fabs(d[n-1]);
         anorm = gan_max2(d__3,d__4);
         for (i__ = 1; i__ < n-1; ++i__)
         {
            /* Computing MAX */
            d__4 = anorm;
            d__5 = fabs(d[i__]) + fabs(e[i__]) + fabs(e[i__-1]);
            anorm = gan_max2(d__4,d__5);
         }
      }
   }
   else if (norm == GAN_NORMTYPE_FROBENIUS)
   {
      /*        Find normF(A). */
      scale = 0.0F;
      sum = 1.0F;
      if (n > 1)
      {
         gan_slassq(n-1, e, 1, &scale, &sum);
         sum *= 2;
      }

      gan_slassq(n, d, 1, &scale, &sum);
      anorm = scale * sqrt(sum);
   }

   ret_val = anorm;
   return ret_val;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */


