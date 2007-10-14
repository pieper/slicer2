/**************************************************************************
*
* File:          $RCSfile: slartg.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:50 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/slartg.h>
#include <gandalf/linalg/clapack/slamch.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_slartg(float f, float g, float *cs, float *sn, float *r)
{
   long i__1;
   float d__1, d__2;
   long i__;
   float scale;
   long count;
   float f1, g1, safmn2, safmx2;
   float safmin, eps;

    safmin = gan_slamch(GAN_LAMCH_SFMIN);
    eps = gan_slamch(GAN_LAMCH_EPS);
    d__1 = gan_slamch(GAN_LAMCH_BASE);
    i__1 = (long) (log(safmin / eps) / log(gan_slamch(GAN_LAMCH_BASE)) / 2.0F);
    safmn2 = pow(d__1, i__1);
    safmx2 = 1.0F / safmn2;

   if (g == 0.0F)
   {
      *cs = 1.0F;
      *sn = 0.0F;
      *r = f;
   }
   else if (f == 0.)
   {
      *cs = 0.;
      *sn = 1.;
      *r = g;
   }
   else
   {
      f1 = f;
      g1 = g;
      /* Computing MAX */
      d__1 = fabs(f1), d__2 = fabs(g1);
      scale = gan_max2(d__1,d__2);
      if (scale >= safmx2)
      {
         count = 0;
L10:
         ++count;
         f1 *= safmn2;
         g1 *= safmn2;

         /* Computing MAX */
         d__1 = fabs(f1), d__2 = fabs(g1);
         scale = gan_max2(d__1,d__2);
         if (scale >= safmx2)
            goto L10;

         /* Computing 2nd power */
         d__1 = f1;
         /* Computing 2nd power */
         d__2 = g1;
         *r = sqrt(d__1 * d__1 + d__2 * d__2);
         *cs = f1 / *r;
         *sn = g1 / *r;
         i__1 = count;
         for (i__ = 1; i__ <= i__1; ++i__)
            *r *= safmx2;
      }
      else if (scale <= safmn2)
      {
         count = 0;
L30:
         ++count;
         f1 *= safmx2;
         g1 *= safmx2;
         /* Computing MAX */
         d__1 = fabs(f1), d__2 = fabs(g1);
         scale = gan_max2(d__1,d__2);
         if (scale <= safmn2)
            goto L30;

         /* Computing 2nd power */
         d__1 = f1;
         /* Computing 2nd power */
         d__2 = g1;
         *r = sqrt(d__1 * d__1 + d__2 * d__2);
         *cs = f1 / *r;
         *sn = g1 / *r;
         i__1 = count;
         for (i__ = 1; i__ <= i__1; ++i__)
            *r *= safmn2;
      }
      else
      {
         /* Computing 2nd power */
         d__1 = f1;

         /* Computing 2nd power */
         d__2 = g1;
         *r = sqrt(d__1 * d__1 + d__2 * d__2);
         *cs = f1 / *r;
         *sn = g1 / *r;
      }

      if (fabs(f) > fabs(g) && *cs < 0.0F)
      {
         *cs = -(*cs);
         *sn = -(*sn);
         *r = -(*r);
      }
   }

   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
