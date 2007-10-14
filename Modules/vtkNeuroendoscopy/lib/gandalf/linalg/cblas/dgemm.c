/**************************************************************************
*
* File:          $RCSfile: dgemm.c,v $
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
#include <gandalf/linalg/cblas/dgemm.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_dgemm ( Gan_TposeFlag transa, Gan_TposeFlag transb,
             long m, long n, long k,
             double alpha, double *a, long lda, double *b, long ldb,
             double beta, double *c, long ldc )
{
   double total;
   long i, j, l;

   gan_err_test_bool ( m >= 0 && n >= 0 && k >= 0 && ldc >= gan_max2_l(1,m),
                       "gan_dgemm", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );
   
   /* return immediately if there is no data to process or the result is
      no change */
   if ( ((alpha == 0.0 || k == 0) && beta == 1.0) || m == 0 || n == 0 )
      return GAN_TRUE;

   /* read string transa to determine whether matrix a is transposed */
   gan_err_test_bool ( transa ? (lda >= gan_max2_l(1,k))
                              : (lda >= gan_max2_l(1,m)),
                       "gan_dgemm", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   /* read string transa to determine whether matrix b is transposed */
   gan_err_test_bool ( transb ? (ldb >= gan_max2_l(1,n))
                              : (ldb >= gan_max2_l(1,k)),
                       "gan_dgemm", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   if ( alpha == 0.0 )
   {
      double *cp;

      if ( beta == 0.0 )
         for ( i = n-1; i >= 0; i--, c += ldc ) 
            for ( j = m-1, cp = c; j >= 0; j-- ) *cp++ = 0.0;
      else
         for ( i = n-1; i >= 0; i--, c += ldc )
            for ( j = m-1, cp = c; j >= 0; j-- ) *cp++ *= beta;
      
      return GAN_TRUE;
   }

   if ( transa )
   {
      if ( transb )
      {
         if ( beta == 0.0 )
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = 0.0; l >= 0; l-- )
                     total += a[j*lda+l]*b[l*ldb+i];

                  c[j] = total;
               }
         else
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = beta*c[j]; l >= 0; l-- )
                     total += a[j*lda+l]*b[l*ldb+i];

                  c[j] = total;
               }
      }
      else /* !transb */
      {
         if ( beta == 0.0 )
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = 0.0; l >= 0; l-- )
                     total += a[j*lda+l]*b[i*ldb+l];

                  c[j] = total;
               }
         else
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = beta*c[j]; l >= 0; l-- )
                     total += a[j*lda+l]*b[i*ldb+l];

                  c[j] = total;
               }
      }
   }
   else /* !transa */
   {
      if ( transb )
      {
         if ( beta == 0.0 )
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = 0.0; l >= 0; l-- )
                     total += a[l*lda+j]*b[l*ldb+i];

                  c[j] = total;
               }
         else
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = beta*c[j]; l >= 0; l-- )
                     total += a[l*lda+j]*b[l*ldb+i];

                  c[j] = total;
               }
      }
      else /* !transb */
      {
         if ( beta == 0.0 )
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = 0.0; l >= 0; l-- )
                     total += a[l*lda+j]*b[i*ldb+l];

                  c[j] = total;
               }
         else
            for ( i = n-1, c += i*ldc; i >= 0; i--, c -= ldc )
               for ( j = m-1; j >= 0; j-- )
               {
                  for ( l = k-1, total = beta*c[j]; l >= 0; l-- )
                     total += a[l*lda+j]*b[i*ldb+l];

                  c[j] = total;
               }
      }
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
