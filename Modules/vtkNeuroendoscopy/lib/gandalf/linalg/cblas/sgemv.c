/**************************************************************************
*
* File:          $RCSfile: sgemv.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:22 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/sgemv.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_sgemv ( Gan_TposeFlag trans, long m, long n, float alpha,
             float *a, long lda, float *x, long incx, 
             float beta, float *y, long incy )
{
   float total=0.0F;
   long i, j;

   gan_err_test_bool ( m >= 0 && n >= 0, "gan_sgemv",
                       GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   if ( trans )
      for ( j = n-1; j >= 0; j-- )
      {
         for ( i = m-1, total = 0.0; i >= 0; i-- )
            total += a[j*lda+i]*x[i*incx];

         y[j*incy] = alpha*total + beta*y[j*incy];
      }
   else /* !trans */
      for ( i = m-1; i >= 0; i-- )
      {
         for ( j = n-1, total = 0.0; j >= 0; j-- )
            total += a[j*lda+i]*x[j*incx];

         y[i*incy] = alpha*total + beta*y[i*incy];
      }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
