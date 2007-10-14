/**************************************************************************
*
* File:          $RCSfile: spptrf.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:51 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/spptrf.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_spptrf ( Gan_LapackUpLoFlag upper, long n, float *ap, long *info )
{
   float *app, d;
   long i;

   /* lower triangular form not implemented */
   if ( !upper )
   {
      *info = -1;
      return GAN_FALSE;
   }

   /* size of matrix must be >= 0 */
   if ( n < 0 )
   {
      *info = -2;
      return GAN_FALSE;
   }
   
   /* return immediately for zero-size matrix */
   *info = 0;
   if ( n == 0 ) return GAN_TRUE;

   for ( i = 0, app = ap; i < n; app += ++i )
   {
      gan_stpsv ( upper, GAN_TRANSPOSE, GAN_NOUNIT, i, ap, app, 1 );
      d = app[i] - gan_sdot ( i, app, 1, app, 1 );
      if ( d < 0.0F )
      {
         app[i] = d;
         *info = i;
         return GAN_FALSE;
      }

      app[i] = (float)sqrt(d);
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
