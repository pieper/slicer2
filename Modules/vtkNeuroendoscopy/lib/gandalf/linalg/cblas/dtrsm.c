/**************************************************************************
*
* File:          $RCSfile: dtrsm.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:21 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/dtrsm.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_dtrsm ( Gan_LapackSideFlag side, Gan_LapackUpLoFlag uplo,
             Gan_TposeFlag transa, Gan_LapackUnitFlag diag,
             long m, long n, double alpha, double *a,
             long lda, double *b, long ldb )
{
   long i, j, k, nrowa;
   double temp;

#define A(I,J) a[(I) + (J)* (lda)]
#define B(I,J) b[(I) + (J)* (ldb)]

   if ( side == GAN_LEFTMULT )
      nrowa = m;
   else /* side == GAN_RIGHTMULT */
      nrowa = n;

   gan_err_test_bool ( m >= 0 && n >= 0, "gan_dtrsm",
                       GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   gan_err_test_bool ( lda >= gan_max2(1,nrowa) && ldb >= gan_max2(1,m),
                       "gan_dtrsm", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   /* Quick return if possible. */
   if (n == 0) return 0;

   /* And when  alpha.eq.zero. */
   if (alpha == 0.0)
   {
      for (j = 0; j < n; ++j)
         for (i = 0; i < m; ++i)
            B(i,j) = 0.0;

      return GAN_TRUE;
    }

   /* Start the operations. */
   if ( side == GAN_LEFTMULT )
   {
      if ( transa == GAN_NOTRANSPOSE )
      {
         /* Form  B := alpha*inv( A )*B. */
         if ( uplo == GAN_MATRIXPART_UPPER )
            for (j = 0; j < n; ++j)
            {
               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,j) = alpha * B(i,j);

               for (k = m-1; k >= 0; --k) {
                  if (B(k,j) != 0.0)
                  {
                     if (diag == GAN_NOUNIT) B(k,j) /= A(k,k);
                     for (i = 0; i <= k-1; ++i) B(i,j) -= B(k,j) * A(i,k);
                  }
               }
            }
         else
            for (j = 0; j < n; ++j)
            {
               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,j) = alpha * B(i,j);

               for (k = 0; k < m; ++k)
                  if (B(k,j) != 0.)
                  {
                     if (diag == GAN_NOUNIT) B(k,j) /= A(k,k);
                     for (i = k+1; i < m; ++i) B(i,j) -= B(k,j) * A(i,k);
                  }
            }
      }
      else
      {
         /* Form  B := alpha*inv( A' )*B. */
         if (uplo == GAN_MATRIXPART_UPPER)
            for (j = 0; j < n; ++j)
               for (i = 0; i < m; ++i)
               {
                  temp = alpha * B(i,j);
                  for (k = 0; k <= i-1; ++k) temp -= A(k,i) * B(k,j);
                  if (diag == GAN_NOUNIT) temp /= A(i,i);
                  B(i,j) = temp;
               }
         else
            for (j = 0; j < n; ++j)
               for (i = m-1; i >= 0; --i)
               {
                  temp = alpha * B(i,j);
                  for (k = i+1; k < m; ++k) temp -= A(k,i) * B(k,j);
                  if (diag == GAN_NOUNIT) temp /= A(i,i);
                  B(i,j) = temp;
               }
      }
   }
   else
   {
      if ( transa == GAN_NOTRANSPOSE )
      {
         /* Form  B := alpha*B*inv( A ). */
         if ( uplo == GAN_MATRIXPART_UPPER )
            for (j = 0; j < n; ++j)
            {
               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,j) = alpha * B(i,j);

               for (k = 0; k <= j-1; ++k)
                  if (A(k,j) != 0.0)
                     for (i = 0; i < m; ++i) B(i,j) -= A(k,j) * B(i,k);

               if ( diag == GAN_NOUNIT )
               {
                  temp = 1.0 / A(j,j);
                  for (i = 0; i < m; ++i) B(i,j) = temp * B(i,j);
               }
            }
         else
            for (j = n-1; j >= 0; --j)
            {
               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,j) = alpha * B(i,j);

               for (k = j+1; k < n; ++k)
                  if (A(k,j) != 0.0)
                     for (i = 0; i < m; ++i) B(i,j) -= A(k,j) * B(i,k);

               if ( diag == GAN_NOUNIT )
               {
                  temp = 1.0 / A(j,j);
                  for (i = 0; i < m; ++i) B(i,j) = temp * B(i,j);
               }
            }
      }
      else
      {
         /* Form  B := alpha*B*inv( A' ). */
         if ( uplo == GAN_MATRIXPART_UPPER )
            for (k = n-1; k >= 0; --k)
            {
               if ( diag == GAN_NOUNIT )
               {
                  temp = 1.0 / A(k,k);
                  for (i = 0; i < m; ++i) B(i,k) = temp * B(i,k);
               }

               for (j = 0; j <= k-1; ++j)
                  if (A(j,k) != 0.0)
                  {
                     temp = A(j,k);
                     for (i = 0; i < m; ++i) B(i,j) -= temp * B(i,k);
                  }

               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,k) = alpha * B(i,k);
            }
         else
            for (k = 0; k < n; ++k)
            {
               if ( diag == GAN_NOUNIT )
               {
                  temp = 1.0 / A(k,k);
                  for (i = 0; i < m; ++i) B(i,k) = temp * B(i,k);
               }

               for (j = k+1; j < n; ++j)
                  if (A(j,k) != 0.0)
                  {
                     temp = A(j,k);
                     for (i = 0; i < m; ++i) B(i,j) -= temp * B(i,k);
                  }

               if (alpha != 1.0)
                  for (i = 0; i < m; ++i) B(i,k) = alpha * B(i,k);
            }
      }
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
