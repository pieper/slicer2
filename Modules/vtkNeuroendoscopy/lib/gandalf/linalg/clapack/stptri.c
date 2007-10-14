/**************************************************************************
*
* File:          $RCSfile: stptri.c,v $
* Module:        CLAPAC function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:52 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/stptri.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_stptri ( Gan_LapackUpLoFlag upper, Gan_LapackUnitFlag unit,
              long n, float *ap, long *info )
{
   long i;
   float *app;

   /* lower triangular form not implemented */
   if ( !upper )
   {
      *info = -1;
      return GAN_FALSE;
   }

   /* unit triangular form not implemented */
   if ( unit )
   {
      *info = -2;
      return GAN_FALSE;
   }

   /* size of matrix must be >= 0 */
   if ( n < 0 )
   {
      *info = -3;
      return GAN_FALSE;
   }

   /* recursively solve for inverse of matrix */
   *info = 0;
   for ( i = 0, app = ap; i < n; app += ++i )
   {
      if ( app[i] == 0.0 )
      {
         *info = i;
         return GAN_FALSE;
      }
      
      app[i] = 1.0F/app[i];
      gan_stpmv ( upper, GAN_NOTRANSPOSE, unit, i, ap, app, 1 );
      gan_sscal ( i, -app[i], app, 1 );
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
