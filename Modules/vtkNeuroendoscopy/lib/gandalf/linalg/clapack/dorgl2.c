/**************************************************************************
*
* File:          $RCSfile: dorgl2.c,v $
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
#include <gandalf/linalg/clapack/dorgl2.h>
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
 gan_dorgl2 ( long m, long n, long k, double *a, long lda, double *tau,
              double *work, long *info )
{
   long i, j, l;

   *info = 0;
   if (m < 0) *info = -1;
   else if (n < m) *info = -2;
   else if (k < 0 || k > m) *info = -3;
   else if (lda < gan_max2(1,m)) *info = -5;
   if (*info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if ( m == 0 ) return GAN_FALSE;

   /*     Initialise columns 1:n-k to columns of the unit matrix */
   if ( k <  m)
      /*        Initialise rows k+1:m to rows of the unit matrix */
      for ( j=0; j < n; j++)
      {
         for ( l = k; l < m; l++)
            a[j*lda+l] = 0.0;

         if ( j >= k && j < m )
                a[j*(lda+1)] = 1.0;
      }

   for ( i = k-1; i >= 0; i-- )
   {
      /*        Apply H(i) to A(i:m,i:n) from the right */
      if ( i < n-1 )
      {
         if (i < m-1)
         {
            a[i*(lda+1)] = 1.0;
            gan_dlarf ( GAN_RIGHTMULT, m-i-1, n-i, &a[i*(lda+1)], lda, tau[i],
                        &a[i*lda+i+1], lda, work );
         }

         gan_dscal ( n-i-1, -tau[i], &a[(i+1)*lda+i], lda );
      }

      a[i*(lda+1)] = 1.0 - tau[i];

      /*        Set A(1:i-1,i) to zero */
      for ( l = 0; l < i; l++ )
         a[l*lda+i] = 0.0;
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */
