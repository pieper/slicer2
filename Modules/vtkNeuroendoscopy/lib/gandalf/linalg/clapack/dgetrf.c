/**************************************************************************
*
* File:          $RCSfile: dgetrf.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:44 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/dgetrf.h>
#include <gandalf/linalg/clapack/dgetf2.h>
#include <gandalf/linalg/clapack/dlaswp.h>
#include <gandalf/linalg/clapack/ilaenv.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_dgetrf ( long m, long n, double *a, long lda, long *ipiv, long *info )
{
   long i, j, iinfo, jb, nb;

#define IPIV(I) ipiv[(I)]

#define A(I,J) a[(I) + (J)*(lda)]

   *info = 0;
   if (m < 0) *info = -1;
   else if (n < 0) *info = -2;
   else if (lda < gan_max2(1,m)) *info = -4;
   if (*info != 0) return GAN_FALSE;

   /* Quick return if possible */
   if (m == 0 || n == 0) return GAN_TRUE;

   /* Determine the block size for this environment. */
   nb = gan_ilaenv ( 1, "DGETRF", " ", m, n, -1, -1, 6L, 1L );
   if (nb <= 1 || nb >= gan_min2(m,n))
      /* Use unblocked code. */
      return gan_dgetf2 ( m, n, &A(0,0), lda, &IPIV(0), info );

   /* Use blocked code. */
   for (j = 0; nb < 0 ? j >= gan_min2(m,n)-1 : j < gan_min2(m,n); j += nb)
   {
      /* Computing MIN */
      jb = gan_min2_l(gan_min2(m,n)-j, nb);

      /* Factor diagonal and subdiagonal blocks and test for exact   
         singularity. */
      if ( !gan_dgetf2 ( m-j, jb, &A(j,j), lda, &IPIV(j), &iinfo ) &&
           iinfo > 0 )
         /*           Adjust INFO and the pivot indices. */
         *info = iinfo + j;

      /* Computing MIN */
      for (i = j; i < gan_min2(m,j+jb); ++i)
         IPIV(i) = j + IPIV(i);

      /* Apply interchanges to columns 1:J-1. */
      gan_dlaswp ( j, &A(0,0), lda, j, j+jb, &IPIV(0), 1 );

      if (j + jb < n)
      {
         /*  Apply interchanges to columns J+JB:N. */
         gan_dlaswp ( n-j-jb, &A(0,j+jb), lda, j, j+jb-1, &IPIV(0), 1 );

         /* Compute block row of U. */
         gan_dtrsm ( GAN_LEFTMULT, GAN_MATRIXPART_LOWER, GAN_NOTRANSPOSE, GAN_UNIT,
                     jb, n-j-jb, 1.0, &A(j,j), lda, &A(j,j+jb), lda );
         if (j + jb < m)
            /* Update trailing submatrix. */
            gan_dgemm ( GAN_NOTRANSPOSE, GAN_NOTRANSPOSE, m-j-jb, n-j-jb, jb, 
                        -1.0, &A(j+jb,j), lda, &A(j,j+jb), lda, 1.0,
                        &A(j+jb,j+jb), lda );
      }
    }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
