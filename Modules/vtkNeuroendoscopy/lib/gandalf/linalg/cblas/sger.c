/**************************************************************************
*
* File:          $RCSfile: sger.c,v $
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
#include <gandalf/linalg/cblas/sger.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_sger ( long m, long n, float alpha, 
            float *x, long incx, float *y, long incy, 
            float *a, long lda )
{
   long i, j;

   gan_err_test_bool ( m >= 0 && n >= 0, "gan_sger",
                       GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   if ( alpha == 1.0 )
      for ( j = n-1; j >= 0; j-- )
         for ( i = m-1; i >= 0; i-- )
            a[j*lda+i] += x[i*incx]*y[j*incy];
   else
      for ( j = n-1; j >= 0; j-- )
         for ( i = m-1; i >= 0; i-- )
            a[j*lda+i] += alpha*x[i*incx]*y[j*incy];

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
