/**************************************************************************
*
* File:          $RCSfile: sspr.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:23 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/sspr.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_sspr ( Gan_LapackUpLoFlag upper, long n, float alpha, 
            float *x, long incx, float *ap )
{
   long i, j;

   gan_err_test_bool ( upper, "gan_sspr", GAN_ERROR_NOT_IMPLEMENTED, "" );

   if ( n == 0 ) return GAN_TRUE;
   if ( incx == 1 )
   {
      if ( alpha == 1.0 )
         for ( i = n-1; i >= 0; i-- )
            for ( j = n-1; j >= i; j-- )
               ap[j*(j+1)/2+i] += x[i]*x[j];
      else
         for ( i = n-1; i >= 0; i-- )
            for ( j = n-1; j >= i; j-- )
               ap[j*(j+1)/2+i] += alpha*x[i]*x[j];
   }
   else
   {
      if ( alpha == 1.0 )
         for ( i = n-1; i >= 0; i-- )
            for ( j = n-1; j >= i; j-- )
               ap[j*(j+1)/2+i] += x[i*incx]*x[j*incx];
      else
         for ( i = n-1; i >= 0; i-- )
            for ( j = n-1; j >= i; j-- )
               ap[j*(j+1)/2+i] += alpha*x[i*incx]*x[j*incx];
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

