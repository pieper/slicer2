/**************************************************************************
*
* File:          $RCSfile: dpptri.c,v $
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
#include <gandalf/linalg/clapack/dpptri.h>
#include <gandalf/linalg/clapack/dtptri.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_dpptri ( Gan_LapackUpLoFlag upper, long n, double *ap, long *info )
{
   long i;
   double *app;

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

   /* compute inverse */
   if ( !gan_dtptri ( upper, GAN_NOUNIT, n, ap, info ) )
      return GAN_FALSE;

   /* recursively solve for rest of matrix */
   for ( i = 0, app = ap; i < n; app += ++i )
   {
      gan_dspr ( upper, i, 1.0, app, 1, ap );
      gan_dscal ( i, app[i], app, 1 );
      app[i] *= app[i];
   }
      
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
