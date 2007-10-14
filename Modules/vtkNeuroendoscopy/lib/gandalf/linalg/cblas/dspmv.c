/**************************************************************************
*
* File:          $RCSfile: dspmv.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:20 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/dspmv.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_dspmv ( Gan_LapackUpLoFlag upper, long n, double alpha, 
             double *ap, double *x, long incx, double beta, 
             double *y, long incy )
{
   long i, j;

   gan_err_test_bool ( upper, "gan_dspmv", GAN_ERROR_NOT_IMPLEMENTED, "" );

   if ( n == 0 ) return GAN_TRUE;
   if ( incx == 1 && incy == 1 )
      for ( i = n-1; i >= 0; i-- )
      {
         y[i] *= beta;
         for ( j = n-1; j >= 0; j-- )
         if ( i < j ) y[i] += alpha*ap[j*(j+1)/2+i]*x[j];
         else         y[i] += alpha*ap[i*(i+1)/2+j]*x[j];
      }
   else
      for ( i = n-1; i >= 0; i-- )
      {
         y[i*incy] *= beta;
         for ( j = n-1; j >= 0; j-- )
         if ( i < j ) y[i*incy] += alpha*ap[j*(j+1)/2+i]*x[j*incx];
         else         y[i*incy] += alpha*ap[i*(i+1)/2+j]*x[j*incx];
      }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
