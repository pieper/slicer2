/**************************************************************************
*
* File:          $RCSfile: sgetri.c,v $
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
#include <gandalf/linalg/clapack/sgetri.h>
#include <gandalf/linalg/clapack/strtri.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_sgetri ( long n, float *a, long lda, long *ipiv,
              float *work, long lwork, long *info )
{
   long i, j, jp;
   
   if ( n < 0 )
   {
      *info = -1;
      return GAN_FALSE;
   }

   if ( lda < gan_max2_l(1,n) )
   {
      *info = -3;
      return GAN_FALSE;
   }
   
   if ( lwork < gan_max2_l(1,n) )
   {
      *info = -6;
      return GAN_FALSE;
   }   

   *info = 0;

   /* invert upper triangular factor */
   if ( !gan_strtri ( GAN_MATRIXPART_UPPER, GAN_NOUNIT, n, a, lda, info ) )
      return GAN_FALSE;
    
   for ( j = n-1; j >= 0; j-- )
   {
      for ( i = j + 1; i < n; i++ )
      {
         work[i] = a[j*lda+i];
         a[j*lda+i] = 0.0;
      }

      if ( j < n-1 )
         gan_sgemv ( GAN_NOTRANSPOSE, n, n-j-1, -1.0, &a[(j+1)*lda], lda,
                     &work[j+1], 1, 1.0, &a[j*lda], 1 );
   }

   /* apply column swaps */
   for ( j = n-2; j >= 0; j-- )
   {
      jp = ipiv[j];
      if ( jp != j )
         gan_sswap ( n, &a[j*lda], 1, &a[jp*lda], 1 );
   }
   
   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
