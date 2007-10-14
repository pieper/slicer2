/**************************************************************************
*
* File:          $RCSfile: dorg2l.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:46 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/dorg2l.h>
#include <gandalf/linalg/clapack/dlarf.h>
#include <gandalf/linalg/clapack/dlarfg.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_dorg2l ( long m, long n, long k, double *a, long lda, double *tau,
              double *work, long *info )
{
   long i, ii, j, l;

   *info = 0;
   if (m < 0) *info = -1;
   else if (n < 0 || n > m) *info = -2;
   else if (k < 0 || k > n) *info = -3;
   else if (lda < gan_max2(1,m)) *info = -5;
   if (*info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if ( n == 0 ) return GAN_FALSE;

   /*     Initialise columns 1:n-k to columns of the unit matrix */
   for ( j = 0; j < n-k; j++ )
   {
      for ( l = 0; l < m; l++ )
         a[j*lda+l] = 0.0;

      a[j*lda+m-n+j] = 1.0;
   }

   for ( i = 0; i < k; i++ )
   {
      ii = n - k + i;

      /*        Apply H(i) to A(1:m-k+i,1:n-k+i) from the left */
      a[ii*lda+m-n+ii] = 1.0;
      gan_dlarf ( GAN_LEFTMULT, m-n+ii+1, ii, &a[ii*lda], 1, tau[i], a, lda,
                  work );
      gan_dscal ( m-n+ii, -tau[i], &a[ii*lda], 1 );
      a[ii*lda+m-n+ii] = 1.0 - tau[i];

      /*        Set A(m-k+i+1:m,n-k+i) to zero */
      for ( l = m-n+ii+1; l < m; l++ )
         a[ii*lda+l] = 0.0;
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
