/**************************************************************************
*
* File:          $RCSfile: sgesv.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:48 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/sgetrf.h>
#include <gandalf/linalg/clapack/sgetrs.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_sgesv ( long n, long nrhs, float *a, long lda, long *ipiv,
             float *b, long ldb, long *info )
{
#define IPIV(I) ipiv[(I)]

#define A(I,J) a[(I) + (J)*(lda)]
#define B(I,J) b[(I) + (J)*(ldb)]

   *info = 0;
   if (n < 0)                    *info = -1;
   else if (nrhs < 0)            *info = -2;
   else if (lda < gan_max2(1,n)) *info = -4;
   else if (ldb < gan_max2(1,n)) *info = -7;

   if (*info != 0) return GAN_FALSE;

   /* Compute the LU factorization of A. */
   gan_sgetrf ( n, n, &A(0,0), lda, &IPIV(0), info );
   if (*info != 0) return GAN_FALSE;

   /* Solve the system A*X = B, overwriting B with X. */
   gan_sgetrs ( GAN_NOTRANSPOSE, n, nrhs, &A(0,0), lda, &IPIV(0), &B(0,0),
                ldb, info );
   if (*info != 0) return GAN_FALSE;

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
