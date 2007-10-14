/**************************************************************************
*
* File:          $RCSfile: dgetrs.c,v $
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
#include <gandalf/linalg/clapack/dgetrs.h>
#include <gandalf/linalg/clapack/dlaswp.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_dgetrs ( Gan_TposeFlag trans, long n, long nrhs, 
              double *a, long lda, long *ipiv, double *b, long ldb,
              long *info )
{
   *info = 0;
   if (n < 0) *info = -2;
   else if (nrhs < 0) *info = -3;
   else if (lda < gan_max2(1,n)) *info = -5;
   else if (ldb < gan_max2(1,n)) *info = -8;
   if (*info != 0) return GAN_FALSE;

   /* Quick return if possible */
   if (n == 0 || nrhs == 0) return GAN_TRUE;

   if ( trans == GAN_NOTRANSPOSE )
   {
      /* Solve A * X = B. */

      /* Apply row interchanges to the right hand sides. */
      gan_dlaswp ( nrhs, b, ldb, 0, n-1, ipiv, 1 );

      /* Solve L*X = B, overwriting B with X. */
      gan_dtrsm ( GAN_LEFTMULT, GAN_MATRIXPART_LOWER, GAN_NOTRANSPOSE, GAN_UNIT,
                  n, nrhs, 1.0, a, lda, b, ldb );

      /* Solve U*X = B, overwriting B with X. */
      gan_dtrsm ( GAN_LEFTMULT, GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT,
                  n, nrhs, 1.0, a, lda, b, ldb );
   }
   else
   {
      /* Solve A' * X = B. */

      /* Solve U'*X = B, overwriting B with X. */
      gan_dtrsm ( GAN_LEFTMULT, GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT,
                  n, nrhs, 1.0, a, lda, b, ldb );

      /* Solve L'*X = B, overwriting B with X. */
      gan_dtrsm ( GAN_LEFTMULT, GAN_MATRIXPART_LOWER, GAN_TRANSPOSE, GAN_UNIT,
                  n, nrhs, 1.0, a, lda, b, ldb );

      /* Apply row interchanges to the solution vectors. */
      gan_dlaswp ( nrhs, b, ldb, 0, n-1, ipiv, -1 );
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */


